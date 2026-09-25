#ifndef MINI_CAMERA_RAW_LINEAR_LUMINANCE_H
#define MINI_CAMERA_RAW_LINEAR_LUMINANCE_H
namespace mini_camera_raw::detail {
// D65 sRGB -> XYZ Y row, W3C CSS Color 4 conversion reference.
// https://www.w3.org/TR/2026/CRD-css-color-4-20260913/#color-conversion-code
inline double linear_srgb_luminance(const float* rgb) {
  return (87098.0 / 409605.0) * rgb[0] + (175762.0 / 245763.0) * rgb[1] +
         (12673.0 / 175545.0) * rgb[2];
}
}  // namespace mini_camera_raw::detail
#endif
