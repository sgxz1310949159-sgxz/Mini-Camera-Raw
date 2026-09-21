#include "mini_camera_raw/color_transform.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw {
namespace {
ImageMetadata camera(std::size_t w = 1, std::size_t h = 1,
                     std::size_t stride = 3) {
  return {w,
          h,
          3,
          stride,
          PixelFormat::kFloat32,
          CfaPattern::kNone,
          ColorState::kLinearCameraRgb,
          {0, 1, true}};
}
const CameraToWorkingMatrix identity{{1, 0, 0, 0, 1, 0, 0, 0, 1},
                                     RgbColorSpace::kSrgb};
TEST(ColorTransformTest, DirectionBasisAndOutOfRangeAreExplicit) {
  auto input =
      ImageBuffer::from_float(camera(3, 1, 9), {1, 0, 0, 0, 1, 0, 0, 0, 1});
  CameraToWorkingMatrix matrix{{2, -1, 0, 0, 3, 1, -2, 0, 4},
                               RgbColorSpace::kSrgb};
  auto output = transform_camera_rgb(input, matrix);
  const float expected[] = {2, 0, -2, -1, 3, 0, 0, 1, 4};
  for (int i = 0; i < 9; ++i)
    EXPECT_FLOAT_EQ(output.float_data()[i], expected[i]);
  EXPECT_EQ(output.metadata().rgb_color_space, RgbColorSpace::kSrgb);
  EXPECT_EQ(output.metadata().transfer_function, TransferFunction::kLinear);
  EXPECT_EQ(output.metadata().color_state, ColorState::kLinearWorkingRgb);
  EXPECT_FLOAT_EQ(input.float_data()[0], 1);
  EXPECT_NE(input.float_data(), output.float_data());
  EXPECT_THROW(static_cast<void>(transform_camera_rgb(output, identity)),
               std::invalid_argument);
}
TEST(ColorTransformTest, MetadataRequiresFiniteNonzeroThreeColorBasis) {
  SensorMetadata sensor{};
  EXPECT_THROW(static_cast<void>(camera_to_working_matrix(sensor)),
               std::invalid_argument);
  sensor.camera_to_srgb =
      std::array<double, 12>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
  EXPECT_EQ(camera_to_working_matrix(sensor).coefficients,
            identity.coefficients);
  for (int i = 0; i < 12; ++i) {
    auto bad = sensor;
    (*bad.camera_to_srgb)[i] = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(static_cast<void>(camera_to_working_matrix(bad)),
                 std::invalid_argument);
  }
  for (int i : {3, 7, 11}) {
    auto bad = sensor;
    (*bad.camera_to_srgb)[i] = 1e-30;
    EXPECT_THROW(static_cast<void>(camera_to_working_matrix(bad)),
                 std::invalid_argument);
  }
  sensor.camera_to_srgb = std::array<double, 12>{};
  EXPECT_THROW(static_cast<void>(camera_to_working_matrix(sensor)),
               std::invalid_argument);
}
TEST(ColorTransformTest, GeneratedIdentityLinearityAndPadding) {
  for (std::size_t width : {1, 3, 7}) {
    auto m = camera(width, 3, width * 3 + 2);
    auto input = ImageBuffer::create(m);
    for (std::size_t y = 0; y < 3; ++y) {
      for (std::size_t x = 0; x < width * 3; ++x)
        input.float_row(y)[x] = float(int(x + 7 * y) - 10) / 8;
      input.float_row(y)[width * 3] = std::numeric_limits<float>::quiet_NaN();
    }
    auto output = transform_camera_rgb(input, identity);
    CameraToWorkingMatrix mix{{2, -1, 0, 0, 1, 0, 1, 0, 1},
                              RgbColorSpace::kSrgb};
    auto mixed = transform_camera_rgb(input, mix);
    for (std::size_t y = 0; y < 3; ++y)
      for (std::size_t x = 0; x < width; ++x) {
        for (int c = 0; c < 3; ++c)
          EXPECT_FLOAT_EQ(output.float_at(y, x, c), input.float_at(y, x, c));
        EXPECT_FLOAT_EQ(mixed.float_at(y, x, 0),
                        2 * input.float_at(y, x, 0) - input.float_at(y, x, 1));
        EXPECT_FLOAT_EQ(mixed.float_at(y, x, 2),
                        input.float_at(y, x, 0) + input.float_at(y, x, 2));
      }
    EXPECT_EQ(output.metadata().row_stride_elements, width * 3);
  }
}
TEST(ColorTransformTest, RejectsNonfiniteUnknownDestinationAndOverflow) {
  auto input = ImageBuffer::from_float(camera(), {1, 2, 3});
  for (float value : {std::numeric_limits<float>::quiet_NaN(),
                      std::numeric_limits<float>::infinity(),
                      -std::numeric_limits<float>::infinity()})
    for (int c = 0; c < 3; ++c) {
      auto bad = input;
      bad.float_data()[c] = value;
      EXPECT_THROW(static_cast<void>(transform_camera_rgb(bad, identity)),
                   std::invalid_argument);
    }
  for (int c = 0; c < 9; ++c) {
    auto bad = identity;
    bad.coefficients[c] = std::numeric_limits<double>::infinity();
    EXPECT_THROW(static_cast<void>(transform_camera_rgb(input, bad)),
                 std::invalid_argument);
  }
  auto bad = identity;
  bad.destination = RgbColorSpace::kUnspecified;
  EXPECT_THROW(static_cast<void>(transform_camera_rgb(input, bad)),
               std::invalid_argument);
  bad = identity;
  bad.coefficients[0] = std::numeric_limits<double>::max();
  EXPECT_THROW(static_cast<void>(transform_camera_rgb(input, bad)),
               std::overflow_error);
  bad = identity;
  bad.coefficients[0] = std::numeric_limits<float>::max();
  input.float_data()[0] = 2;
  EXPECT_THROW(static_cast<void>(transform_camera_rgb(input, bad)),
               std::overflow_error);
}
TEST(ColorTransformTest,
     HighlightClipNeutralizesCoreWithoutWhiteningEveryHighlight) {
  auto input = ImageBuffer::from_float(
      camera(3, 1, 9), {2.37891f, 1, 1.59863f, 2, .2f, .1f, -.2f, .5f, 1});
  auto clipped = clip_camera_highlights(input);
  CameraToWorkingMatrix matrix{
      {1.636061788, -.4486187398, -.1874430031, -.1151647642, 1.488970757,
       -.3738060296, -.004740227945, -.3236012459, 1.328341484},
      RgbColorSpace::kSrgb};
  auto working = transform_camera_rgb(clipped, matrix);
  for (int c = 0; c < 3; ++c) EXPECT_NEAR(working.float_at(0, 0, c), 1, 1e-6);
  EXPECT_FLOAT_EQ(clipped.float_at(0, 1, 0), 1);
  EXPECT_FLOAT_EQ(clipped.float_at(0, 1, 1), .2f);
  EXPECT_FLOAT_EQ(clipped.float_at(0, 1, 2), .1f);
  EXPECT_FLOAT_EQ(input.float_at(0, 0, 0), 2.37891f);
  EXPECT_FLOAT_EQ(input.float_at(0, 1, 0), 2);
  EXPECT_NE(input.float_data(), clipped.float_data());
}
TEST(ColorTransformTest, HighlightClipGeneratedPropertiesAndPadding) {
  for (std::size_t width : {1, 3, 7}) {
    auto input = ImageBuffer::create(camera(width, 3, width * 3 + 1));
    for (std::size_t y = 0; y < 3; ++y) {
      for (std::size_t i = 0; i < width * 3; ++i)
        input.float_row(y)[i] = (float(i + y) - 3) / 4;
      input.float_row(y)[width * 3] = std::numeric_limits<float>::quiet_NaN();
    }
    auto out = clip_camera_highlights(input);
    auto twice = clip_camera_highlights(out);
    EXPECT_EQ(out.metadata().row_stride_elements, width * 3);
    EXPECT_EQ(out.metadata().color_state, ColorState::kLinearCameraRgb);
    for (std::size_t y = 0; y < 3; ++y)
      for (std::size_t i = 0; i < width * 3; ++i) {
        float v = input.float_row(y)[i];
        EXPECT_FLOAT_EQ(out.float_row(y)[i], v > 1 ? 1 : v);
        EXPECT_FLOAT_EQ(twice.float_row(y)[i], out.float_row(y)[i]);
        EXPECT_LE(out.float_row(y)[i], 1);
        if (i) EXPECT_LE(out.float_row(y)[i - 1], out.float_row(y)[i]);
      }
  }
}
TEST(ColorTransformTest, HighlightClipRejectsWrongStateAndNonfinite) {
  auto input = ImageBuffer::from_float(camera(), {0, 1, 2});
  auto working = transform_camera_rgb(input, identity);
  EXPECT_THROW(static_cast<void>(clip_camera_highlights(working)),
               std::invalid_argument);
  for (float v : {std::numeric_limits<float>::quiet_NaN(),
                  std::numeric_limits<float>::infinity(),
                  -std::numeric_limits<float>::infinity()}) {
    input.float_data()[1] = v;
    EXPECT_THROW(static_cast<void>(clip_camera_highlights(input)),
                 std::invalid_argument);
  }
}
}  // namespace
}  // namespace mini_camera_raw
