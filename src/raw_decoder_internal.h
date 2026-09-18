#ifndef MINI_CAMERA_RAW_RAW_DECODER_INTERNAL_H
#define MINI_CAMERA_RAW_RAW_DECODER_INTERNAL_H

#include "mini_camera_raw/raw_decoder.h"
class LibRaw;

namespace mini_camera_raw::detail {
// Internal borrowed-buffer boundary. Caller guarantees pointer's actual extent.
// Geometry and supplied extent are checked before any sample access.
ImageBuffer copy_active_mosaic(const std::uint16_t* samples, std::size_t count,
                              const SensorGeometry& geometry, CfaPattern cfa);
DecodedRaw copy_libraw_result(LibRaw& decoder);
}  // namespace mini_camera_raw::detail
#endif
