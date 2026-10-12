#include "gui_image.h"
#include <QColorSpace>
#include <QRgba64>
#include <limits>
#include <new>
#include <stdexcept>
namespace mini_camera_raw {
QImage make_gui_image(const ImageBuffer& encoded) {
  const auto& m = encoded.metadata();
  if (m.pixel_format != PixelFormat::kUInt16 || m.channel_count != 3 ||
      m.cfa_pattern != CfaPattern::kNone || m.color_state != ColorState::kEncodedRgb ||
      m.rgb_color_space != RgbColorSpace::kSrgb ||
      m.transfer_function != TransferFunction::kSrgb ||
      m.numeric_range.nominal_min != 0 || m.numeric_range.nominal_max != 65535 ||
      m.numeric_range.allows_out_of_range) {
    throw std::invalid_argument("GUI requires encoded RGB16 sRGB");
  }
  if (m.width > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
      m.height > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("GUI image dimensions exceed Qt limits");
  }
  QImage result(static_cast<int>(m.width), static_cast<int>(m.height), QImage::Format_RGBX64);
  if (result.isNull()) throw std::bad_alloc();
  for (std::size_t y = 0; y < m.height; ++y) {
    const auto* source = encoded.uint16_row(y);
    auto* target = reinterpret_cast<QRgba64*>(result.scanLine(static_cast<int>(y)));
    for (std::size_t x = 0; x < m.width; ++x)
      target[x] = QRgba64::fromRgba64(source[3*x], source[3*x+1], source[3*x+2], 65535);
  }
  result.setColorSpace(QColorSpace::SRgb);
  return result;
}
}
