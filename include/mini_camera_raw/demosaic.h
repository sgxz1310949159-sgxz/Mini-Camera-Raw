#ifndef MINI_CAMERA_RAW_DEMOSAIC_H
#define MINI_CAMERA_RAW_DEMOSAIC_H

#include "mini_camera_raw/image_buffer.h"

namespace mini_camera_raw {
// Scalar bilinear interpolation of float LinearBayer, width/height >= 2.
// Keeps measured channels exactly; averages in-bounds same-color neighbors.
// Returns owned packed R,G,B LinearCameraRgb with no CFA, rotation or clipping.
// Active NaN/Inf and incompatible states/dimensions: invalid_argument.
// Output size overflow: length_error; allocation exceptions propagate.
// Input and padding are never modified; padding does not participate.
[[nodiscard]] ImageBuffer demosaic_bilinear(const ImageBuffer& linear);
}  // namespace mini_camera_raw
#endif
