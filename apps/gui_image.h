#ifndef MINI_CAMERA_RAW_GUI_IMAGE_H
#define MINI_CAMERA_RAW_GUI_IMAGE_H
#include "mini_camera_raw/image_buffer.h"
#include <QImage>
namespace mini_camera_raw {
// Copies encoded RGB16 sRGB into owned opaque RGBX64. No transfer conversion.
// Rejects incompatible metadata; allocation failure propagates as bad_alloc.
QImage make_gui_image(const ImageBuffer& encoded);
}
#endif
