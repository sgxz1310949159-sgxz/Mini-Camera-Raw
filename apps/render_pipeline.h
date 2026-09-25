#ifndef MINI_CAMERA_RAW_APP_RENDER_PIPELINE_H
#define MINI_CAMERA_RAW_APP_RENDER_PIPELINE_H
#include <optional>

#include "mini_camera_raw/color_transform.h"
#include "mini_camera_raw/histogram.h"
namespace mini_camera_raw::app {
enum class HighlightMode { kPreserve, kCameraClip };
struct StageHistograms {
  RgbLuminanceHistogram before_exposure;
  RgbLuminanceHistogram after_exposure;
  RgbLuminanceHistogram after_tone;
  RgbLuminanceHistogram rendered;
};
struct RenderResult {
  ImageBuffer image;
  std::optional<StageHistograms> histograms;
};
// App-internal coordinator. Original camera source is immutable. No encoding.
// Main path always derives from unclipped working RGB. Camera clipping affects
// only the output derivative, before matrix/exposure/tone.
// kCameraClip is the normal P5 baseline; kPreserve is uncorrected diagnostic
// output.
[[nodiscard]] RenderResult render_camera(const ImageBuffer& camera,
                                         const CameraToWorkingMatrix& matrix,
                                         double ev, double strength,
                                         HighlightMode mode,
                                         bool collect_histograms);
}  // namespace mini_camera_raw::app
#endif
