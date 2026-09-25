#ifndef MINI_CAMERA_RAW_HISTOGRAM_H
#define MINI_CAMERA_RAW_HISTOGRAM_H
#include <array>
#include <cstdint>

#include "mini_camera_raw/image_buffer.h"
namespace mini_camera_raw {
struct ChannelHistogram {
  std::array<std::uint64_t, 256> bins{};
  std::uint64_t below_zero = 0;
  std::uint64_t above_one = 0;
  double minimum = 0;
  double maximum = 0;
};
struct RgbLuminanceHistogram {
  // R, G, B, relative linear Y; each channel accounts for every pixel once.
  std::array<ChannelHistogram, 4> channels;
};
// Explicit linear sRGB working/tone-mapped states only; read-only active
// samples, padding ignored.
// [0,1): floor(256*v), 1: bin 255; outliers counted separately, not clamped.
// invalid_argument for incompatible state or nonfinite active components.
[[nodiscard]] RgbLuminanceHistogram compute_histogram(const ImageBuffer& input);
}  // namespace mini_camera_raw
#endif
