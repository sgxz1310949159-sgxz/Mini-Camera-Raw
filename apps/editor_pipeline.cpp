#include "editor_pipeline.h"
#include "mini_camera_raw/display_encode.h"
#include "mini_camera_raw/tone.h"
#include "mini_camera_raw/demosaic.h"
#include "mini_camera_raw/white_balance.h"
#include <stdexcept>
#include <algorithm>
#include <utility>
namespace mini_camera_raw::app {
ImageBuffer render_editor_linear(const ImageBuffer& camera,
    const CameraToWorkingMatrix& matrix, double ev) {
  return apply_exposure(transform_camera_rgb(camera, matrix), ev);
}
ImageBuffer render_editor_output(const ImageBuffer& camera,
    const CameraToWorkingMatrix& matrix, double ev) {
  return encode_srgb16(render_editor_linear(camera, matrix, ev));
}
namespace {
void validate_orientation(const ImageBuffer& encoded,int orientation) {
  const auto& m = encoded.metadata();
  if (orientation != 0 && orientation != 3 && orientation != 5 && orientation != 6)
    throw std::invalid_argument("unsupported RAW orientation");
  if (m.pixel_format != PixelFormat::kUInt16 || m.channel_count != 3 ||
      m.color_state != ColorState::kEncodedRgb || m.rgb_color_space != RgbColorSpace::kSrgb ||
      m.transfer_function != TransferFunction::kSrgb || m.numeric_range.nominal_min != 0 ||
      m.numeric_range.nominal_max != 65535 || m.numeric_range.allows_out_of_range)
    throw std::invalid_argument("orientation requires encoded RGB16 sRGB");
}
}
ImageBuffer orient_editor_output(const ImageBuffer& encoded, int orientation) {
  validate_orientation(encoded,orientation);
  auto m=encoded.metadata();
  const auto width=m.width, height=m.height;
  if (orientation == 5 || orientation == 6) std::swap(m.width,m.height);
  m.row_stride_elements=m.width*3;
  auto out=ImageBuffer::create(m);
  auto* target=out.uint16_data();
  for (std::size_t y=0;y<height;++y) {
    const auto* source=encoded.uint16_row(y);
    for (std::size_t x=0;x<width;++x) {
      std::size_t dx=x,dy=y;
      if (orientation==3) { dx=width-1-x; dy=height-1-y; }
      if (orientation==5) { dx=y; dy=width-1-x; }
      if (orientation==6) { dx=height-1-y; dy=x; }
      // Validated dimensions/layout bound these offsets; copy all RGB channels.
      const auto index=dy*m.row_stride_elements+3*dx;
      for (std::size_t c=0;c<3;++c) target[index+c]=source[3*x+c];
    }
  }
  return out;
}
ImageBuffer orient_editor_output(ImageBuffer&& encoded,int orientation) {
  validate_orientation(encoded,orientation);
  const auto& m=encoded.metadata();
  if(orientation==0 && m.row_stride_elements==m.width*3) return std::move(encoded);
  return orient_editor_output(static_cast<const ImageBuffer&>(encoded),orientation);
}
EditorPhoto prepare_editor_photo(const DecodedRaw& decoded) {
  const int orientation=decoded.sensor.orientation;
  if (orientation!=0 && orientation!=3 && orientation!=5 && orientation!=6)
    throw std::invalid_argument("unsupported RAW orientation");
  if (!decoded.sensor.camera_wb_tile) throw std::invalid_argument("missing camera white balance");
  const auto& crop=decoded.sensor.default_crop;
  const auto& input=decoded.image.metadata();
  if(crop && (!crop->width || !crop->height || crop->left>input.width || crop->top>input.height ||
      crop->width>input.width-crop->left || crop->height>input.height-crop->top))
    throw std::invalid_argument("default crop exceeds decoded image");
  auto matrix=camera_to_working_matrix(decoded.sensor);
  auto normalized=normalize(decoded.image,decoded.sensor.levels);
  auto balanced=apply_white_balance(normalized,
      normalize_camera_wb(*decoded.sensor.camera_wb_tile,normalized.metadata().cfa_pattern));
  auto camera=demosaic_bilinear(balanced);
  if(crop && (crop->left || crop->top || crop->width!=input.width || crop->height!=input.height)) {
    auto metadata=camera.metadata();
    metadata.width=crop->width; metadata.height=crop->height;
    metadata.row_stride_elements=crop->width*3;
    auto output=ImageBuffer::create(metadata);
    for(std::size_t y=0;y<crop->height;++y)
      std::copy_n(camera.float_row(y+crop->top)+crop->left*3,crop->width*3,output.float_row(y));
    camera=std::move(output);
  }
  return {std::move(camera),matrix,orientation};
}
EditorPhoto open_editor_photo(const std::string& path) {
  return prepare_editor_photo(decode_raw(path));
}
ImageBuffer render_editor_photo(const EditorPhoto& photo, double ev) {
  return orient_editor_output(render_editor_output(photo.camera,photo.matrix,ev),photo.orientation);
}

}
