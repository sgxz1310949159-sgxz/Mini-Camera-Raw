#ifndef MINI_CAMERA_RAW_PNG_WRITER_H
#define MINI_CAMERA_RAW_PNG_WRITER_H
#include <string>

#include "mini_camera_raw/image_buffer.h"
namespace mini_camera_raw {
// Writes encoded sRGB RGB16, sRGB chunk, no alpha/rotation/private metadata.
// POSIX exclusive creation: never overwrite an existing target (including
// symlinks). Input/padding unchanged. Invalid state/path: invalid_argument;
// PNG/size limits: length_error; I/O/encoder: runtime_error; allocation
// exceptions propagate. Normal failures clean this call's partial file; crashes
// are not atomic.
[[nodiscard]] const char* png_library_version() noexcept;
void write_png16(const ImageBuffer& encoded, const std::string& path);
}  // namespace mini_camera_raw
#endif
