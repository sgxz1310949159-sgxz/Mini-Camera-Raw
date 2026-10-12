#include "editor_session.h"
#include <gtest/gtest.h>
namespace mini_camera_raw::app {
namespace {
DecodedRaw source() {
  SensorMetadata sensor{};
  sensor.levels={{0,0,0,0},{4096,4096,4096,4096}};
  sensor.camera_wb_tile=std::array<double,4>{2,1,1,1.5};
  sensor.camera_to_srgb=std::array<double,12>{1,0,0,0,0,1,0,0,0,0,1,0};
  sensor.orientation=6;
  return {ImageBuffer::from_uint16({3,3,1,3,PixelFormat::kUInt16,CfaPattern::kRggb,
      ColorState::kRawBayer,{0,4096,false}}, {0,512,1024,1536,2048,2560,3072,3584,4096}),sensor};
}
void equal(const ImageBuffer& a,const ImageBuffer& b) {
  ASSERT_EQ(a.element_count(),b.element_count());
  for(std::size_t i=0;i<a.element_count();++i) EXPECT_EQ(a.uint16_data()[i],b.uint16_data()[i]);
}
TEST(EditorSessionTest, ReusesSourceAcrossExposureResetAndCacheRebuild) {
  auto raw=source();
  auto reference=prepare_editor_photo(raw);
  EditorSession session(std::move(raw));
  EXPECT_EQ(session.retained_bytes(),9U*14);
  for(double ev:{0.,2.,-1.,0.}) equal(session.render(ev),render_editor_photo(reference,ev));
  const auto before=session.render(-1);
  session.rebuild_cache();
  equal(before,session.render(-1));
  equal(session.render(0),render_editor_photo(reference,0));
}
TEST(EditorSessionTest, DefaultCropPreservesDemosaicNeighborsAndRotatesAfterCrop) {
  auto raw=source(); raw.sensor.orientation=0;
  auto full=prepare_editor_photo(raw);
  const auto original=raw.image.uint16_at(0,0,0);
  raw.sensor.default_crop=ImageCrop{1,0,2,3};
  auto cropped=prepare_editor_photo(raw);
  EXPECT_EQ(cropped.camera.metadata().width,2U);
  for(size_t y=0;y<3;++y) for(size_t x=0;x<2;++x) for(size_t c=0;c<3;++c)
    EXPECT_FLOAT_EQ(cropped.camera.float_at(y,x,c),full.camera.float_at(y,x+1,c));
  EXPECT_EQ(raw.image.uint16_at(0,0,0),original);
  for(int flip:{0,3,5,6}) {
    raw.sensor.orientation=flip;
    EditorSession session(raw);
    auto expected=render_editor_output(cropped.camera,cropped.matrix,0);
    equal(session.render(0),orient_editor_output(std::move(expected),flip));
    auto before=session.render(-1); session.rebuild_cache(); equal(before,session.render(-1));
  }
  for(auto crop:{ImageCrop{1,0,3,3},{0,0,0,2},{0,3,1,1},{0,0,1,4}}) {
    raw.sensor.default_crop=crop; EXPECT_THROW(prepare_editor_photo(raw),std::invalid_argument);
  }
}
TEST(EditorSessionTest, RenderFailureDoesNotDamageSession) {
  EditorSession session(source());
  auto before=session.render(0);
  EXPECT_THROW(session.render(21),std::invalid_argument);
  equal(before,session.render(0));
  auto output=session.render(0); output.uint16_data()[0]=65535;
  equal(before,session.render(0));
}
}
}
