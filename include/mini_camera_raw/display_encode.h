#ifndef MINI_CAMERA_RAW_DISPLAY_ENCODE_H
#define MINI_CAMERA_RAW_DISPLAY_ENCODE_H
#include "mini_camera_raw/image_buffer.h"
namespace mini_camera_raw {
// Explicit sRGB/linear working float RGB -> independently owned packed RGB16.
// Validate finite active samples, clamp output only, sRGB transfer, nearest
// integer (half up). Input/padding untouched. No exposure/tone/rotation.
// Unknown spaces or encoded input: invalid_argument. Size: length_error;
// allocation errors propagate.
[[nodiscard]] ImageBuffer encode_srgb16(const ImageBuffer& linear);
}  // namespace mini_camera_raw
#endif
