// Reuse the actual window with test-only failure injection; decoder is synthetic.
#define MINI_CAMERA_RAW_GUI_TEST
#include "../apps/gui_main.cpp"
#include <gtest/gtest.h>
#include <QThread>
#include <QTemporaryDir>
#include <QFile>
namespace {
bool until(const std::function<bool()>& predicate,int milliseconds=3000) {
  const auto end=Clock::now()+std::chrono::milliseconds(milliseconds);
  while(Clock::now()<end) {
    QApplication::processEvents();
    if(predicate()) return true;
    QThread::msleep(2);
  }
  return predicate();
}
TEST(GuiViewTest, PhysicalScaleAnchorAndPanBoundaries) {
  ImageView view(QImage(4000,3000,QImage::Format_RGB32)); view.resize(800,600);
  view.show(); QApplication::processEvents();
  view.setZoom(1, QPointF(400,300));
  EXPECT_NEAR(view.targetRect().width()*view.devicePixelRatioF(),4000,1e-8);
  for(double zoom : {2.,4.,16.}) {
    const QPointF anchor(350,270);
    auto pixel=view.imagePoint(anchor);
    view.setZoom(zoom,anchor);
    EXPECT_NEAR(view.imagePoint(anchor).x(),pixel.x(),1e-8);
    EXPECT_NEAR(view.imagePoint(anchor).y(),pixel.y(),1e-8);
    EXPECT_NEAR(view.targetRect().width()*view.devicePixelRatioF(),4000*zoom,1e-8);
  }
  view.panBy(QPointF(1e8,1e8));
  EXPECT_NEAR(view.targetRect().left(),0,1e-8);
  EXPECT_NEAR(view.targetRect().top(),0,1e-8);
  view.panBy(QPointF(-1e8,-1e8));
  EXPECT_NEAR(view.targetRect().right(),800,1e-8);
  EXPECT_NEAR(view.targetRect().bottom(),600,1e-8);
  view.fit(); EXPECT_NEAR(view.targetRect().width(),800,1e-8);
}
TEST(GuiViewTest, WheelAndDragEventsUpdateViewWithoutChangingPixels) {
  ImageView view(QImage(4000,3000,QImage::Format_RGB32)); view.resize(800,600);
  view.show(); QApplication::processEvents(); view.setZoom(1,QPointF(400,300));
  const auto key=view.image().cacheKey(); const QPointF anchor(350,270);
  const auto pixel=view.imagePoint(anchor);
  QWheelEvent wheel(anchor,anchor,QPoint(),QPoint(0,120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
  QApplication::sendEvent(&view,&wheel);
  EXPECT_NEAR(view.zoom(),1.2,1e-8);
  EXPECT_NEAR(view.imagePoint(anchor).x(),pixel.x(),1e-8);
  const auto before=view.targetRect();
  QMouseEvent press(QEvent::MouseButtonPress,anchor,anchor,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
  QApplication::sendEvent(&view,&press);
  const auto to=anchor+QPointF(40,30);
  QMouseEvent move(QEvent::MouseMove,to,to,Qt::NoButton,Qt::LeftButton,Qt::NoModifier);
  QApplication::sendEvent(&view,&move);
  QMouseEvent release(QEvent::MouseButtonRelease,to,to,Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
  QApplication::sendEvent(&view,&release);
  EXPECT_NEAR(view.targetRect().left()-before.left(),40,1e-8);
  EXPECT_NEAR(view.targetRect().top()-before.top(),30,1e-8);
  EXPECT_EQ(view.image().cacheKey(),key);
  view.setZoom(100,anchor); EXPECT_DOUBLE_EQ(view.zoom(),16);
  view.setZoom(.001,anchor); EXPECT_DOUBLE_EQ(view.zoom(),.1);
}
TEST(GuiViewTest, EditingKeepsViewAndNewSourceFits) {
  ImageView view(QImage(4000,3000,QImage::Format_RGB32)); view.resize(800,600);
  view.show(); QApplication::processEvents(); view.setZoom(4,QPointF(400,300));
  view.panBy(QPointF(-50,-30)); const auto before=view.targetRect();
  view.setImage(QImage(4000,3000,QImage::Format_RGB32),{});
  EXPECT_EQ(view.targetRect(),before);
  view.setImage(QImage(3000,4000,QImage::Format_RGB32),{},true);
  EXPECT_TRUE(view.isFit()); EXPECT_NEAR(view.targetRect().height(),600,1e-8);
}
TEST(GuiExportTest, WaitsForLatestSnapshotAndDecodesExactlyWithoutOverwrite) {
  QTemporaryDir dir; ASSERT_TRUE(dir.isValid()); const auto path=dir.filePath("result.png");
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  window.requestExposure(1); window.requestExposure(-1);
  window.exportPhoto(path); window.requestExposure(3);
  EXPECT_FALSE(window.findChild<QDoubleSpinBox*>("exposure")->isEnabled());
  EXPECT_DOUBLE_EQ(window.testQueue().requested_ev(),-1);
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  const auto expected=mini_camera_raw::make_gui_image(EditorSession(mini_camera_raw::decode_raw("synthetic")).render(-1));
  QImage decoded(path); ASSERT_FALSE(decoded.isNull());
  EXPECT_EQ(decoded.size(),expected.size());
  EXPECT_EQ(decoded.convertToFormat(QImage::Format_RGBX64),expected);
  EXPECT_EQ(decoded.colorSpace(),QColorSpace(QColorSpace::SRgb));
  QFile file(path); ASSERT_TRUE(file.open(QIODevice::ReadOnly)); const auto bytes=file.readAll(); file.close();
  ASSERT_GT(bytes.size(),26); EXPECT_EQ(static_cast<unsigned char>(bytes[24]),16);
  EXPECT_EQ(static_cast<unsigned char>(bytes[25]),2);
  window.exportPhoto(path); ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_TRUE(window.testExportError());
  ASSERT_TRUE(file.open(QIODevice::ReadOnly)); EXPECT_EQ(file.readAll(),bytes);
  EXPECT_EQ(window.testImage(),expected);
  window.exportPhoto(dir.filePath("retry.png")); ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_FALSE(window.testExportError());
}
TEST(GuiExportTest, PreviewFailureRequiresRetryOrDiscardsPendingExport) {
  QTemporaryDir dir; const auto path=dir.filePath("result.png");
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  window.testFailNext(); window.requestExposure(-1); window.exportPhoto(path);
  ASSERT_TRUE(until([&] { return window.testQueue().phase()==EditorQueue::Phase::failed; }));
  EXPECT_FALSE(QFile::exists(path)); window.testRetry();
  ASSERT_TRUE(until([&] { return window.testReady(); })); EXPECT_TRUE(QFile::exists(path));
  const auto cancelled=dir.filePath("cancelled.png");
  window.testFailNext(); window.requestExposure(1); window.exportPhoto(cancelled);
  ASSERT_TRUE(until([&] { return window.testQueue().phase()==EditorQueue::Phase::failed; }));
  window.testDiscard(); ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_FALSE(QFile::exists(cancelled));
}
TEST(GuiExportTest, PortraitOutputAndIoFailureRetainState) {
  QTemporaryDir dir;
  EditorWindow window(false); window.show(); window.openPhoto("synthetic-portrait");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  ASSERT_EQ(window.testImage().size(),QSize(3,4));
  const auto initial=window.testImage();
  window.exportPhoto(dir.filePath("missing/result.png"));
  ASSERT_TRUE(until([&] { return window.testReady(); })); EXPECT_TRUE(window.testExportError());
  EXPECT_EQ(window.testImage(),initial);
  const auto path=dir.filePath("portrait.png"); window.exportPhoto(path);
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_EQ(QImage(path).convertToFormat(QImage::Format_RGBX64),initial);
}
TEST(GuiEditorTest, ZoomAndPanDoNotQueueEditsAndResetKeepsView) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  auto* view=window.testView(); view->setZoom(4,QPointF(400,300)); view->panBy(QPointF(20,10));
  const auto before=view->targetRect(); EXPECT_EQ(window.testQueue().displayed().sequence,0U);
  window.requestExposure(1); window.requestExposure(0,true);
  ASSERT_TRUE(until([&] { return window.testReady(); })); EXPECT_EQ(view->targetRect(),before);
  window.openPhoto("synthetic"); ASSERT_TRUE(until([&] { return window.testReady(); })); EXPECT_TRUE(view->isFit());
}
TEST(GuiExportTest, CloseWaitsForAcceptedExport) {
  QTemporaryDir dir; const auto path=dir.filePath("close.png");
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  window.requestExposure(-1); window.exportPhoto(path); window.close();
  ASSERT_TRUE(until([&] { return !window.isVisible(); }));
  EXPECT_TRUE(QFile::exists(path));
}
TEST(GuiEditorTest, LatestExposureAndResetReturnExactInitialFrame) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  const auto initial=window.testImage();
  window.requestExposure(2); window.requestExposure(-1); window.requestExposure(0,true);
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_EQ(window.testQueue().displayed().sequence,3U);
  EXPECT_EQ(window.testQueue().displayed().ev,0);
  EXPECT_EQ(window.testImage(),initial);
}
TEST(GuiEditorTest, HundredthEvStepsStaySynchronizedAndVisible) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  auto* spin=window.findChild<QDoubleSpinBox*>("exposure");
  auto* slider=window.findChild<QSlider*>("exposureSlider");
  EXPECT_DOUBLE_EQ(spin->minimum(),-5); EXPECT_DOUBLE_EQ(spin->maximum(),5);
  EXPECT_EQ(slider->minimum(),-500); EXPECT_EQ(slider->maximum(),500);
  spin->stepUp();
  EXPECT_DOUBLE_EQ(window.testQueue().requested_ev(),.01);
  EXPECT_EQ(slider->value(),1);
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_TRUE(window.statusBar()->currentMessage().contains("0.01 EV"));
  slider->setValue(-1);
  EXPECT_DOUBLE_EQ(spin->value(),-.01);
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_DOUBLE_EQ(window.testQueue().displayed().ev,-.01);
  EXPECT_TRUE(window.statusBar()->currentMessage().contains("-0.01 EV"));
}
TEST(GuiEditorTest, RapidInputKeepsLatestWithoutDisablingAndStillWaitsForPaint) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); })); window.hide();
  for(int n=1;n<=100;++n) window.requestExposure(n/100.);
  EXPECT_EQ(window.testQueue().size(),2U);
  EXPECT_DOUBLE_EQ(window.testQueue().requested_ev(),1.);
  EXPECT_TRUE(window.findChild<QDoubleSpinBox*>("exposure")->isEnabled());
  EXPECT_TRUE(window.findChild<QSlider*>("exposureSlider")->isEnabled());
  ASSERT_TRUE(until([&] { return window.testQueue().phase()==EditorQueue::Phase::awaiting_paint; }));
  EXPECT_EQ(window.testQueue().displayed().sequence,0U);
  window.show();
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_EQ(window.testQueue().displayed().sequence,100U);
  EXPECT_TRUE(window.findChild<QDoubleSpinBox*>("exposure")->isEnabled());
}
TEST(GuiEditorTest, FailureRetryDiscardAndFailedOpenPreserveSuccessfulFrame) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  auto initial=window.testImage();
  window.testFailNext(); window.requestExposure(1); window.requestExposure(2);
  ASSERT_TRUE(until([&] { return window.testQueue().phase()==EditorQueue::Phase::failed; }));
  EXPECT_EQ(window.testImage(),initial); EXPECT_EQ(window.testQueue().displayed().sequence,0U);
  window.testRetry(); ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_EQ(window.testQueue().displayed().sequence,2U);
  auto last=window.testImage();
  window.testFailNext(); window.requestExposure(-2); window.requestExposure(0,true);
  ASSERT_TRUE(until([&] { return window.testQueue().phase()==EditorQueue::Phase::failed; }));
  window.testDiscard(); EXPECT_TRUE(window.testReady()); EXPECT_EQ(window.testImage(),last);
  EXPECT_EQ(window.testQueue().requested_ev(),2);
  window.openPhoto("does-not-exist"); ASSERT_TRUE(until([&] { return window.testOpenError(); }));
  EXPECT_EQ(window.testImage(),last); EXPECT_EQ(window.testQueue().displayed().ev,2);
  window.testDiscard(); window.requestExposure(0,true);
  ASSERT_TRUE(until([&] { return window.testReady(); })); EXPECT_EQ(window.testImage(),initial);
  const auto oldSession=window.testQueue().displayed().session;
  window.openPhoto("synthetic"); ASSERT_TRUE(until([&] { return window.testReady(); }));
  EXPECT_GT(window.testQueue().displayed().session,oldSession);
  EXPECT_EQ(window.testQueue().displayed().sequence,0U);
}
TEST(GuiEditorTest, ClosingDrainsAlreadyAcceptedWork) {
  EditorWindow window(false); window.show(); window.openPhoto("synthetic");
  ASSERT_TRUE(until([&] { return window.testReady(); }));
  window.requestExposure(1); window.requestExposure(0,true); window.close();
  EXPECT_TRUE(window.isVisible());
  ASSERT_TRUE(until([&] { return !window.isVisible(); }));
  EXPECT_EQ(window.testQueue().displayed().sequence,2U);
}
}
int main(int argc,char**argv) {
  if(qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) qputenv("QT_QPA_PLATFORM","offscreen");
  QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false);
  ::testing::InitGoogleTest(&argc,argv); return RUN_ALL_TESTS();
}
