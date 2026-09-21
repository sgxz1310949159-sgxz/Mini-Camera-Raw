#include "mini_camera_raw/display_encode.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace mini_camera_raw {
ImageBuffer encode_srgb16(const ImageBuffer& linear) {
  auto metadata = linear.metadata();
  if (metadata.color_state != ColorState::kLinearWorkingRgb ||
      metadata.rgb_color_space != RgbColorSpace::kSrgb ||
      metadata.transfer_function != TransferFunction::kLinear)
    throw std::invalid_argument("sRGB output requires explicit linear sRGB");
  metadata.pixel_format = PixelFormat::kUInt16;
  metadata.color_state = ColorState::kEncodedRgb;
  metadata.transfer_function = TransferFunction::kSrgb;
  metadata.numeric_range = {0, 65535, false};
  metadata.row_stride_elements = metadata.width * 3;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* source = linear.float_row(y);
    auto* target = output.uint16_row(y);
    for (std::size_t x = 0; x < metadata.width * 3; ++x) {
      if (!std::isfinite(source[x]))
        throw std::invalid_argument("nonfinite active working sample");
      const double v = std::clamp(double(source[x]), 0.0, 1.0);
      const double encoded =
          v <= 0.0031308 ? 12.92 * v : 1.055 * std::pow(v, 1.0 / 2.4) - 0.055;
      target[x] = static_cast<std::uint16_t>(std::floor(encoded * 65535 + 0.5));
    }
  }
  return output;
}
}  // namespace mini_camera_raw
