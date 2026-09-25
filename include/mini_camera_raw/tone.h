#ifndef MINI_CAMERA_RAW_TONE_H
#define MINI_CAMERA_RAW_TONE_H
#include "mini_camera_raw/image_buffer.h"
namespace mini_camera_raw {
// Explicit linear sRGB working float RGB -> independent packed working RGB.
// out = in * 2^ev, ev finite in [-20,20]. No clipping; padding ignored.
// invalid_argument: state/parameter/nonfinite; overflow_error: float overflow;
// allocation/size errors propagate. See docs/p6-design-preparation.md.
[[nodiscard]] ImageBuffer apply_exposure(const ImageBuffer& input, double ev);
// Linear working sRGB -> tone-mapped working sRGB (linear transfer).
// strength finite in [0,1]; RGB / (1 + strength * max(Y,0)).
// Zero strength copies values exactly but still marks tone completion.
// Same ownership/error conventions as exposure; repeated tone is rejected.
[[nodiscard]] ImageBuffer apply_tone(const ImageBuffer& input, double strength);
}  // namespace mini_camera_raw
#endif
