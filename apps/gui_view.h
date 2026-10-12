#ifndef MINI_CAMERA_RAW_GUI_VIEW_H
#define MINI_CAMERA_RAW_GUI_VIEW_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>

// Display-only transform. No ISP work or scaled image allocation occurs here.
class ImageView final : public QWidget {
 public:
  explicit ImageView(QImage image) : image_(std::move(image)) { fit(); }
  const QImage& image() const { return image_; }
  std::function<void(double,bool)> changed;
  bool isFit() const { return fit_; }
  double zoom() const { return scale()*devicePixelRatioF(); }
  QRectF targetRect() const {
    const double s=scale();
    const QPointF center=boundedCenter(s);
    return {QPointF(width()/2.,height()/2.)-center*s,QSizeF(image_.size())*s};
  }
  QPointF imagePoint(QPointF point) const { return (point-targetRect().topLeft())/scale(); }
  void fit() { fit_=true; center_=QPointF(image_.width()/2.,image_.height()/2.); notify(); }
  void setZoom(double value,QPointF anchor) {
    if(!std::isfinite(value)) return;
    const auto pixel=imagePoint(anchor);
    zoom_=std::clamp(value,.1,16.); fit_=false;
    center_=pixel-(anchor-QPointF(width()/2.,height()/2.))/scale();
    center_=boundedCenter(scale()); notify();
  }
  void panBy(QPointF delta) {
    center_=boundedCenter(scale())-delta/scale();
    center_=boundedCenter(scale()); notify();
  }
  void setImage(QImage image,std::function<void()> painted,bool newSource=false) {
    image_=std::move(image); painted_=std::move(painted);
    if(newSource) fit(); else notify();
  }
 protected:
  void paintEvent(QPaintEvent*) override {
    {
      QPainter painter(this); painter.fillRect(rect(),Qt::darkGray);
      painter.setRenderHint(QPainter::SmoothPixmapTransform,false);
      painter.drawImage(targetRect(),image_);
    }
    if(painted_) {
      auto callback=std::move(painted_); painted_={};
      QTimer::singleShot(0,this,[callback=std::move(callback)] { callback(); });
    }
  }
  void resizeEvent(QResizeEvent* event) override { QWidget::resizeEvent(event); center_=boundedCenter(scale()); notify(); }
  bool event(QEvent* event) override {
    if(event->type()==QEvent::DevicePixelRatioChange) { center_=boundedCenter(scale()); notify(); }
    return QWidget::event(event);
  }
  void wheelEvent(QWheelEvent* event) override {
    const double steps=!event->pixelDelta().isNull()?event->pixelDelta().y()/120.:event->angleDelta().y()/120.;
    setZoom(zoom()*std::pow(1.2,std::clamp(steps,-20.,20.)),event->position()); event->accept();
  }
  void mousePressEvent(QMouseEvent* event) override {
    if(event->button()==Qt::LeftButton) { dragging_=true; last_=event->position(); setCursor(Qt::ClosedHandCursor); event->accept(); }
    else QWidget::mousePressEvent(event);
  }
  void mouseMoveEvent(QMouseEvent* event) override {
    if(dragging_ && (event->buttons()&Qt::LeftButton)) { panBy(event->position()-last_); last_=event->position(); event->accept(); }
    else QWidget::mouseMoveEvent(event);
  }
  void mouseReleaseEvent(QMouseEvent* event) override {
    if(event->button()==Qt::LeftButton) { dragging_=false; unsetCursor(); event->accept(); }
    else QWidget::mouseReleaseEvent(event);
  }
 private:
  double scale() const {
    if(fit_) return std::max(1e-9,std::min(width()/double(image_.width()),height()/double(image_.height())));
    return zoom_/devicePixelRatioF();
  }
  QPointF boundedCenter(double s) const {
    auto bound=[](double c,double extent,double viewport) {
      return extent<=viewport?extent/2:std::clamp(c,viewport/2,extent-viewport/2);
    };
    return {bound(center_.x(),image_.width(),width()/s),bound(center_.y(),image_.height(),height()/s)};
  }
  void notify() { update(); if(changed) changed(zoom(),fit_); }
  QImage image_;
  QPointF center_,last_;
  double zoom_=1;
  bool fit_=true,dragging_=false;
  std::function<void()> painted_;
};
#endif
