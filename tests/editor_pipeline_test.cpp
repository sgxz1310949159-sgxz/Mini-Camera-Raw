#include "editor_pipeline.h"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw::app {
namespace {
const CameraToWorkingMatrix identity{{1,0,0, 0,1,0, 0,0,1}, RgbColorSpace::kSrgb};
ImageBuffer camera() {
  return ImageBuffer::from_float({2,1,3,7,PixelFormat::kFloat32,CfaPattern::kNone,
      ColorState::kLinearCameraRgb,{0,1,true}},
      {1.2f,1.6f,-.2f, .18f,0,2, std::numeric_limits<float>::quiet_NaN()});
}
TEST(EditorPipelineTest, PreservesHeadroomNegativesAndLinearState) {
  const auto in=camera();
  const auto out=render_editor_linear(in,identity,-1);
  EXPECT_EQ(out.metadata().color_state,ColorState::kLinearWorkingRgb);
  EXPECT_NEAR(out.float_at(0,0,0),.6,1e-7);
  EXPECT_NEAR(out.float_at(0,0,1),.8,1e-7);
  EXPECT_NEAR(out.float_at(0,0,2),-.1,1e-7);
  EXPECT_FLOAT_EQ(in.float_at(0,0,0),1.2f);
  EXPECT_TRUE(std::isnan(in.float_row(0)[6]));
}
TEST(EditorPipelineTest, MatchesIndependentEncodedOracleForRepeatedAdjustments) {
  const auto in=camera();
  for (double ev : {0.,2.,-1.,0.}) {
    const auto out=render_editor_output(in,identity,ev);
    for (int x=0;x<2;++x) for (int c=0;c<3;++c) {
      double v=std::max(0.,std::min(1.,double(in.float_at(0,x,c))*std::exp2(ev)));
      double s=v<=.0031308 ? 12.92*v : 1.055*std::pow(v,1./2.4)-.055;
      EXPECT_NEAR(out.uint16_at(0,x,c),std::floor(s*65535+.5),1);
    }
    EXPECT_EQ(out.metadata().color_state,ColorState::kEncodedRgb);
  }
}
TEST(EditorPipelineTest, RejectsInvalidParametersAndActiveSamples) {
  auto in=camera();
  EXPECT_THROW(render_editor_output(in,identity,21),std::invalid_argument);
  EXPECT_THROW(render_editor_output(in,identity,std::numeric_limits<double>::quiet_NaN()),std::invalid_argument);
  in.float_at(0,0,0)=std::numeric_limits<float>::infinity();
  EXPECT_THROW(render_editor_output(in,identity,0),std::invalid_argument);
}
TEST(EditorOrientationTest, ExactNonSquarePermutationAllDirections) {
  auto m=ImageMetadata{3,2,3,10,PixelFormat::kUInt16,CfaPattern::kNone,
      ColorState::kEncodedRgb,{0,65535,false},RgbColorSpace::kSrgb,TransferFunction::kSrgb};
  auto src=ImageBuffer::from_uint16(m,{1,11,21,2,12,22,3,13,23,999,
                                      4,14,24,5,15,25,6,16,26,999});
  const int flips[]={0,3,5,6};
  const int expected[4][6]={{1,2,3,4,5,6},{6,5,4,3,2,1},{3,6,2,5,1,4},{4,1,5,2,6,3}};
  for (int f=0;f<4;++f) {
    const auto out=orient_editor_output(src,flips[f]);
    EXPECT_EQ(out.metadata().width,f<2?3U:2U);
    EXPECT_EQ(out.metadata().height,f<2?2U:3U);
    for(int n=0;n<6;++n) for(int c=0;c<3;++c)
      EXPECT_EQ(out.uint16_data()[3*n+c],expected[f][n]+10*c);
  }
  EXPECT_EQ(src.uint16_row(0)[9],999);
  EXPECT_EQ(src.uint16_at(1,2,0),6);
  EXPECT_THROW(orient_editor_output(src,1),std::invalid_argument);
  EXPECT_THROW(orient_editor_output(camera(),0),std::invalid_argument);
}
TEST(EditorOrientationTest, OwnedZeroOrientationReusesOnlyPackedStorage) {
  auto packed=render_editor_output(camera(),identity,0);
  const auto* data=packed.uint16_data();
  auto out=orient_editor_output(std::move(packed),0);
  EXPECT_EQ(out.uint16_data(),data);
  auto m=out.metadata(); m.row_stride_elements+=1;
  auto padded=ImageBuffer::create(m);
  for(std::size_t x=0;x<m.width*3;++x) padded.uint16_row(0)[x]=out.uint16_row(0)[x];
  const auto* paddedData=padded.uint16_data();
  auto repacked=orient_editor_output(std::move(padded),0);
  EXPECT_NE(repacked.uint16_data(),paddedData);
  EXPECT_EQ(repacked.metadata().row_stride_elements,m.width*3);
  for(std::size_t x=0;x<m.width*3;++x) EXPECT_EQ(repacked.uint16_row(0)[x],out.uint16_row(0)[x]);
  EXPECT_THROW(orient_editor_output(render_editor_output(camera(),identity,0),2),std::invalid_argument);
  EXPECT_THROW(orient_editor_output(camera(),0),std::invalid_argument);
}
TEST(EditorOrientationTest, FreshRendersDoNotAccumulateRotation) {
  const EditorPhoto photo{camera(),identity,6};
  const auto a=render_editor_photo(photo,0);
  const auto b=render_editor_photo(photo,0);
  EXPECT_EQ(a.metadata().width,1U); EXPECT_EQ(a.metadata().height,2U);
  for(std::size_t i=0;i<a.element_count();++i) EXPECT_EQ(a.uint16_data()[i],b.uint16_data()[i]);
  EXPECT_EQ(photo.camera.metadata().width,2U);
}

}
}
