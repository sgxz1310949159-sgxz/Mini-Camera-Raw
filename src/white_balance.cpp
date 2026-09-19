#include "mini_camera_raw/white_balance.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace mini_camera_raw {
namespace {
void validate_gains(const std::array<double, 4>& tile) {
  for (double gain : tile) {
    if (!std::isfinite(gain) || gain <= 0) {
      throw std::invalid_argument(
          "white balance requires positive finite gains");
    }
  }
}
}  // namespace

WhiteBalanceGains normalize_camera_wb(const std::array<double, 4>& camera_tile,
                                      CfaPattern cfa) {
  std::size_t green_a;
  std::size_t green_b;
  switch (cfa) {
    case CfaPattern::kRggb:
    case CfaPattern::kBggr:
      green_a = 1;
      green_b = 2;
      break;
    case CfaPattern::kGrbg:
    case CfaPattern::kGbrg:
      green_a = 0;
      green_b = 3;
      break;
    default:
      throw std::invalid_argument("camera white balance requires a Bayer CFA");
  }
  validate_gains(camera_tile);
  const double lo = std::min(camera_tile[green_a], camera_tile[green_b]);
  const double hi = std::max(camera_tile[green_a], camera_tile[green_b]);
  // Arithmetic mean without overflowing a sum or halving two tiny equal values.
  const double reference = lo + (hi - lo) / 2;
  WhiteBalanceGains gains{};
  for (std::size_t p = 0; p < 4; ++p) {
    gains.tile[p] = camera_tile[p] / reference;
    if (!std::isfinite(gains.tile[p]) || gains.tile[p] <= 0) {
      throw std::overflow_error(
          "normalized white balance gain is unrepresentable");
    }
  }
  return gains;
}

ImageBuffer apply_white_balance(const ImageBuffer& linear,
                                const WhiteBalanceGains& gains) {
  const auto& input = linear.metadata();
  if (input.color_state != ColorState::kLinearBayer ||
      input.pixel_format != PixelFormat::kFloat32) {
    throw std::invalid_argument("white balance requires float LinearBayer");
  }
  validate_gains(gains.tile);
  auto metadata = input;
  metadata.row_stride_elements = input.width;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < input.height; ++y) {
    const auto* source = linear.float_row(y);
    auto* target = output.float_row(y);
    for (std::size_t x = 0; x < input.width; ++x) {
      if (!std::isfinite(source[x])) {
        throw std::invalid_argument(
            "white balance requires finite active samples");
      }
      const double value = double(source[x]) * gains.tile[2 * (y % 2) + x % 2];
      if (!std::isfinite(value) ||
          std::abs(value) > std::numeric_limits<float>::max()) {
        throw std::overflow_error(
            "white balanced sample cannot be represented as float");
      }
      target[x] = static_cast<float>(value);
    }
  }
  return output;
}
}  // namespace mini_camera_raw
