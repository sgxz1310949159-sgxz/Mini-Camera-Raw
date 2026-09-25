#include "mini_camera_raw/histogram.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
namespace mini_camera_raw {
namespace {
ImageMetadata hist_metadata(std::size_t w, std::size_t h = 1,
                            std::size_t p = 0) {
  return {w,
          h,
          3,
          w * 3 + p,
          PixelFormat::kFloat32,
          CfaPattern::kNone,
          ColorState::kLinearWorkingRgb,
          {0, 1, true},
          RgbColorSpace::kSrgb,
          TransferFunction::kLinear};
}
TEST(HistogramTest, ExactEdgesNeighborsOutliersAndExtrema) {
  const float edge = 1.f / 256;
  const float values[] = {-.1f,
                          0,
                          std::nextafter(edge, 0.f),
                          edge,
                          std::nextafter(edge, 1.f),
                          std::nextafter(1.f, 0.f),
                          1.f,
                          std::nextafter(1.f, 2.f)};
  auto input = ImageBuffer::create(hist_metadata(8));
  for (int i = 0; i < 8; ++i)
    for (int c = 0; c < 3; ++c) input.float_at(0, i, c) = values[i];
  auto hist = compute_histogram(input);
  for (int c = 0; c < 3; ++c) {
    const auto& channel = hist.channels[c];
    EXPECT_EQ(channel.below_zero, 1);
    EXPECT_EQ(channel.above_one, 1);
    EXPECT_EQ(channel.bins[0], 2);
    EXPECT_EQ(channel.bins[1], 2);
    EXPECT_EQ(channel.bins[255], 2);
    EXPECT_DOUBLE_EQ(channel.minimum, values[0]);
    EXPECT_DOUBLE_EQ(channel.maximum, values[7]);
  }
}
TEST(HistogramTest, PrimariesHaveLinearLuminanceNotEncodedLuma) {
  auto input =
      ImageBuffer::from_float(hist_metadata(3), {1, 0, 0, 0, 1, 0, 0, 0, 1});
  auto hist = compute_histogram(input);
  EXPECT_EQ(hist.channels[3].bins[54], 1);
  EXPECT_EQ(hist.channels[3].bins[183], 1);
  EXPECT_EQ(hist.channels[3].bins[18], 1);
  EXPECT_NEAR(hist.channels[3].minimum, .07219231536073371, 1e-15);
  EXPECT_NEAR(hist.channels[3].maximum, .7151686787677559, 1e-15);
}
TEST(HistogramTest, GeneratedConservationPaddingAndSourcePreservation) {
  for (std::size_t w : {1, 3, 7, 257}) {
    auto input = ImageBuffer::create(hist_metadata(w, 3, 2));
    for (std::size_t y = 0; y < 3; ++y) {
      for (std::size_t i = 0; i < w * 3; ++i)
        input.float_row(y)[i] = float(int(i + 7 * y) - 20) / 32;
      input.float_row(y)[w * 3] = std::numeric_limits<float>::quiet_NaN();
    }
    auto before = input;
    auto hist = compute_histogram(input);
    for (const auto& c : hist.channels) {
      auto total =
          std::accumulate(c.bins.begin(), c.bins.end(), std::uint64_t{0}) +
          c.below_zero + c.above_one;
      EXPECT_EQ(total, w * 3);
    }
    for (std::size_t y = 0; y < 3; ++y)
      for (std::size_t i = 0; i < w * 3; ++i)
        EXPECT_EQ(input.float_row(y)[i], before.float_row(y)[i]);
  }
}
TEST(HistogramTest, RejectsStateAndEveryNonfiniteChannel) {
  auto input = ImageBuffer::from_float(hist_metadata(1), {0, 1, 2});
  for (float value : {std::numeric_limits<float>::quiet_NaN(),
                      std::numeric_limits<float>::infinity(),
                      -std::numeric_limits<float>::infinity()})
    for (int c = 0; c < 3; ++c) {
      auto bad = input;
      bad.float_data()[c] = value;
      EXPECT_THROW((void)compute_histogram(bad), std::invalid_argument);
    }
  auto m = hist_metadata(1);
  m.color_state = ColorState::kLinearCameraRgb;
  m.rgb_color_space = RgbColorSpace::kUnspecified;
  m.transfer_function = TransferFunction::kUnspecified;
  EXPECT_THROW((void)compute_histogram(ImageBuffer::create(m)),
               std::invalid_argument);
}
}  // namespace
}  // namespace mini_camera_raw

namespace mini_camera_raw {
TEST(HistogramTest, ToneMappedHistogramHasSameBinsForSameSamples) {
  auto m = hist_metadata(1);
  auto original = ImageBuffer::from_float(m, {-.1f, .25f, 2});
  m.color_state = ColorState::kToneMappedWorkingRgb;
  auto toned = ImageBuffer::from_float(m, {-.1f, .25f, 2});
  auto a = compute_histogram(original);
  auto b = compute_histogram(toned);
  for (int c = 0; c < 4; ++c) {
    EXPECT_EQ(a.channels[c].bins, b.channels[c].bins);
    EXPECT_EQ(a.channels[c].below_zero, b.channels[c].below_zero);
    EXPECT_EQ(a.channels[c].above_one, b.channels[c].above_one);
  }
}
}  // namespace mini_camera_raw

namespace mini_camera_raw {
TEST(HistogramTest, FiniteExtremesAreOutliersWithoutIndexOverflow) {
  const float limit = std::numeric_limits<float>::max();
  auto input =
      ImageBuffer::from_float(hist_metadata(1), {-limit, limit, limit});
  auto hist = compute_histogram(input);
  EXPECT_EQ(hist.channels[0].below_zero, 1);
  EXPECT_EQ(hist.channels[1].above_one, 1);
  EXPECT_EQ(hist.channels[2].above_one, 1);
  EXPECT_EQ(hist.channels[3].above_one, 1);
  EXPECT_TRUE(std::isfinite(hist.channels[3].maximum));
}
}  // namespace mini_camera_raw
