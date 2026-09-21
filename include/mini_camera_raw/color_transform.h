#ifndef MINI_CAMERA_RAW_COLOR_TRANSFORM_H
#define MINI_CAMERA_RAW_COLOR_TRANSFORM_H
#include "mini_camera_raw/raw_decoder.h"
namespace mini_camera_raw {
struct CameraToWorkingMatrix {
  std::array<double, 9>
      coefficients;  // Row-major; output = M * RGB column vector.
  RgbColorSpace destination;
};
// Supported three-color LibRaw metadata only; no green-column folding or WB.
// Missing/nonfinite/zero matrix or nonzero fourth column: invalid_argument.
[[nodiscard]] CameraToWorkingMatrix camera_to_working_matrix(
    const SensorMetadata& sensor);
// Explicit output-only hard highlight clipping after WB, before the matrix.
// Packed owned LinearCameraRgb copy: min(channel, 1), negatives preserved.
// Requires P3/P4 nominal [0,1] scale and green-normalized camera WB.
// Source unchanged; no reconstruction. Invalid state/nonfinite:
// invalid_argument.
[[nodiscard]] ImageBuffer clip_camera_highlights(const ImageBuffer& input);
// Immutable float LinearCameraRgb -> packed linear working RGB. Only sRGB in
// P5. No clipping/WB/rotation. Padding ignored. invalid_argument:
// state/nonfinite; overflow_error: arithmetic; length_error: size; allocation
// failures propagate.
[[nodiscard]] ImageBuffer transform_camera_rgb(
    const ImageBuffer& input, const CameraToWorkingMatrix& matrix);
}  // namespace mini_camera_raw
#endif
