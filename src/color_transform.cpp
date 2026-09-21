#include "mini_camera_raw/color_transform.h"

#include <cmath>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw {
CameraToWorkingMatrix camera_to_working_matrix(const SensorMetadata& sensor) {
  if (!sensor.camera_to_srgb)
    throw std::invalid_argument("missing camera color matrix");
  const auto& source = *sensor.camera_to_srgb;
  for (double v : source)
    if (!std::isfinite(v))
      throw std::invalid_argument("nonfinite camera matrix");
  CameraToWorkingMatrix result{{}, RgbColorSpace::kSrgb};
  bool nonzero = false;
  for (std::size_t r = 0; r < 3; ++r) {
    if (source[r * 4 + 3] != 0)
      throw std::invalid_argument("unsupported four-color matrix basis");
    for (std::size_t c = 0; c < 3; ++c) {
      result.coefficients[r * 3 + c] = source[r * 4 + c];
      nonzero = nonzero || source[r * 4 + c] != 0;
    }
  }
  if (!nonzero) throw std::invalid_argument("empty camera color matrix");
  return result;
}
ImageBuffer clip_camera_highlights(const ImageBuffer& input) {
  auto metadata = input.metadata();
  if (metadata.color_state != ColorState::kLinearCameraRgb)
    throw std::invalid_argument("highlight clipping requires camera RGB");
  metadata.row_stride_elements = metadata.width * 3;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* source = input.float_row(y);
    float* target = output.float_row(y);
    for (std::size_t i = 0; i < metadata.width * 3; ++i) {
      const float value = source[i];
      if (!std::isfinite(value))
        throw std::invalid_argument("nonfinite active camera sample");
      target[i] = value > 1.f ? 1.f : value;
    }
  }
  return output;
}
ImageBuffer transform_camera_rgb(const ImageBuffer& input,
                                 const CameraToWorkingMatrix& matrix) {
  auto metadata = input.metadata();
  if (metadata.color_state != ColorState::kLinearCameraRgb ||
      matrix.destination != RgbColorSpace::kSrgb)
    throw std::invalid_argument(
        "requires camera RGB and supported working space");
  for (double v : matrix.coefficients)
    if (!std::isfinite(v))
      throw std::invalid_argument("nonfinite matrix coefficient");
  metadata.color_state = ColorState::kLinearWorkingRgb;
  metadata.rgb_color_space = matrix.destination;
  metadata.transfer_function = TransferFunction::kLinear;
  metadata.row_stride_elements =
      metadata.width * 3;  // Validated input layout bounds it.
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* source = input.float_row(y);
    float* target = output.float_row(y);
    for (std::size_t x = 0; x < metadata.width; ++x) {
      for (std::size_t c = 0; c < 3; ++c)
        if (!std::isfinite(source[x * 3 + c]))
          throw std::invalid_argument("nonfinite active camera sample");
      for (std::size_t r = 0; r < 3; ++r) {
        double value = 0;
        for (std::size_t c = 0; c < 3; ++c)
          value += matrix.coefficients[r * 3 + c] * double(source[x * 3 + c]);
        if (!std::isfinite(value) ||
            std::abs(value) > std::numeric_limits<float>::max())
          throw std::overflow_error("working RGB sample cannot be represented");
        target[x * 3 + r] = static_cast<float>(value);
      }
    }
  }
  return output;
}
}  // namespace mini_camera_raw
