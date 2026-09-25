#include "render_pipeline.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "mini_camera_raw/display_encode.h"
namespace mini_camera_raw::app {
namespace {
ImageBuffer camera_input() {
  ImageMetadata m{3,
                  1,
                  3,
                  10,
                  PixelFormat::kFloat32,
                  CfaPattern::kNone,
                  ColorState::kLinearCameraRgb,
                  {0, 1, true}};
  return ImageBuffer::from_float(
      m, {2.4f, 1.f, 1.6f, -.1f, .18f, .4f, 0.f, 0.f, 0.f,
          std::numeric_limits<float>::quiet_NaN()});
}
const CameraToWorkingMatrix matrix{{1.6, -.4, -.2, -.1, 1.4, -.3, 0, -.3, 1.3},
                                   RgbColorSpace::kSrgb};
TEST(RenderTest, NeutralClipMatchesP5AndPreservesCamera) {
  auto camera = camera_input();
  auto original = camera;
  auto result =
      render_camera(camera, matrix, 0, 0, HighlightMode::kCameraClip, true);
  auto expected = encode_srgb16(
      transform_camera_rgb(clip_camera_highlights(camera), matrix));
  auto encoded = encode_srgb16(result.image);
  for (std::size_t i = 0; i < expected.element_count(); ++i)
    EXPECT_EQ(encoded.uint16_data()[i], expected.uint16_data()[i]);
  for (int i = 0; i < 9; ++i)
    EXPECT_EQ(camera.float_data()[i], original.float_data()[i]);
  ASSERT_TRUE(result.histograms.has_value());
  EXPECT_GT(result.histograms->before_exposure.channels[0].above_one, 0);
  EXPECT_EQ(result.image.metadata().color_state,
            ColorState::kToneMappedWorkingRgb);
}
TEST(RenderTest, NegativeExposureKeepsP5SaturatedCoreNeutral) {
  auto camera = camera_input();
  auto result =
      render_camera(camera, matrix, -2, 0, HighlightMode::kCameraClip, true);
  for (int c = 0; c < 3; ++c)
    EXPECT_NEAR(result.image.float_at(0, 0, c), .25, 1e-6);
  EXPECT_FLOAT_EQ(camera.float_at(0, 0, 0), 2.4f);
}
TEST(RenderTest, BothBranchesRespondToExposureAndToneAgainstIndependentOracle) {
  for (auto mode : {HighlightMode::kPreserve, HighlightMode::kCameraClip})
    for (double ev : {-2., 0., 2.})
      for (double s : {0., 1.}) {
        auto camera = camera_input();
        auto result = render_camera(camera, matrix, ev, s, mode, true);
        for (int x = 0; x < 3; ++x) {
          long double rgb[3] = {};
          for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c) {
              long double camera_value =
                  static_cast<long double>(camera.float_at(0, x, c));
              if (mode == HighlightMode::kCameraClip)
                camera_value = std::min(camera_value, 1.L);
              rgb[r] +=
                  matrix.coefficients[3 * r + c] * camera_value * std::exp2(ev);
            }
          const long double y = (87098.L / 409605) * rgb[0] +
                                (175762.L / 245763) * rgb[1] +
                                (12673.L / 175545) * rgb[2];
          for (int c = 0; c < 3; ++c) {
            long double expected = rgb[c] / (1 + s * std::max(y, 0.L));
            EXPECT_NEAR(result.image.float_at(0, x, c), expected,
                        1e-5L + 1e-4L * std::abs(expected));
          }
        }
        auto actual = compute_histogram(result.image);
        for (int c = 0; c < 4; ++c)
          EXPECT_EQ(actual.channels[c].bins,
                    result.histograms->rendered.channels[c].bins);
        EXPECT_FLOAT_EQ(camera.float_data()[0], 2.4f);
      }
}
TEST(RenderTest, DiagnosticsDoNotChangeRenderAndInvalidControlsFail) {
  auto camera = camera_input();
  auto a =
      render_camera(camera, matrix, 1, .25, HighlightMode::kPreserve, false);
  auto b =
      render_camera(camera, matrix, 1, .25, HighlightMode::kPreserve, true);
  EXPECT_FALSE(a.histograms);
  for (int i = 0; i < 9; ++i)
    EXPECT_EQ(a.image.float_data()[i], b.image.float_data()[i]);
  EXPECT_THROW((void)render_camera(camera, matrix, 0, 0,
                                   static_cast<HighlightMode>(55), false),
               std::invalid_argument);
  EXPECT_THROW((void)render_camera(camera, matrix, 21, 0,
                                   HighlightMode::kCameraClip, false),
               std::invalid_argument);
  EXPECT_THROW((void)render_camera(camera, matrix, 0, 2,
                                   HighlightMode::kCameraClip, false),
               std::invalid_argument);
}
}  // namespace
}  // namespace mini_camera_raw::app

namespace mini_camera_raw::app {
TEST(RenderTest, WorkingExposureOverflowCannotBeHiddenByOutputClipping) {
  auto camera = camera_input();
  camera.float_data()[0] = std::numeric_limits<float>::max();
  CameraToWorkingMatrix identity{{1, 0, 0, 0, 1, 0, 0, 0, 1},
                                 RgbColorSpace::kSrgb};
  EXPECT_THROW((void)render_camera(camera, identity, 1, 0,
                                   HighlightMode::kCameraClip, false),
               std::overflow_error);
}
}  // namespace mini_camera_raw::app

namespace mini_camera_raw::app {
TEST(RenderTest, FiniteCameraMappedToZeroNeedsNoCameraExposureIntermediate) {
  auto camera = camera_input();
  camera.float_data()[0] = std::numeric_limits<float>::max();
  CameraToWorkingMatrix zero{{0, 0, 0, 0, 0, 0, 0, 0, 0}, RgbColorSpace::kSrgb};
  auto result =
      render_camera(camera, zero, 1, 0, HighlightMode::kCameraClip, false);
  for (int i = 0; i < 9; ++i) EXPECT_EQ(result.image.float_data()[i], 0);
}
}  // namespace mini_camera_raw::app
