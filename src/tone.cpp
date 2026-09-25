#include "mini_camera_raw/tone.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "linear_luminance.h"
namespace mini_camera_raw {
ImageBuffer apply_exposure(const ImageBuffer& input, double ev) {
  auto metadata = input.metadata();
  if (metadata.color_state != ColorState::kLinearWorkingRgb ||
      metadata.rgb_color_space != RgbColorSpace::kSrgb ||
      metadata.transfer_function != TransferFunction::kLinear)
    throw std::invalid_argument(
        "exposure requires explicit linear working sRGB");
  if (!std::isfinite(ev) || ev < -20 || ev > 20)
    throw std::invalid_argument("EV must be finite in [-20,20]");
  const double gain = std::exp2(ev);
  metadata.row_stride_elements = metadata.width * 3;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* source = input.float_row(y);
    float* target = output.float_row(y);
    for (std::size_t i = 0; i < metadata.width * 3; ++i) {
      if (!std::isfinite(source[i]))
        throw std::invalid_argument("nonfinite active exposure sample");
      const double value = double(source[i]) * gain;
      if (std::abs(value) > std::numeric_limits<float>::max())
        throw std::overflow_error("exposed sample cannot be represented");
      target[i] = ev == 0 ? source[i] : static_cast<float>(value);
    }
  }
  return output;
}
ImageBuffer apply_tone(const ImageBuffer& input, double strength) {
  auto metadata = input.metadata();
  if (metadata.color_state != ColorState::kLinearWorkingRgb ||
      metadata.rgb_color_space != RgbColorSpace::kSrgb ||
      metadata.transfer_function != TransferFunction::kLinear)
    throw std::invalid_argument("tone requires explicit linear working sRGB");
  if (!std::isfinite(strength) || strength < 0 || strength > 1)
    throw std::invalid_argument("tone strength must be finite in [0,1]");
  metadata.row_stride_elements = metadata.width * 3;
  metadata.color_state = ColorState::kToneMappedWorkingRgb;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* source = input.float_row(y);
    float* target = output.float_row(y);
    for (std::size_t x = 0; x < metadata.width; ++x) {
      const float* rgb = source + 3 * x;
      for (int c = 0; c < 3; ++c)
        if (!std::isfinite(rgb[c]))
          throw std::invalid_argument("nonfinite active tone sample");
      const double luminance = detail::linear_srgb_luminance(rgb);
      // The denominator is >= 1: no singular division or magnitude expansion.
      const double scale = 1 / (1 + strength * std::max(luminance, 0.0));
      for (int c = 0; c < 3; ++c)
        target[3 * x + c] = strength == 0 || luminance <= 0
                                ? rgb[c]
                                : static_cast<float>(double(rgb[c]) * scale);
    }
  }
  return output;
}
}  // namespace mini_camera_raw
