#ifndef MINI_CAMERA_RAW_NORMALIZE_H
#define MINI_CAMERA_RAW_NORMALIZE_H

#include "mini_camera_raw/image_buffer.h"

#include <array>

namespace mini_camera_raw {

struct SensorLevels {
  // Decoded code units, visible 2x2 tile order: (0,0), (0,1), (1,0), (1,1).
  std::array<double, 4> black;
  std::array<double, 4> white;
};

// RawBayer uint16 -> packed float LinearBayer; preserves CFA and never clips.
// invalid_argument: incompatible input/levels; overflow_error: float overflow.
// Full contract: docs/p3-raw-normalization-spec.md.
[[nodiscard]] ImageBuffer normalize(const ImageBuffer& raw,
                                    const SensorLevels& levels);

}  // namespace mini_camera_raw
#endif
