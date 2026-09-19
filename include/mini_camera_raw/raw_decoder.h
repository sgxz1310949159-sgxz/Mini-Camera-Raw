#ifndef MINI_CAMERA_RAW_RAW_DECODER_H
#define MINI_CAMERA_RAW_RAW_DECODER_H

#include "mini_camera_raw/normalize.h"

#include <optional>
#include <string>

namespace mini_camera_raw {
struct SensorGeometry {
  std::size_t raw_width;
  std::size_t raw_height;
  std::size_t active_left;
  std::size_t active_top;
  std::size_t active_width;
  std::size_t active_height;
  std::size_t raw_pitch_bytes;
};

struct SensorMetadata {
  SensorGeometry geometry;
  SensorLevels levels;
  unsigned source_bits;  // LibRaw raw_bps; 0 = unknown, not storage precision.
  int orientation;       // LibRaw flip; image is not rotated.
  std::optional<std::array<double, 4>> camera_wb_tile;
  // Row-major 3x4 LibRaw rgb_cam, camera RGBG -> linear sRGB; not applied.
  std::optional<std::array<double, 12>> camera_to_srgb;
  std::string libraw_version;
  unsigned unpack_options;
};

struct DecodedRaw {
  ImageBuffer image;
  SensorMetadata sensor;
};

// Native Sony ILCE-7CM2 single-frame integer Bayer only. No ISP/rotation.
// Returns independent owned storage; no private filename retained in metadata.
// invalid_argument: invalid path/metadata; length_error: overflow;
// runtime_error: unsupported input, I/O or decode failure.
[[nodiscard]] DecodedRaw decode_raw(const std::string& path);
}  // namespace mini_camera_raw
#endif
