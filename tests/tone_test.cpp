#include "mini_camera_raw/tone.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw {
namespace {
ImageMetadata working(std::size_t w = 1, std::size_t h = 1,
                      std::size_t padding = 0) {
  return {w,
          h,
          3,
          w * 3 + padding,
          PixelFormat::kFloat32,
          CfaPattern::kNone,
          ColorState::kLinearWorkingRgb,
          {0, 1, true},
          RgbColorSpace::kSrgb,
          TransferFunction::kLinear};
}
TEST(ExposureTest, SignedValuesHeadroomAndIdentity) {
  auto in = ImageBuffer::from_float(working(), {-.1f, .18f, 2.f});
  auto out = apply_exposure(in, 1);
  EXPECT_FLOAT_EQ(out.float_data()[0], -.2f);
  EXPECT_FLOAT_EQ(out.float_data()[1], .36f);
  EXPECT_FLOAT_EQ(out.float_data()[2], 4.f);
  auto same = apply_exposure(in, 0);
  EXPECT_EQ(std::memcmp(in.float_data(), same.float_data(), in.byte_size()), 0);
  EXPECT_NE(in.float_data(), same.float_data());
  EXPECT_FLOAT_EQ(in.float_data()[2], 2.f);
  EXPECT_EQ(out.metadata().color_state, ColorState::kLinearWorkingRgb);
}
TEST(ExposureTest, GeneratedReferenceCompositionAndPaddedLayouts) {
  for (std::size_t w : {1, 3, 7}) {
    auto in = ImageBuffer::create(working(w, 3, 2));
    for (std::size_t y = 0; y < 3; ++y) {
      for (std::size_t i = 0; i < w * 3; ++i)
        in.float_row(y)[i] = float(int(i + 11 * y) - 17) / 8;
      in.float_row(y)[w * 3] = std::numeric_limits<float>::quiet_NaN();
    }
    for (double ev : {-20., -1.5, 0., .75, 20.}) {
      auto out = apply_exposure(in, ev);
      auto back = apply_exposure(out, -ev);
      EXPECT_EQ(out.metadata().row_stride_elements, w * 3);
      for (std::size_t y = 0; y < 3; ++y)
        for (std::size_t i = 0; i < w * 3; ++i) {
          long double ref = static_cast<long double>(in.float_row(y)[i]) *
                            std::exp2(static_cast<long double>(ev));
          EXPECT_NEAR(out.float_row(y)[i], ref, 1e-6L + 1e-6L * std::abs(ref));
          EXPECT_NEAR(back.float_row(y)[i], in.float_row(y)[i], 1e-6);
        }
      EXPECT_TRUE(std::isnan(in.float_row(0)[w * 3]));
    }
    auto combined = apply_exposure(apply_exposure(in, .75), -1.5);
    auto direct = apply_exposure(in, -.75);
    for (std::size_t i = 0; i < direct.element_count(); ++i)
      EXPECT_NEAR(combined.float_data()[i], direct.float_data()[i], 1e-6);
  }
}
TEST(ExposureTest, RejectsParametersSamplesStateAndOverflow) {
  auto in = ImageBuffer::from_float(working(), {1, 2, 3});
  for (double ev : {-20.01, 20.01, std::numeric_limits<double>::infinity(),
                    std::numeric_limits<double>::quiet_NaN()})
    EXPECT_THROW((void)apply_exposure(in, ev), std::invalid_argument);
  for (float bad : {std::numeric_limits<float>::infinity(),
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::quiet_NaN()})
    for (int c = 0; c < 3; ++c) {
      auto x = in;
      x.float_data()[c] = bad;
      EXPECT_THROW((void)apply_exposure(x, 0), std::invalid_argument);
    }
  in.float_data()[0] = std::numeric_limits<float>::max();
  EXPECT_THROW((void)apply_exposure(in, 1), std::overflow_error);
  auto m = working();
  m.rgb_color_space = RgbColorSpace::kUnspecified;
  m.transfer_function = TransferFunction::kUnspecified;
  EXPECT_THROW((void)apply_exposure(ImageBuffer::create(m), 0),
               std::invalid_argument);
}
}  // namespace
}  // namespace mini_camera_raw

namespace mini_camera_raw {
namespace {
TEST(ToneTest, NeutralReferenceAndStateOrder) {
  for (float v : {0.f, .18f, 1.f, 4.f}) {
    auto in = ImageBuffer::from_float(working(), {v, v, v});
    auto out = apply_tone(in, 1);
    for (int c = 0; c < 3; ++c)
      EXPECT_NEAR(out.float_data()[c], double(v) / (1 + double(v)), 1e-6);
    EXPECT_EQ(out.metadata().color_state, ColorState::kToneMappedWorkingRgb);
    EXPECT_EQ(out.metadata().transfer_function, TransferFunction::kLinear);
    EXPECT_THROW((void)apply_exposure(out, 0), std::invalid_argument);
    EXPECT_THROW((void)apply_tone(out, 0), std::invalid_argument);
    auto same = apply_tone(in, 0);
    EXPECT_EQ(std::memcmp(in.float_data(), same.float_data(), in.byte_size()),
              0);
    EXPECT_NE(in.float_data(), same.float_data());
  }
}
TEST(ToneTest, NonpositiveLuminanceAndNegativeComponents) {
  auto in = ImageBuffer::from_float(working(), {-2.f, 0.f, 0.f});
  auto out = apply_tone(in, 1);
  EXPECT_EQ(std::memcmp(in.float_data(), out.float_data(), in.byte_size()), 0);
  in = ImageBuffer::from_float(working(), {-.1f, 1.f, 2.f});
  out = apply_tone(in, 1);
  EXPECT_LT(out.float_data()[0], 0);
  const double scale = out.float_data()[1];
  EXPECT_NEAR(out.float_data()[0], -.1 * scale, 1e-7);
  EXPECT_NEAR(out.float_data()[2], 2 * scale, 1e-7);
  EXPECT_FLOAT_EQ(in.float_data()[2], 2);
}
TEST(ToneTest, GeneratedMonotonicLuminanceAndPadding) {
  auto in = ImageBuffer::create(working(257, 3, 1));
  for (std::size_t y = 0; y < 3; ++y) {
    for (std::size_t x = 0; x < 257; ++x)
      for (int c = 0; c < 3; ++c)
        in.float_at(y, x, c) = float(int(x) - 32) / 16;
    in.float_row(y)[257 * 3] = std::numeric_limits<float>::quiet_NaN();
  }
  for (double strength : {0., .25, 1.}) {
    auto out = apply_tone(in, strength);
    EXPECT_EQ(out.metadata().row_stride_elements, 257 * 3);
    for (std::size_t y = 0; y < 3; ++y)
      for (std::size_t x = 1; x < 257; ++x)
        EXPECT_GE(out.float_at(y, x, 0), out.float_at(y, x - 1, 0));
  }
}
TEST(ToneTest, RejectsInvalidStrengthStateSamplesAndHandlesFiniteExtremes) {
  auto in = ImageBuffer::from_float(working(), {1, 2, 3});
  for (double s : {-.01, 1.01, std::numeric_limits<double>::quiet_NaN(),
                   std::numeric_limits<double>::infinity()})
    EXPECT_THROW((void)apply_tone(in, s), std::invalid_argument);
  for (float v : {std::numeric_limits<float>::quiet_NaN(),
                  std::numeric_limits<float>::infinity(),
                  -std::numeric_limits<float>::infinity()})
    for (int c = 0; c < 3; ++c) {
      auto bad = in;
      bad.float_data()[c] = v;
      EXPECT_THROW((void)apply_tone(bad, 0), std::invalid_argument);
    }
  for (int c = 0; c < 3; ++c)
    in.float_data()[c] = std::numeric_limits<float>::max();
  auto out = apply_tone(in, 1);
  for (int c = 0; c < 3; ++c) EXPECT_NEAR(out.float_data()[c], 1, 1e-6);
  auto m = working();
  m.rgb_color_space = RgbColorSpace::kUnspecified;
  m.transfer_function = TransferFunction::kUnspecified;
  EXPECT_THROW((void)apply_tone(ImageBuffer::create(m), 1),
               std::invalid_argument);
}
}  // namespace
}  // namespace mini_camera_raw

namespace mini_camera_raw {
TEST(ExposureTest, RepresentationalUnderflowAndSignedZeroAreExplicit) {
  auto input = ImageBuffer::from_float(
      working(), {-0.f, std::numeric_limits<float>::denorm_min(),
                  -std::numeric_limits<float>::denorm_min()});
  auto same = apply_exposure(input, 0);
  EXPECT_TRUE(std::signbit(same.float_data()[0]));
  auto small = apply_exposure(input, -20);
  EXPECT_EQ(small.float_data()[1], 0.f);
  EXPECT_EQ(small.float_data()[2], 0.f);
  EXPECT_TRUE(std::signbit(small.float_data()[2]));
}
}  // namespace mini_camera_raw
