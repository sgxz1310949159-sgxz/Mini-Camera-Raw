#include "mini_camera_raw/demosaic.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "mini_camera_raw/white_balance.h"

namespace mini_camera_raw {
namespace {
constexpr std::array<CfaPattern, 4> phases{
    CfaPattern::kRggb, CfaPattern::kBggr, CfaPattern::kGrbg, CfaPattern::kGbrg};
// Test oracle uses explicit channel tiles and directional stencils.
constexpr int colors[4][4] = {
    {0, 1, 1, 2}, {2, 1, 1, 0}, {1, 0, 2, 1}, {1, 2, 0, 1}};
ImageMetadata metadata(int w, int h, int padding = 0, int phase = 0) {
  return {std::size_t(w),           std::size_t(h),        1,
          std::size_t(w + padding), PixelFormat::kFloat32, phases[phase],
          ColorState::kLinearBayer, {0, 1, true}};
}
double oracle(const ImageBuffer& input, int phase, int y, int x, int channel) {
  const int measured = colors[phase][2 * (y % 2) + x % 2];
  if (channel == measured) return input.float_at(y, x, 0);
  constexpr int axial[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  constexpr int diagonal[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  double sum = 0;
  int count = 0;
  const bool diagonal_needed = measured != 1 && channel != 1;
  for (int k = 0; k < 4; ++k) {
    const auto& offset = diagonal_needed ? diagonal[k] : axial[k];
    if (measured == 1) {
      const bool horizontal =
          colors[phase][2 * (y % 2) + (1 - x % 2)] == channel;
      if ((offset[0] == 0) != horizontal) continue;
    }
    const int ny = y + offset[0], nx = x + offset[1];
    if (ny < 0 || nx < 0 || ny >= int(input.metadata().height) ||
        nx >= int(input.metadata().width))
      continue;
    sum += input.float_at(ny, nx, 0);
    ++count;
  }
  EXPECT_GT(count, 0);
  return sum / count;
}
TEST(DemosaicTest, TwoByTwoHandVectorAndOwnership) {
  auto input = ImageBuffer::from_float(metadata(2, 2), {2, 4, 6, 8});
  auto result = demosaic_bilinear(input);
  const float expected[] = {2, 5, 8, 2, 4, 8, 2, 6, 8, 2, 5, 8};
  for (int i = 0; i < 12; ++i) EXPECT_EQ(expected[i], result.float_data()[i]);
  EXPECT_EQ(ColorState::kLinearCameraRgb, result.metadata().color_state);
  EXPECT_EQ(CfaPattern::kNone, result.metadata().cfa_pattern);
  EXPECT_EQ(PixelFormat::kFloat32, result.metadata().pixel_format);
  EXPECT_EQ(3U, result.metadata().channel_count);
  EXPECT_EQ(6U, result.metadata().row_stride_elements);
  EXPECT_TRUE(result.metadata().numeric_range.allows_out_of_range);
  result.float_data()[0] = 100;
  EXPECT_EQ(2, input.float_data()[0]);
}
TEST(DemosaicTest, GeneratedAllPhasesSmallOddBordersAndPaddingMatchOracle) {
  for (int phase = 0; phase < 4; ++phase)
    for (int w = 2; w <= 9; ++w)
      for (int h = 2; h <= 7; ++h) {
        SCOPED_TRACE(::testing::Message() << phase << ":" << w << "x" << h);
        auto in = ImageBuffer::create(metadata(w, h, 3, phase));
        auto packed = ImageBuffer::create(metadata(w, h, 0, phase));
        std::array<float, 3> low{INFINITY, INFINITY, INFINITY},
            high{-INFINITY, -INFINITY, -INFINITY};
        for (int y = 0; y < h; ++y) {
          for (int x = 0; x < w + 3; ++x) in.float_row(y)[x] = NAN;
          for (int x = 0; x < w; ++x) {
            float v = float((x * 13 + y * 7 + x * y * 3) % 41 - 20) / 7;
            in.float_row(y)[x] = packed.float_row(y)[x] = v;
            int c = colors[phase][2 * (y % 2) + x % 2];
            low[c] = std::min(low[c], v);
            high[c] = std::max(high[c], v);
          }
        }
        const auto out = demosaic_bilinear(in),
                   other = demosaic_bilinear(packed);
        EXPECT_EQ(std::size_t(w), out.metadata().width);
        EXPECT_EQ(std::size_t(h), out.metadata().height);
        for (int y = 0; y < h; ++y)
          for (int x = 0; x < w; ++x)
            for (int c = 0; c < 3; ++c) {
              double expected = oracle(in, phase, y, x, c);
              float actual = out.float_at(y, x, c);
              EXPECT_NEAR(expected, actual, 1e-6 + 1e-6 * std::abs(expected));
              EXPECT_EQ(actual, other.float_at(y, x, c));
              EXPECT_GE(actual, low[c]);
              EXPECT_LE(actual, high[c]);
              if (c == colors[phase][2 * (y % 2) + x % 2])
                EXPECT_EQ(in.float_at(y, x, 0), actual);
            }
        const auto again = demosaic_bilinear(in);
        for (std::size_t p = 0; p < out.element_count(); ++p)
          EXPECT_EQ(out.float_data()[p], again.float_data()[p]);
      }
}
TEST(DemosaicTest, ConstantChannelsAndInteriorAffineRamps) {
  for (int phase = 0; phase < 4; ++phase) {
    auto in = ImageBuffer::create(metadata(7, 5, 0, phase));
    const float base[3] = {-.5F, .25F, 2.5F};
    for (bool ramp : {false, true}) {
      for (int y = 0; y < 5; ++y)
        for (int x = 0; x < 7; ++x) {
          int c = colors[phase][2 * (y % 2) + x % 2];
          in.float_row(y)[x] = base[c] + (ramp ? .125F * x - .25F * y : 0);
        }
      const auto out = demosaic_bilinear(in);
      const int margin = ramp ? 1 : 0;
      for (int y = margin; y < 5 - margin; ++y)
        for (int x = margin; x < 7 - margin; ++x)
          for (int c = 0; c < 3; ++c)
            EXPECT_EQ(base[c] + (ramp ? .125F * x - .25F * y : 0),
                      out.float_at(y, x, c));
    }
  }
}
TEST(DemosaicTest, ChannelImpulsesAtEveryPosition) {
  for (int phase = 0; phase < 4; ++phase)
    for (int sy = 0; sy < 5; ++sy)
      for (int sx = 0; sx < 5; ++sx) {
        auto in = ImageBuffer::create(metadata(5, 5, 0, phase));
        in.float_row(sy)[sx] = 1;
        const auto out = demosaic_bilinear(in);
        const int source_channel = colors[phase][2 * (sy % 2) + sx % 2];
        for (int y = 0; y < 5; ++y)
          for (int x = 0; x < 5; ++x)
            for (int c = 0; c < 3; ++c) {
              EXPECT_NEAR(oracle(in, phase, y, x, c), out.float_at(y, x, c),
                          1e-6);
              if (c != source_channel) EXPECT_EQ(0, out.float_at(y, x, c));
            }
      }
}
TEST(DemosaicTest, RejectsSingleDimensionsAndWrongStates) {
  for (int phase = 0; phase < 4; ++phase)
    for (auto dims : {std::array<int, 2>{1, 1}, {1, 5}, {5, 1}}) {
      auto in = ImageBuffer::create(metadata(dims[0], dims[1], 0, phase));
      EXPECT_THROW(static_cast<void>(demosaic_bilinear(in)),
                   std::invalid_argument);
    }
  auto raw = ImageBuffer::create({2,
                                  2,
                                  1,
                                  2,
                                  PixelFormat::kUInt16,
                                  CfaPattern::kRggb,
                                  ColorState::kRawBayer,
                                  {0, 65535, false}});
  EXPECT_THROW(static_cast<void>(demosaic_bilinear(raw)),
               std::invalid_argument);
  for (auto state :
       {ColorState::kLinearCameraRgb, ColorState::kLinearWorkingRgb}) {
    auto rgb = ImageBuffer::create({2,
                                    2,
                                    3,
                                    6,
                                    PixelFormat::kFloat32,
                                    CfaPattern::kNone,
                                    state,
                                    {0, 1, true}});
    EXPECT_THROW(static_cast<void>(demosaic_bilinear(rgb)),
                 std::invalid_argument);
  }
}
TEST(DemosaicTest, RejectsNonfiniteAtEveryActivePosition) {
  auto in = ImageBuffer::create(metadata(3, 3));
  for (int p = 0; p < 9; ++p)
    for (float value : {NAN, INFINITY, -INFINITY}) {
      in.float_data()[p] = value;
      EXPECT_THROW(static_cast<void>(demosaic_bilinear(in)),
                   std::invalid_argument);
      EXPECT_TRUE(std::isnan(value) ? std::isnan(in.float_data()[p])
                                    : in.float_data()[p] == value);
      in.float_data()[p] = 0;
    }
}
TEST(DemosaicTest, FloatExtremesCannotOverflowAverages) {
  for (float v :
       {std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
        std::numeric_limits<float>::denorm_min()}) {
    auto in = ImageBuffer::from_float(metadata(3, 3),
                                      ImageBuffer::FloatStorage(9, v));
    const auto out = demosaic_bilinear(in);
    for (std::size_t p = 0; p < out.element_count(); ++p)
      EXPECT_EQ(v, out.float_data()[p]);
  }
}
TEST(DemosaicTest, WhiteBalanceCompositionKeepsSpatialGreensDistinct) {
  auto in = ImageBuffer::from_float(metadata(2, 2), {1, 1, 1, 1});
  const auto balanced = apply_white_balance(in, {{2, 1, 3, 4}});
  const auto out = demosaic_bilinear(balanced);
  const float expected[] = {2, 2, 4, 2, 1, 4, 2, 3, 4, 2, 2, 4};
  for (int i = 0; i < 12; ++i) EXPECT_EQ(expected[i], out.float_data()[i]);
}
}  // namespace
}  // namespace mini_camera_raw
