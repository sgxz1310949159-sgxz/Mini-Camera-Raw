#include "mini_camera_raw/display_encode.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw {
namespace {
ImageMetadata working(std::size_t w = 1, std::size_t h = 1,
                      std::size_t stride = 3) {
  return {w,
          h,
          3,
          stride,
          PixelFormat::kFloat32,
          CfaPattern::kNone,
          ColorState::kLinearWorkingRgb,
          {0, 1, true},
          RgbColorSpace::kSrgb,
          TransferFunction::kLinear};
}
TEST(DisplayEncodeTest, ClipsOnlyOutputAndUsesSrgbInsteadOfPowerGamma) {
  auto in = ImageBuffer::from_float(
      working(3, 1, 9), {-1, 0, 2, 0.0031308F, 0.18F, 0.5F, 1, 0.001F, 0.25F});
  auto out = encode_srgb16(in);
  const unsigned expected[] = {0,     0,     65535, 2651, 30235,
                               48192, 65535, 847,   35199};
  for (int i = 0; i < 9; ++i) EXPECT_EQ(out.uint16_data()[i], expected[i]);
  EXPECT_FLOAT_EQ(in.float_data()[0], -1);
  EXPECT_FLOAT_EQ(in.float_data()[2], 2);
  EXPECT_EQ(out.metadata().color_state, ColorState::kEncodedRgb);
  EXPECT_EQ(out.metadata().transfer_function, TransferFunction::kSrgb);
  EXPECT_THROW(static_cast<void>(encode_srgb16(out)), std::invalid_argument);
}
TEST(DisplayEncodeTest, InverseGeneratedRampMonotonicityAndPadding) {
  auto in = ImageBuffer::create(working(257, 3, 257 * 3 + 1));
  for (std::size_t y = 0; y < 3; ++y) {
    for (std::size_t x = 0; x < 257 * 3; ++x) {
      double e = double(x) / 770;
      in.float_row(y)[x] =
          float(e <= 0.04045 ? e / 12.92 : std::pow((e + 0.055) / 1.055, 2.4));
    }
    in.float_row(y)[771] = std::numeric_limits<float>::quiet_NaN();
  }
  auto out = encode_srgb16(in);
  for (std::size_t y = 0; y < 3; ++y)
    for (std::size_t x = 0; x < 771; ++x) {
      EXPECT_NEAR(out.uint16_row(y)[x],
                  std::floor(double(x) / 770 * 65535 + 0.5), 1);
      if (x) EXPECT_LE(out.uint16_row(y)[x - 1], out.uint16_row(y)[x]);
    }
  EXPECT_EQ(out.metadata().row_stride_elements, 771U);
  EXPECT_TRUE(std::isnan(in.float_row(0)[771]));
}
TEST(DisplayEncodeTest, ThresholdNeighborsAndQuantizationHalfSteps) {
  float t = 0.0031308F;
  auto in = ImageBuffer::from_float(
      working(), {std::nextafter(t, 0.F), t, std::nextafter(t, 1.F)});
  auto out = encode_srgb16(in);
  for (int c = 0; c < 3; ++c) EXPECT_EQ(out.uint16_data()[c], 2651);
  // Linear branch: safely straddle a half-code boundary in encoded space.
  for (int q : {0, 1, 100, 1000}) {
    auto a = ImageBuffer::from_float(working(),
                                     {float((q + 0.49) / (65535 * 12.92)),
                                      float((q + 0.51) / (65535 * 12.92)), 0});
    auto b = encode_srgb16(a);
    EXPECT_EQ(b.uint16_data()[0], q);
    EXPECT_EQ(b.uint16_data()[1], q + 1);
  }
}
TEST(DisplayEncodeTest, RejectsUnknownSpaceAndNonfiniteActiveSamples) {
  auto m = working();
  m.rgb_color_space = RgbColorSpace::kUnspecified;
  m.transfer_function = TransferFunction::kUnspecified;
  auto unknown = ImageBuffer::create(m);
  EXPECT_THROW(static_cast<void>(encode_srgb16(unknown)),
               std::invalid_argument);
  for (float v : {std::numeric_limits<float>::quiet_NaN(),
                  std::numeric_limits<float>::infinity(),
                  -std::numeric_limits<float>::infinity()})
    for (int c = 0; c < 3; ++c) {
      auto in = ImageBuffer::create(working());
      in.float_data()[c] = v;
      EXPECT_THROW(static_cast<void>(encode_srgb16(in)), std::invalid_argument);
    }
}
}  // namespace
}  // namespace mini_camera_raw
