#include "mini_camera_raw/histogram.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "linear_luminance.h"
namespace mini_camera_raw {
RgbLuminanceHistogram compute_histogram(const ImageBuffer& input) {
  const auto& metadata = input.metadata();
  if ((metadata.color_state != ColorState::kLinearWorkingRgb &&
       metadata.color_state != ColorState::kToneMappedWorkingRgb) ||
      metadata.rgb_color_space != RgbColorSpace::kSrgb ||
      metadata.transfer_function != TransferFunction::kLinear)
    throw std::invalid_argument(
        "histogram requires explicit linear working sRGB");
  RgbLuminanceHistogram result{};
  for (auto& c : result.channels) {
    c.minimum = std::numeric_limits<double>::infinity();
    c.maximum = -std::numeric_limits<double>::infinity();
  }
  for (std::size_t y = 0; y < metadata.height; ++y) {
    const float* row = input.float_row(y);
    for (std::size_t x = 0; x < metadata.width; ++x) {
      const float* rgb = row + 3 * x;
      for (int c = 0; c < 3; ++c)
        if (!std::isfinite(rgb[c]))
          throw std::invalid_argument("nonfinite active histogram sample");
      const double values[] = {rgb[0], rgb[1], rgb[2],
                               detail::linear_srgb_luminance(rgb)};
      for (std::size_t c = 0; c < 4; ++c) {
        auto& channel = result.channels[c];
        const double v = values[c];
        channel.minimum = std::min(channel.minimum, v);
        channel.maximum = std::max(channel.maximum, v);
        if (v < 0)
          ++channel.below_zero;
        else if (v > 1)
          ++channel.above_one;
        else
          ++channel.bins[v == 1 ? 255 : static_cast<std::size_t>(v * 256)];
      }
    }
  }
  return result;
}
}  // namespace mini_camera_raw
