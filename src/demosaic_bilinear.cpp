#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "mini_camera_raw/demosaic.h"

namespace mini_camera_raw {
ImageBuffer demosaic_bilinear(const ImageBuffer& linear) {
  const auto& input = linear.metadata();
  if (input.color_state != ColorState::kLinearBayer ||
      input.pixel_format != PixelFormat::kFloat32 || input.width < 2 ||
      input.height < 2) {
    throw std::invalid_argument(
        "demosaic requires float LinearBayer at least 2x2");
  }
  std::size_t red_x;
  std::size_t red_y;
  switch (input.cfa_pattern) {
    case CfaPattern::kRggb:
      red_x = 0;
      red_y = 0;
      break;
    case CfaPattern::kBggr:
      red_x = 1;
      red_y = 1;
      break;
    case CfaPattern::kGrbg:
      red_x = 1;
      red_y = 0;
      break;
    case CfaPattern::kGbrg:
      red_x = 0;
      red_y = 1;
      break;
    default:
      throw std::invalid_argument("demosaic requires a Bayer CFA");
  }
  const auto color_at = [red_x, red_y](std::size_t y, std::size_t x) {
    const bool red_column = x % 2 == red_x;
    const bool red_row = y % 2 == red_y;
    return red_column && red_row ? 0 : (!red_column && !red_row ? 2 : 1);
  };
  for (std::size_t y = 0; y < input.height; ++y) {
    const auto* row = linear.float_row(y);
    for (std::size_t x = 0; x < input.width; ++x) {
      if (!std::isfinite(row[x])) {
        throw std::invalid_argument("demosaic requires finite active samples");
      }
    }
  }
  auto metadata = input;
  if (input.width > std::numeric_limits<std::size_t>::max() / 3) {
    throw std::length_error("RGB row element count overflows size_t");
  }
  metadata.channel_count = 3;
  metadata.row_stride_elements = input.width * 3;
  metadata.color_state = ColorState::kLinearCameraRgb;
  metadata.cfa_pattern = CfaPattern::kNone;
  auto output = ImageBuffer::create(metadata);
  for (std::size_t y = 0; y < input.height; ++y) {
    const auto* measured_row = linear.float_row(y);
    auto* target = output.float_row(y);
    for (std::size_t x = 0; x < input.width; ++x) {
      std::array<double, 3> sums{};
      std::array<unsigned, 3> counts{};
      // In a Bayer 3x3 neighborhood, each missing color has exactly the
      // bilinear axial/diagonal support. Omit out-of-image neighbors.
      for (std::size_t ny = y == 0 ? 0 : y - 1;
           ny <= std::min(y + 1, input.height - 1); ++ny) {
        const auto* row = linear.float_row(ny);
        for (std::size_t nx = x == 0 ? 0 : x - 1;
             nx <= std::min(x + 1, input.width - 1); ++nx) {
          const auto color = color_at(ny, nx);
          sums[color] += double(row[nx]);
          ++counts[color];
        }
      }
      const int measured = color_at(y, x);
      for (int color = 0; color < 3; ++color) {
        // Accepted dimensions guarantee nonempty support. A convex average
        // of finite floats fits in float; double safely holds up to nine terms.
        target[3 * x + color] =
            color == measured ? measured_row[x]
                              : static_cast<float>(sums[color] / counts[color]);
      }
    }
  }
  return output;
}
}  // namespace mini_camera_raw
