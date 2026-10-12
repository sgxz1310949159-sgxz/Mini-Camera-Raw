#ifndef MINI_CAMERA_RAW_EDITOR_PIPELINE_H
#define MINI_CAMERA_RAW_EDITOR_PIPELINE_H
#include "mini_camera_raw/color_transform.h"
namespace mini_camera_raw::app {
// Immutable camera RGB -> linear working sRGB with exposure, no clipping/tone.
// Preserves signed headroom until final encoding. Uses existing validation/errors.
[[nodiscard]] ImageBuffer render_editor_linear(const ImageBuffer& camera,
    const CameraToWorkingMatrix& matrix, double ev);
// Shared full-size RGB16 sRGB output for preview/export; final encoding clips.
[[nodiscard]] ImageBuffer render_editor_output(const ImageBuffer& camera,
    const CameraToWorkingMatrix& matrix, double ev);
// Supported LibRaw flip values: 0,3,5,6; exact pixel permutation, no interpolation.
[[nodiscard]] ImageBuffer orient_editor_output(const ImageBuffer& encoded, int orientation);
// Consumes an owned output; packed orientation-zero output keeps its storage.
[[nodiscard]] ImageBuffer orient_editor_output(ImageBuffer&& encoded, int orientation);
struct EditorPhoto {
  ImageBuffer camera;
  CameraToWorkingMatrix matrix;
  int orientation;
};
[[nodiscard]] EditorPhoto prepare_editor_photo(const DecodedRaw& decoded);
[[nodiscard]] EditorPhoto open_editor_photo(const std::string& path);
// Always starts from sensor-order camera data; applies orientation exactly once.
[[nodiscard]] ImageBuffer render_editor_photo(const EditorPhoto& photo, double ev);
}
#endif
