#include "render_pipeline.h"

#include <stdexcept>
#include <utility>

#include "mini_camera_raw/tone.h"
namespace mini_camera_raw::app {
RenderResult render_camera(const ImageBuffer& camera,
                           const CameraToWorkingMatrix& matrix, double ev,
                           double strength, HighlightMode mode,
                           bool collect_histograms) {
  if (mode != HighlightMode::kPreserve && mode != HighlightMode::kCameraClip)
    throw std::invalid_argument("unknown highlight mode");
  std::optional<StageHistograms> histograms;
  if (collect_histograms) histograms.emplace();
  {
    // Validate and preserve the unbounded editing path even for clipped output.
    auto working = transform_camera_rgb(camera, matrix);
    if (histograms) histograms->before_exposure = compute_histogram(working);
    auto exposed = apply_exposure(working, ev);
    if (histograms) histograms->after_exposure = compute_histogram(exposed);
    auto toned = apply_tone(exposed, strength);
    if (histograms) histograms->after_tone = compute_histogram(toned);
    if (mode == HighlightMode::kPreserve) {
      if (histograms) histograms->rendered = histograms->after_tone;
      return {std::move(toned), std::move(histograms)};
    }
  }
  // P5-compatible output derivative; the original editing path stays intact.
  auto clipped = clip_camera_highlights(camera);
  auto baseline = transform_camera_rgb(clipped, matrix);
  auto rendered = apply_tone(apply_exposure(baseline, ev), strength);
  if (histograms) histograms->rendered = compute_histogram(rendered);
  return {std::move(rendered), std::move(histograms)};
}
}  // namespace mini_camera_raw::app
