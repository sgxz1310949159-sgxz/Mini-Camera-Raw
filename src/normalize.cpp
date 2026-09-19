#include "mini_camera_raw/normalize.h"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace mini_camera_raw {

ImageBuffer normalize(const ImageBuffer& raw, const SensorLevels& levels) {
  const auto& input = raw.metadata();
  if (input.color_state != ColorState::kRawBayer ||
      input.pixel_format != PixelFormat::kUInt16) {
    throw std::invalid_argument("normalization requires uint16 RawBayer");
  }
  std::array<double, 4> denominator{};
  for (std::size_t p = 0; p < 4; ++p) {
    if (!std::isfinite(levels.black[p]) || !std::isfinite(levels.white[p]) ||
        levels.black[p] < 0 || levels.white[p] > 65535 ||
        levels.white[p] <= levels.black[p]) {
      throw std::invalid_argument("sensor levels require 0 <= black < white <= 65535");
    }
    denominator[p] = levels.white[p] - levels.black[p];
  }
  auto metadata = input;
  metadata.row_stride_elements = input.width;
  metadata.pixel_format = PixelFormat::kFloat32;
  metadata.color_state = ColorState::kLinearBayer;
  metadata.numeric_range = {0, 1, true};
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < input.height; ++y) {
    const auto* source = raw.uint16_row(y);
    auto* target = output.float_row(y);
    for (std::size_t x = 0; x < input.width; ++x) {
      if (source[x] > input.numeric_range.nominal_max) {
        throw std::invalid_argument("raw sample exceeds declared code range");
      }
      const std::size_t p = 2 * (y % 2) + x % 2;
      const double value = (static_cast<double>(source[x]) - levels.black[p]) /
                           denominator[p];
      if (!std::isfinite(value) || std::abs(value) > std::numeric_limits<float>::max()) {
        throw std::overflow_error("normalized sample cannot be represented as float");
      }
      target[x] = static_cast<float>(value);
    }
  }
  return output;
}
}  // namespace mini_camera_raw
