#include "mini_camera_raw/white_balance.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace mini_camera_raw {
namespace {
ImageMetadata linear_metadata(std::size_t w, std::size_t h, std::size_t stride,
                              CfaPattern cfa = CfaPattern::kRggb) {
  return {w,
          h,
          1,
          stride,
          PixelFormat::kFloat32,
          cfa,
          ColorState::kLinearBayer,
          {0, 1, true}};
}
TEST(WhiteBalanceTest, HandVectorAndOwnership) {
  auto input = ImageBuffer::from_float(linear_metadata(2, 2, 2),
                                       {-.25F, .5F, .75F, 1.25F});
  auto out = apply_white_balance(input, {{2, 1, 1, 3}});
  const float expected[] = {-.5F, .5F, .75F, 3.75F};
  for (int i = 0; i < 4; ++i) EXPECT_EQ(expected[i], out.float_data()[i]);
  EXPECT_EQ(ColorState::kLinearBayer, out.metadata().color_state);
  EXPECT_EQ(CfaPattern::kRggb, out.metadata().cfa_pattern);
  EXPECT_TRUE(out.metadata().numeric_range.allows_out_of_range);
  out.float_data()[0] = 99;
  EXPECT_EQ(-.25F, input.float_data()[0]);
}
TEST(WhiteBalanceTest, CameraNormalizationAllPhasesAndUnequalGreens) {
  for (auto cfa : {CfaPattern::kRggb, CfaPattern::kBggr, CfaPattern::kGrbg,
                   CfaPattern::kGbrg}) {
    const bool off = cfa == CfaPattern::kRggb || cfa == CfaPattern::kBggr;
    const std::array<double, 4> tile = off ? std::array<double, 4>{8, 2, 6, 12}
                                           : std::array<double, 4>{2, 8, 12, 6};
    const auto g = normalize_camera_wb(tile, cfa);
    for (int p = 0; p < 4; ++p) EXPECT_DOUBLE_EQ(tile[p] / 4, g.tile[p]);
  }
  const auto g = normalize_camera_wb({4, 2, 2, 6}, CfaPattern::kRggb);
  EXPECT_EQ((std::array<double, 4>{2, 1, 1, 3}), g.tile);
}
TEST(WhiteBalanceTest, GeneratedIdentityLiteralGainsPaddingAndMonotonicity) {
  for (auto cfa : {CfaPattern::kRggb, CfaPattern::kBggr, CfaPattern::kGrbg,
                   CfaPattern::kGbrg})
    for (std::size_t w : {1U, 2U, 5U})
      for (std::size_t h : {1U, 2U, 3U}) {
        auto a = ImageBuffer::create(linear_metadata(w, h, w, cfa));
        auto b = ImageBuffer::create(linear_metadata(w, h, w + 3, cfa));
        for (std::size_t y = 0; y < h; ++y) {
          for (std::size_t x = 0; x < w + 3; ++x) b.float_row(y)[x] = NAN;
          for (std::size_t x = 0; x < w; ++x)
            a.float_row(y)[x] = b.float_row(y)[x] = (int(y * w + x) - 4) * .25F;
        }
        const WhiteBalanceGains gains{{2, .5, 1.5, 3}};
        auto identity = apply_white_balance(a, {{1, 1, 1, 1}});
        auto packed = apply_white_balance(a, gains);
        auto padded = apply_white_balance(b, gains);
        EXPECT_EQ(w, padded.metadata().row_stride_elements);
        EXPECT_EQ(h, padded.metadata().height);
        EXPECT_EQ(cfa, padded.metadata().cfa_pattern);
        for (std::size_t y = 0; y < h; ++y)
          for (std::size_t x = 0; x < w; ++x) {
            EXPECT_EQ(a.float_row(y)[x], identity.float_row(y)[x]);
            EXPECT_EQ(packed.float_row(y)[x], padded.float_row(y)[x]);
            EXPECT_DOUBLE_EQ(
                double(a.float_row(y)[x]) * gains.tile[2 * (y % 2) + x % 2],
                packed.float_row(y)[x]);
            b.float_row(y)[x] += 1;
          }
        auto larger = apply_white_balance(b, gains);
        for (std::size_t y = 0; y < h; ++y)
          for (std::size_t x = 0; x < w; ++x)
            EXPECT_GT(larger.float_row(y)[x], padded.float_row(y)[x]);
      }
}
TEST(WhiteBalanceTest, NormalizationPreservesCommonScaleAndTinyEqualGreens) {
  const double tiny = std::numeric_limits<double>::denorm_min();
  EXPECT_EQ(
      (std::array<double, 4>{1, 1, 1, 1}),
      normalize_camera_wb({tiny, tiny, tiny, tiny}, CfaPattern::kRggb).tile);
  for (auto cfa : {CfaPattern::kRggb, CfaPattern::kBggr, CfaPattern::kGrbg,
                   CfaPattern::kGbrg}) {
    const std::array<double, 4> tile{2, 3, 5, 7};
    const auto reference = normalize_camera_wb(tile, cfa);
    for (double scale : {1e-200, .125, 16., 1e200}) {
      auto scaled = tile;
      for (double& value : scaled) value *= scale;
      const auto result = normalize_camera_wb(scaled, cfa);
      for (int p = 0; p < 4; ++p)
        EXPECT_NEAR(reference.tile[p], result.tile[p],
                    1e-6 + 1e-6 * std::abs(reference.tile[p]));
    }
  }
}

TEST(WhiteBalanceTest, RejectsEveryInvalidGainAndCfa) {
  auto in = ImageBuffer::create(linear_metadata(1, 1, 1));
  for (int p = 0; p < 4; ++p)
    for (double bad :
         {0., -1., double(INFINITY), double(-INFINITY), double(NAN)}) {
      WhiteBalanceGains g{{1, 1, 1, 1}};
      g.tile[p] = bad;
      EXPECT_THROW(static_cast<void>(apply_white_balance(in, g)),
                   std::invalid_argument);
      EXPECT_THROW(
          static_cast<void>(normalize_camera_wb(g.tile, CfaPattern::kRggb)),
          std::invalid_argument);
    }
  for (auto cfa : {CfaPattern::kNone, static_cast<CfaPattern>(999)})
    EXPECT_THROW(static_cast<void>(normalize_camera_wb({1, 1, 1, 1}, cfa)),
                 std::invalid_argument);
}
TEST(WhiteBalanceTest, RejectsNonfiniteActiveSamplesWithoutMutation) {
  auto in = ImageBuffer::create(linear_metadata(3, 3, 3));
  for (int p = 0; p < 9; ++p)
    for (float bad : {INFINITY, -INFINITY, NAN}) {
      in.float_data()[p] = bad;
      EXPECT_THROW(static_cast<void>(apply_white_balance(in, {{1, 1, 1, 1}})),
                   std::invalid_argument);
      EXPECT_TRUE(std::isnan(bad) ? std::isnan(in.float_data()[p])
                                  : in.float_data()[p] == bad);
      in.float_data()[p] = 0;
    }
}
TEST(WhiteBalanceTest, RejectsRawAndRgbInputs) {
  auto raw = ImageBuffer::create({2,
                                  2,
                                  1,
                                  2,
                                  PixelFormat::kUInt16,
                                  CfaPattern::kRggb,
                                  ColorState::kRawBayer,
                                  {0, 65535, false}});
  EXPECT_THROW(static_cast<void>(apply_white_balance(raw, {{1, 1, 1, 1}})),
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
    EXPECT_THROW(static_cast<void>(apply_white_balance(rgb, {{1, 1, 1, 1}})),
                 std::invalid_argument);
  }
}
TEST(WhiteBalanceTest, ExtremesOverflowAndSampleUnderflow) {
  const float max = std::numeric_limits<float>::max();
  auto in = ImageBuffer::from_float(linear_metadata(1, 1, 1), {max});
  EXPECT_EQ(max, apply_white_balance(in, {{1, 1, 1, 1}}).float_data()[0]);
  EXPECT_THROW(static_cast<void>(apply_white_balance(in, {{2, 1, 1, 1}})),
               std::overflow_error);
  EXPECT_EQ(max, in.float_data()[0]);
  in.float_data()[0] = -max;
  EXPECT_THROW(static_cast<void>(apply_white_balance(in, {{2, 1, 1, 1}})),
               std::overflow_error);
  in.float_data()[0] = std::numeric_limits<float>::denorm_min();
  EXPECT_EQ(0, apply_white_balance(
                   in, {{std::numeric_limits<double>::min(), 1, 1, 1}})
                   .float_data()[0]);
  const double big = std::numeric_limits<double>::max();
  EXPECT_EQ((std::array<double, 4>{1, 1, 1, 1}),
            normalize_camera_wb({big, big, big, big}, CfaPattern::kRggb).tile);
  EXPECT_THROW(static_cast<void>(normalize_camera_wb({big, 1e-300, 1e-300, big},
                                                     CfaPattern::kRggb)),
               std::overflow_error);
  EXPECT_THROW(static_cast<void>(normalize_camera_wb(
                   {std::numeric_limits<double>::denorm_min(), big, big, 1},
                   CfaPattern::kRggb)),
               std::overflow_error);
}
}  // namespace
}  // namespace mini_camera_raw
