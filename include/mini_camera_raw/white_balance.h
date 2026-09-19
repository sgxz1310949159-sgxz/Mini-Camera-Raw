#ifndef MINI_CAMERA_RAW_WHITE_BALANCE_H
#define MINI_CAMERA_RAW_WHITE_BALANCE_H

#include <array>

#include "mini_camera_raw/image_buffer.h"

namespace mini_camera_raw {
struct WhiteBalanceGains {
  // Active-origin spatial order: 00, 01, 10, 11; not LibRaw color indices.
  std::array<double, 4> tile;
};

// Divide camera gains by their two-green mean, preserving both green ratios.
// invalid_argument: unknown/None CFA or nonpositive/nonfinite inputs.
// overflow_error: unrepresentable positive normalized gains.
[[nodiscard]] WhiteBalanceGains normalize_camera_wb(
    const std::array<double, 4>& camera_tile, CfaPattern cfa);

// Literal gains, applied exactly once by the caller, before demosaic.
// Requires float LinearBayer. Returns independent packed storage, same
// CFA/size. No clipping; finite active samples only; padding ignored; input
// never modified. invalid_argument: invalid state, sample or gain;
// overflow_error: numeric result; length_error: output size overflow;
// allocation failures propagate.
[[nodiscard]] ImageBuffer apply_white_balance(const ImageBuffer& linear,
                                              const WhiteBalanceGains& gains);
}  // namespace mini_camera_raw
#endif
