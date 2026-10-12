#include "gui_image.h"
#include <QColorSpace>
#include <QRgba64>
#include <gtest/gtest.h>
#include <stdexcept>
namespace mini_camera_raw {
namespace {
ImageMetadata encoded_metadata() {
  return {3, 2, 3, 10, PixelFormat::kUInt16, CfaPattern::kNone,
          ColorState::kEncodedRgb, {0, 65535, false},
          RgbColorSpace::kSrgb, TransferFunction::kSrgb};
}
TEST(GuiImageTest, PreservesChannelsStrideGrayAndOpaqueAlpha) {
  auto source = ImageBuffer::from_uint16(encoded_metadata(),
      {65535,0,0, 0,65535,0, 0,0,65535, 123,
       65535,65535,65535, 32768,32768,32768, 0,0,0, 456});
  auto out = make_gui_image(source);
  EXPECT_EQ(out.format(), QImage::Format_RGBX64);
  EXPECT_EQ(out.colorSpace(), QColorSpace(QColorSpace::SRgb));
  EXPECT_EQ(out.size(), QSize(3,2));
  for (int y=0; y<2; ++y) {
    const auto* row = reinterpret_cast<const QRgba64*>(out.constScanLine(y));
    for (int x=0; x<3; ++x) {
      EXPECT_EQ(row[x].red(), source.uint16_at(y,x,0));
      EXPECT_EQ(row[x].green(), source.uint16_at(y,x,1));
      EXPECT_EQ(row[x].blue(), source.uint16_at(y,x,2));
      EXPECT_EQ(row[x].alpha(), 65535);
    }
  }
  EXPECT_EQ(source.uint16_row(1)[9],456);
  source.uint16_at(0,0,0)=0;
  EXPECT_EQ(out.pixelColor(0,0).rgba64().red(),65535);
}
TEST(GuiImageTest, OwnsStorageAfterSourceDestruction) {
  const auto out = [] {
    auto source = ImageBuffer::create(encoded_metadata());
    source.uint16_at(1,2,2)=12345;
    return make_gui_image(source);
  }();
  EXPECT_EQ(out.pixelColor(2,1).rgba64().blue(),12345);
}
TEST(GuiImageTest, RejectsWrongEncodingSpaceAndRange) {
  auto m=encoded_metadata();
  m.transfer_function=TransferFunction::kLinear;
  EXPECT_THROW(make_gui_image(ImageBuffer::create(m)),std::invalid_argument);
  m=encoded_metadata(); m.rgb_color_space=RgbColorSpace::kUnspecified;
  EXPECT_THROW(make_gui_image(ImageBuffer::create(m)),std::invalid_argument);
  m=encoded_metadata(); m.numeric_range.nominal_max=1023;
  EXPECT_THROW(make_gui_image(ImageBuffer::create(m)),std::invalid_argument);
}
}
}
