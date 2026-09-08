#ifndef MINI_CAMERA_RAW_IMAGE_METADATA_H
#define MINI_CAMERA_RAW_IMAGE_METADATA_H

#include <cstddef>

namespace mini_camera_raw {

enum class PixelFormat {
  kUInt16,
  kFloat32,
};

enum class CfaPattern {
  kNone,
  kRggb,
  kBggr,
  kGrbg,
  kGbrg,
};

enum class ColorState {
  kRawBayer,
  kLinearBayer,
  kLinearCameraRgb,
  kLinearWorkingRgb,
};

struct NumericRange {
  // Declared sample semantics only; ImageBuffer never clips to this interval.
  double nominal_min;
  double nominal_max;
  bool allows_out_of_range;
};

struct ImageMetadata {
  std::size_t width;
  std::size_t height;
  std::size_t channel_count;
  // Number of elements from one row start to the next, including padding.
  std::size_t row_stride_elements;
  PixelFormat pixel_format;
  CfaPattern cfa_pattern;
  ColorState color_state;
  NumericRange numeric_range;
};

}  // namespace mini_camera_raw

#endif
