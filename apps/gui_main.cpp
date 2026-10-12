#include "gui_image.h"
#include "gui_view.h"
#include "editor_session.h"
#include "editor_queue.h"
#include "mini_camera_raw/png_writer.h"
#include <optional>
#include <QApplication>
#include <QColorSpace>
#include <QDebug>
#include <QMainWindow>
#include <QPainter>
#include <QSurfaceFormat>
#include <QTimer>
#include <QWindow>
#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolBar>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QLabel>
#include <QSignalBlocker>
#include <chrono>
#include <future>
#include <functional>
#include <memory>
#include <utility>

namespace {
using mini_camera_raw::app::EditorQueue;
using mini_camera_raw::app::EditRequest;
using mini_camera_raw::app::EditorSession;
using Clock=std::chrono::steady_clock;
QImage demonstration_image() {
  using namespace mini_camera_raw;
  const ImageMetadata m{3,2,3,10,PixelFormat::kUInt16,CfaPattern::kNone,
      ColorState::kEncodedRgb,{0,65535,false},RgbColorSpace::kSrgb,TransferFunction::kSrgb};
  return make_gui_image(ImageBuffer::from_uint16(m,
      {65535,0,0,0,65535,0,0,0,65535,123,65535,65535,65535,32768,32768,32768,0,0,0,456}));
}
struct RenderResult {
  std::shared_ptr<const EditorSession> session;
  QImage image;
  double seconds;
};
struct ExportSnapshot {
  std::shared_ptr<const EditorSession> session;
  std::uint64_t sessionId;
  double ev;
  QString path;
};
class EditorWindow final : public QMainWindow {
 public:
  explicit EditorWindow(bool smoke) : smoke_(smoke) {
    setWindowTitle(QStringLiteral("Mini-Camera Raw — P9"));
    view_=new ImageView(demonstration_image()); setCentralWidget(view_);
    auto* bar=addToolBar(QStringLiteral("照片与曝光")); bar->setMovable(false);
    open_=bar->addAction(QStringLiteral("打开 RAW"));
    connect(open_,&QAction::triggered,this,[this] {
      auto path=QFileDialog::getOpenFileName(this,QStringLiteral("打开 RAW"),{},QStringLiteral("Sony RAW (*.ARW *.arw)"));
      if(!path.isEmpty()) openPhoto(path);
    });
    export_=bar->addAction(QStringLiteral("导出 PNG"));
    connect(export_,&QAction::triggered,this,[this] {
      if(!freezeExport()) return;
      const auto path=QFileDialog::getSaveFileName(this,QStringLiteral("导出 RGB16 PNG（不覆盖已有文件）"),
          QStringLiteral("export.png"),QStringLiteral("PNG (*.png)"),nullptr,QFileDialog::DontConfirmOverwrite);
      if(path.isEmpty()) { exportSnapshot_.reset(); refresh(); finishOrContinue(); return; }
      exportSnapshot_->path=path; startExport();
    });
    bar->addWidget(new QLabel(QStringLiteral("  请求曝光 ")));
    ev_=new QDoubleSpinBox; ev_->setObjectName("exposure");
    ev_->setRange(-5,5); ev_->setDecimals(2); ev_->setSingleStep(.01);
    ev_->setSuffix(QStringLiteral(" EV")); ev_->setKeyboardTracking(false); bar->addWidget(ev_);
    slider_=new QSlider(Qt::Horizontal); slider_->setObjectName("exposureSlider");
    slider_->setRange(-500,500); slider_->setSingleStep(1); slider_->setMinimumWidth(160);
    bar->addWidget(slider_);
    reset_=bar->addAction(QStringLiteral("复位"));
    retry_=bar->addAction(QStringLiteral("重试"));
    discard_=bar->addAction(QStringLiteral("放弃失败及后续项"));
    connect(ev_,&QDoubleSpinBox::valueChanged,this,[this](double value) { requestExposure(value); });
    connect(slider_,&QSlider::valueChanged,this,[this](int value) { requestExposure(value/100.); });
    connect(reset_,&QAction::triggered,this,[this] { requestExposure(0,true); });
    connect(retry_,&QAction::triggered,this,[this] {
      if(openError_) openPhoto(lastPath_);
      else if(queue_.phase()==EditorQueue::Phase::failed) { queue_.retry(); error_.clear(); startNext(); }
    });
    connect(discard_,&QAction::triggered,this,[this] {
      if(openError_) { openError_=false; error_.clear(); }
      else if(queue_.phase()==EditorQueue::Phase::failed) { queue_.discard_failed(); exportSnapshot_.reset(); error_.clear(); }
      syncControls(); refresh();
    });
    addToolBarBreak();
    auto* viewing=addToolBar(QStringLiteral("查看")); viewing->setMovable(false);
    auto* fit=viewing->addAction(QStringLiteral("适应视图"));
    auto* actual=viewing->addAction(QStringLiteral("100%"));
    auto* minus=viewing->addAction(QStringLiteral("缩小"));
    auto* plus=viewing->addAction(QStringLiteral("放大"));
    auto* zoomLabel=new QLabel; viewing->addWidget(zoomLabel);
    view_->changed=[zoomLabel](double zoom,bool fit) {
      zoomLabel->setText(QStringLiteral("  %1% %2").arg(zoom*100,0,'f',1).arg(fit?QStringLiteral("（适应）"):QString()));
    };
    connect(fit,&QAction::triggered,this,[this] { view_->fit(); });
    connect(actual,&QAction::triggered,this,[this] { view_->setZoom(1,QPointF(view_->width()/2.,view_->height()/2.)); });
    connect(minus,&QAction::triggered,this,[this] { view_->setZoom(view_->zoom()/1.2,QPointF(view_->width()/2.,view_->height()/2.)); });
    connect(plus,&QAction::triggered,this,[this] { view_->setZoom(view_->zoom()*1.2,QPointF(view_->width()/2.,view_->height()/2.)); });
    timer_.setInterval(30); connect(&timer_,&QTimer::timeout,this,[this] { collect(); });
    resize(1120,720); refresh();
  }
  void openPhoto(const QString& path) {
    if(pending_.valid() || opening_ || queue_.size() || exportSnapshot_ || closing_) return;
    opening_=true; openError_=false; error_.clear(); exportError_.clear(); exportNotice_.clear(); lastPath_=path; refresh();
    try {
      pending_=std::async(std::launch::async,[path=path.toStdString()] {
        const auto start=Clock::now();
        auto session=std::make_shared<EditorSession>(mini_camera_raw::decode_raw(path));
        auto image=mini_camera_raw::make_gui_image(session->render(0));
        return RenderResult{std::move(session),std::move(image),std::chrono::duration<double>(Clock::now()-start).count()};
      });
      timer_.start();
    } catch(const std::exception& error) { opening_=false; openError_=true; error_=QString::fromUtf8(error.what()); refresh(); }
  }
  void requestExposure(double value,bool explicitReset=false) {
    if(!session_ || opening_ || openError_ || closing_ || exportSnapshot_ ||
        (!explicitReset && value==queue_.requested_ev())) { syncControls(); return; }
    if(!queue_.accept(value)) { syncControls(); refresh(); return; }
    exportError_.clear(); exportNotice_.clear();
    qInfo()<<"accepted EV"<<value<<"unfinished"<<queue_.size();
    syncControls(); startNext();
  }
  void exportPhoto(const QString& path) {
    if(path.isEmpty() || !freezeExport()) return;
    exportSnapshot_->path=path; startExport();
  }
#ifdef MINI_CAMERA_RAW_GUI_TEST
  ImageView* testView() const { return view_; }
  bool testExportError() const { return !exportError_.isEmpty(); }
  QImage testImage() const { return view_->image(); }
  const EditorQueue& testQueue() const { return queue_; }
  bool testReady() const { return session_ && !opening_ && !pending_.valid() && !queue_.size() && !exportSnapshot_; }
  void testFailNext() { failNext_=true; }
  void testRetry() { retry_->trigger(); }
  void testDiscard() { discard_->trigger(); }
  bool testOpenError() const { return openError_; }
#endif
 protected:
  void closeEvent(QCloseEvent* event) override {
    if(exportPending_.valid() || pending_.valid() || opening_ || (exportSnapshot_ && queue_.phase()!=EditorQueue::Phase::failed) || (queue_.size() && queue_.phase()!=EditorQueue::Phase::failed)) {
      closing_=true; refresh(); event->ignore();
    } else event->accept();
  }
 private:
  bool freezeExport() {
    if(!session_ || opening_ || openError_ || closing_ || exportSnapshot_ || queue_.phase()==EditorQueue::Phase::failed) return false;
    exportSnapshot_=ExportSnapshot{session_,queue_.displayed().session,queue_.requested_ev(),{}};
    exportError_.clear(); exportNotice_.clear(); refresh(); return true;
  }
  void startExport() {
    if(!exportSnapshot_ || exportSnapshot_->path.isEmpty() || exportPending_.valid() || pending_.valid() || opening_ || queue_.size()) { refresh(); return; }
    try {
      const auto snapshot=*exportSnapshot_;
      exportPending_=std::async(std::launch::async,[snapshot] {
        const auto start=Clock::now();
        mini_camera_raw::write_png16(snapshot.session->render(snapshot.ev),snapshot.path.toStdString());
        qInfo()<<"exported session"<<snapshot.sessionId<<"EV"<<snapshot.ev
               <<"seconds"<<std::chrono::duration<double>(Clock::now()-start).count();
      });
      timer_.start();
    } catch(const std::exception& error) {
      exportError_=QString::fromUtf8(error.what()); exportSnapshot_.reset();
      if(closing_) close();
    }
    refresh();
  }
  void syncControls() {
    const QSignalBlocker spinBlock(ev_), sliderBlock(slider_);
    ev_->setValue(queue_.requested_ev()); slider_->setValue(qRound(queue_.requested_ev()*100));
  }
  void refresh() {
    const bool failed=queue_.phase()==EditorQueue::Phase::failed;
    const bool editable=session_ && !opening_ && !openError_ && !closing_ && !exportSnapshot_ && queue_.can_accept();
    ev_->setEnabled(editable); slider_->setEnabled(editable); reset_->setEnabled(editable);
    open_->setEnabled(!opening_ && !pending_.valid() && !queue_.size() && !closing_ && !exportSnapshot_);
    export_->setEnabled(session_ && !opening_ && !openError_ && !closing_ && !failed && !exportSnapshot_);
    retry_->setEnabled((failed || openError_) && !closing_);
    discard_->setEnabled((failed || openError_) && !closing_);
    QString state;
    if(closing_) state=QStringLiteral("等待已接受操作完成后关闭");
    else if(opening_) state=QStringLiteral("正在打开并处理 RAW");
    else if(!error_.isEmpty()) state=QStringLiteral("失败：%1；保留上次成功画面，队列已暂停").arg(error_);
    else if(exportSnapshot_) state=exportPending_.valid()?QStringLiteral("正在导出 RGB16 PNG"):QStringLiteral("导出已锁定 %1 EV，等待选择路径或预览完成").arg(exportSnapshot_->ev,0,'f',2);
    else if(!exportError_.isEmpty()) state=QStringLiteral("导出失败：%1；请重新导出并选择新路径").arg(exportError_);
    else if(!exportNotice_.isEmpty() && !queue_.size()) state=exportNotice_;
    else if(!session_) state=QStringLiteral("请选择照片；当前为合成显示验证图");

    else if(queue_.phase()==EditorQueue::Phase::awaiting_paint) state=QStringLiteral("等待窗口绘制");
    else state=queue_.size()?QStringLiteral("正在处理"):QStringLiteral("就绪");
    if(session_) {
      const auto shown=queue_.displayed();
      state+=QStringLiteral(" | 已显示 #%1：%2 EV | 请求：%3 EV | 未完成 %4/2（优先最新值）")
          .arg(shown.sequence).arg(shown.ev,0,'f',2).arg(queue_.requested_ev(),0,'f',2).arg(queue_.size());
    }
    statusBar()->showMessage(state);
  }
  void startNext() {
    if(pending_.valid() || exportPending_.valid()) return;
    auto request=queue_.begin();
    if(!request) { refresh(); return; }
    active_=*request;
    qInfo()<<"started"<<active_.session<<active_.sequence<<active_.ev;
    bool fail=false;
#ifdef MINI_CAMERA_RAW_GUI_TEST
    fail=failNext_; failNext_=false;
#endif
    try {
      pending_=std::async(std::launch::async,[session=session_,request=*request,fail] {
        if(fail) throw std::runtime_error("injected render failure");
        const auto start=Clock::now();
        auto image=mini_camera_raw::make_gui_image(session->render(request.ev));
        return RenderResult{session,std::move(image),std::chrono::duration<double>(Clock::now()-start).count()};
      });
      timer_.start();
    } catch(const std::exception& error) { queue_.fail(active_); error_=QString::fromUtf8(error.what()); }
    refresh();
  }
  void collect() {
    if(exportPending_.valid()) {
      if(exportPending_.wait_for(std::chrono::seconds(0))!=std::future_status::ready) return;
      timer_.stop();
      try {
        exportPending_.get();
        exportNotice_=QStringLiteral("已导出 %1 EV：%2").arg(exportSnapshot_->ev,0,'f',2).arg(exportSnapshot_->path);
      } catch(const std::exception& error) { exportError_=QString::fromUtf8(error.what()); }
      exportSnapshot_.reset(); refresh(); finishOrContinue(); return;
    }
    if(!pending_.valid() || pending_.wait_for(std::chrono::seconds(0))!=std::future_status::ready) return;
    timer_.stop();
    try {
      auto result=pending_.get();
      qInfo()<<"render seconds"<<result.seconds<<"retained bytes"<<result.session->retained_bytes();
      if(opening_) {
        // Commit the new source only after its complete frame exists.
        std::function<void()> painted=[this] {
          opening_=false; qInfo()<<"painted initial"<<queue_.displayed().session;
          refresh(); finishOrContinue();
        };
        queue_.new_session(); session_=std::move(result.session); syncControls();
        view_->setImage(std::move(result.image),std::move(painted),true);
      } else {
        if(result.session!=session_) throw std::logic_error("mismatched render session");
        const auto request=active_;
        std::function<void()> painted=[this,request] {
          queue_.painted(request);
          qInfo()<<"painted"<<request.session<<request.sequence<<request.ev;
          refresh(); finishOrContinue();
        };
        queue_.rendered(active_);
        view_->setImage(std::move(result.image),std::move(painted));
      }
    } catch(const std::exception& error) {
      if(opening_) { opening_=false; openError_=true; }
      else queue_.fail(active_);
      error_=QString::fromUtf8(error.what()); qWarning()<<"editor failed:"<<error.what();
      if(smoke_) QApplication::exit(2);
      if(closing_) close();
    }
    refresh();
  }
  void finishOrContinue() {
    if(exportSnapshot_ && !queue_.size()) { startExport(); return; }
    if(closing_ && !queue_.size()) { close(); return; }
    if(smoke_ && !queue_.size()) { QTimer::singleShot(200,this,[] { QApplication::exit(0); }); return; }
    startNext();
  }
  ImageView* view_=nullptr;
  QDoubleSpinBox* ev_=nullptr;
  QSlider* slider_=nullptr;
  QAction *open_=nullptr,*reset_=nullptr,*retry_=nullptr,*discard_=nullptr,*export_=nullptr;
  QTimer timer_;
  std::future<RenderResult> pending_;
  std::future<void> exportPending_;
  std::optional<ExportSnapshot> exportSnapshot_;
  QString exportError_,exportNotice_;
  std::shared_ptr<const EditorSession> session_;
  EditorQueue queue_;
  EditRequest active_{0,0,0};
  QString error_,lastPath_;
  bool smoke_=false,opening_=false,openError_=false,closing_=false;
#ifdef MINI_CAMERA_RAW_GUI_TEST
  bool failNext_=false;
#endif
};
}

#ifndef MINI_CAMERA_RAW_GUI_TEST
int main(int argc, char* argv[]) {
  // Qt 6.11 Cocoa maps this explicit surface space to its NSView/backing store.
  QSurfaceFormat format;
  format.setColorSpace(QColorSpace::SRgb);
  QSurfaceFormat::setDefaultFormat(format);
  QApplication application(argc, argv);
  QApplication::setApplicationName(QStringLiteral("Mini-Camera Raw"));
  const bool smoke=application.arguments().contains(QStringLiteral("--smoke-test"));
  EditorWindow window(smoke);
  window.show();
  QString rawPath;
  for (const auto& arg : application.arguments().mid(1))
    if (!arg.startsWith(QStringLiteral("--"))) rawPath=arg;
  if (!rawPath.isEmpty()) window.openPhoto(rawPath);
  if (smoke && rawPath.isEmpty()) {
    QTimer::singleShot(1500, &window, [&] {
      const auto* handle = window.windowHandle();
      const bool ready = handle && handle->isExposed() &&
          handle->format().colorSpace() == QColorSpace(QColorSpace::SRgb);
      qInfo() << "GUI smoke:" << ready << "Qt:" << qVersion()
              << "platform:" << application.platformName()
              << "DPR:" << window.devicePixelRatioF();
      application.exit(ready ? 0 : 2);
    });
  }
  return application.exec();
}

#endif
