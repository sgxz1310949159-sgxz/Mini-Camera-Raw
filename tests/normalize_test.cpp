#include "mini_camera_raw/normalize.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace mini_camera_raw {
namespace {
ImageMetadata raw_metadata(std::size_t width, std::size_t height,
                           std::size_t stride, CfaPattern cfa = CfaPattern::kRggb) {
  return {width, height, 1, stride, PixelFormat::kUInt16, cfa,
          ColorState::kRawBayer, {0, 65535, false}};
}

TEST(NormalizeTest, HandVectorPreservesNegativeAndAboveWhite) {
  auto raw = ImageBuffer::from_uint16(raw_metadata(2, 2, 2), {0, 200, 700, 1400});
  const SensorLevels levels{{100, 200, 300, 400}, {1100, 1200, 1100, 1200}};
  auto output = normalize(raw, levels);
  EXPECT_NEAR(-0.1, output.float_at(0, 0, 0), 1e-6);
  EXPECT_FLOAT_EQ(0, output.float_at(0, 1, 0));
  EXPECT_FLOAT_EQ(0.5, output.float_at(1, 0, 0));
  EXPECT_FLOAT_EQ(1.25, output.float_at(1, 1, 0));
  EXPECT_EQ(ColorState::kLinearBayer, output.metadata().color_state);
  EXPECT_EQ(PixelFormat::kFloat32, output.metadata().pixel_format);
  EXPECT_TRUE(output.metadata().numeric_range.allows_out_of_range);
  EXPECT_EQ(1400, raw.uint16_at(1, 1, 0));
}

TEST(NormalizeTest, AllCfaOddDimensionsAndPaddingMatchAnalyticReference) {
  const SensorLevels levels{{10, 20, 30, 40}, {100, 200, 300, 400}};
  for (auto cfa : {CfaPattern::kRggb, CfaPattern::kBggr,
                   CfaPattern::kGrbg, CfaPattern::kGbrg}) {
    auto packed = ImageBuffer::create(raw_metadata(5, 3, 5, cfa));
    auto padded = ImageBuffer::create(raw_metadata(5, 3, 8, cfa));
    for (std::size_t y = 0; y < 3; ++y) {
      for (std::size_t x = 0; x < 8; ++x) padded.uint16_row(y)[x] = 65535;
      for (std::size_t x = 0; x < 5; ++x)
        packed.uint16_row(y)[x] = padded.uint16_row(y)[x] = 75 * (y + x);
    }
    auto a = normalize(packed, levels);
    auto b = normalize(padded, levels);
    EXPECT_EQ(cfa, b.metadata().cfa_pattern);
    EXPECT_EQ(5U, b.metadata().row_stride_elements);
    EXPECT_EQ(3U, b.metadata().height);
    for (std::size_t y = 0; y < 3; ++y)
      for (std::size_t x = 0; x < 5; ++x) {
        const auto p = 2 * (y % 2) + x % 2;
        const double expected = (75.0 * (y + x) - levels.black[p]) /
                                (levels.white[p] - levels.black[p]);
        EXPECT_NEAR(expected, b.float_at(y, x, 0), 1e-6 + 1e-6 * std::abs(expected));
        EXPECT_FLOAT_EQ(a.float_at(y, x, 0), b.float_at(y, x, 0));
      }
  }
}

TEST(NormalizeTest, SinglePixelEndpointsAndMonotonicity) {
  const SensorLevels levels{{0, 0, 0, 0}, {65535, 65535, 65535, 65535}};
  auto raw = ImageBuffer::create(raw_metadata(1, 1, 1));
  float previous = -1;
  for (unsigned value : {0U, 1U, 128U, 1024U, 32768U, 65535U}) {
    raw.uint16_row(0)[0] = value;
    const auto result = normalize(raw, levels);
    const float sample = result.float_row(0)[0];
    EXPECT_GT(sample, previous);
    EXPECT_NEAR(value / 65535.0, sample, 1e-6);
    previous = sample;
  }
  EXPECT_FLOAT_EQ(1, previous);
}

TEST(NormalizeTest, RejectsInvalidLevelsAtEveryPosition) {
  auto raw = ImageBuffer::create(raw_metadata(1, 1, 1));
  const SensorLevels valid{{0, 0, 0, 0}, {100, 100, 100, 100}};
  for (std::size_t p = 0; p < 4; ++p) {
    for (double bad : {-1.0, 100.0, 101.0, std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::quiet_NaN()}) {
      auto levels = valid; levels.black[p] = bad;
      EXPECT_THROW(static_cast<void>(normalize(raw, levels)), std::invalid_argument);
    }
    for (double bad : {-1.0, 0.0, 65536.0, std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::quiet_NaN()}) {
      auto levels = valid; levels.white[p] = bad;
      EXPECT_THROW(static_cast<void>(normalize(raw, levels)), std::invalid_argument);
    }
  }
}

TEST(NormalizeTest, RejectsAllFloatInputsIncludingNonfinite) {
  auto metadata = raw_metadata(1, 1, 1);
  metadata.pixel_format = PixelFormat::kFloat32;
  metadata.color_state = ColorState::kLinearBayer;
  metadata.numeric_range = {0, 1, true};
  const SensorLevels levels{{0, 0, 0, 0}, {1, 1, 1, 1}};
  for (float v : {0.0F, std::numeric_limits<float>::infinity(),
                   std::numeric_limits<float>::quiet_NaN()}) {
    auto raw = ImageBuffer::from_float(metadata, {v});
    EXPECT_THROW(static_cast<void>(normalize(raw, levels)), std::invalid_argument);
  }
}

TEST(NormalizeTest, RejectsSamplesBeyondDeclaredRangeButIgnoresPadding) {
  auto metadata = raw_metadata(1, 1, 2);
  metadata.numeric_range.nominal_max = 100;
  auto raw = ImageBuffer::from_uint16(metadata, {100, 65535});
  const SensorLevels levels{{0, 0, 0, 0}, {100, 100, 100, 100}};
  EXPECT_FLOAT_EQ(1, normalize(raw, levels).float_row(0)[0]);
  raw.uint16_row(0)[0] = 101;
  EXPECT_THROW(static_cast<void>(normalize(raw, levels)), std::invalid_argument);
}

TEST(NormalizeTest, RejectsUnrepresentableFloatResult) {
  auto raw = ImageBuffer::from_uint16(raw_metadata(1, 1, 1), {65535});
  SensorLevels levels{{0, 0, 0, 0}, {1e-300, 1, 1, 1}};
  EXPECT_THROW(static_cast<void>(normalize(raw, levels)), std::overflow_error);
}
}  // namespace
}  // namespace mini_camera_raw
