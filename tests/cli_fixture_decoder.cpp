// Test-only decoder substitute linked into a separate CLI executable.
// Production CLI still links the real LibRaw decoder; no synthetic input mode.
#include <stdexcept>

#include "mini_camera_raw/raw_decoder.h"
namespace mini_camera_raw {
DecodedRaw decode_raw(const std::string& path) {
  if (path != "synthetic" && path != "saturated" && path != "missing-wb" &&
      path != "missing-matrix")
    throw std::runtime_error("synthetic decoder rejected input");
  ImageMetadata m{3,
                  3,
                  1,
                  3,
                  PixelFormat::kUInt16,
                  CfaPattern::kRggb,
                  ColorState::kRawBayer,
                  {0, 4096, false}};
  SensorMetadata sensor{};
  sensor.levels = {{0, 0, 0, 0}, {4096, 4096, 4096, 4096}};
  if (path != "missing-wb")
    sensor.camera_wb_tile = std::array<double, 4>{2, 1, 1, 1.5};
  if (path != "missing-matrix")
    sensor.camera_to_srgb =
        std::array<double, 12>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
  return {
      ImageBuffer::from_uint16(
          m, ImageBuffer::UInt16Storage(9, path == "saturated" ? 4096 : 1024)),
      sensor};
}
}  // namespace mini_camera_raw
