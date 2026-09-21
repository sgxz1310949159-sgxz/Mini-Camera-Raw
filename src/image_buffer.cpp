#include "mini_camera_raw/image_buffer.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace mini_camera_raw {
namespace {

struct ValidatedLayout {
  std::size_t element_count;
};

std::size_t checked_multiply(std::size_t left, std::size_t right,
                             const char* description) {
  if (left != 0 && right > std::numeric_limits<std::size_t>::max() / left) {
    throw std::length_error(std::string{description} + " overflows size_t");
  }
  return left * right;
}

bool is_known(PixelFormat format) {
  switch (format) {
    case PixelFormat::kUInt16:
    case PixelFormat::kFloat32:
      return true;
  }
  return false;
}

bool is_known(CfaPattern pattern) {
  switch (pattern) {
    case CfaPattern::kNone:
    case CfaPattern::kRggb:
    case CfaPattern::kBggr:
    case CfaPattern::kGrbg:
    case CfaPattern::kGbrg:
      return true;
  }
  return false;
}

bool is_known(ColorState state) {
  switch (state) {
    case ColorState::kRawBayer:
    case ColorState::kLinearBayer:
    case ColorState::kLinearCameraRgb:
    case ColorState::kLinearWorkingRgb:
    case ColorState::kEncodedRgb:
      return true;
  }
  return false;
}

bool has_concrete_cfa(CfaPattern pattern) {
  return pattern != CfaPattern::kNone;
}

void require_linear_range(const NumericRange& range) {
  if (range.nominal_min != 0.0 || range.nominal_max != 1.0 ||
      !range.allows_out_of_range) {
    throw std::invalid_argument(
        "linear image states require nominal [0, 1] with out-of-range values "
        "allowed");
  }
}

void validate_color_identity(const ImageMetadata& m) {
  if (m.rgb_color_space != RgbColorSpace::kUnspecified &&
      m.rgb_color_space != RgbColorSpace::kSrgb)
    throw std::invalid_argument("unknown RGB color space");
  if (m.transfer_function != TransferFunction::kUnspecified &&
      m.transfer_function != TransferFunction::kLinear &&
      m.transfer_function != TransferFunction::kSrgb)
    throw std::invalid_argument("unknown transfer function");
  if (m.color_state == ColorState::kEncodedRgb) {
    if (m.rgb_color_space != RgbColorSpace::kSrgb ||
        m.transfer_function != TransferFunction::kSrgb)
      throw std::invalid_argument(
          "encoded RGB requires supported space/transfer");
  } else if (m.color_state == ColorState::kLinearWorkingRgb &&
             m.rgb_color_space == RgbColorSpace::kSrgb) {
    if (m.transfer_function != TransferFunction::kLinear)
      throw std::invalid_argument("working sRGB requires linear transfer");
  } else if (m.rgb_color_space != RgbColorSpace::kUnspecified ||
             m.transfer_function != TransferFunction::kUnspecified) {
    throw std::invalid_argument(
        "incompatible color identity and processing state");
  }
}

void validate_state_combination(const ImageMetadata& metadata) {
  validate_color_identity(metadata);
  switch (metadata.color_state) {
    case ColorState::kRawBayer:
      if (metadata.pixel_format != PixelFormat::kUInt16 ||
          metadata.channel_count != 1 ||
          !has_concrete_cfa(metadata.cfa_pattern)) {
        throw std::invalid_argument(
            "raw Bayer requires uint16, one channel, and a concrete CFA");
      }
      if (metadata.numeric_range.nominal_min != 0.0 ||
          metadata.numeric_range.nominal_max >
              static_cast<double>(std::numeric_limits<std::uint16_t>::max()) ||
          metadata.numeric_range.allows_out_of_range) {
        throw std::invalid_argument(
            "raw Bayer requires an in-range uint16 code-value interval");
      }
      return;
    case ColorState::kLinearBayer:
      if (metadata.pixel_format != PixelFormat::kFloat32 ||
          metadata.channel_count != 1 ||
          !has_concrete_cfa(metadata.cfa_pattern)) {
        throw std::invalid_argument(
            "linear Bayer requires float32, one channel, and a concrete CFA");
      }
      require_linear_range(metadata.numeric_range);
      return;
    case ColorState::kEncodedRgb:
      if (metadata.pixel_format != PixelFormat::kUInt16 ||
          metadata.channel_count != 3 ||
          metadata.cfa_pattern != CfaPattern::kNone ||
          metadata.numeric_range.nominal_min != 0 ||
          metadata.numeric_range.nominal_max != 65535 ||
          metadata.numeric_range.allows_out_of_range)
        throw std::invalid_argument(
            "encoded RGB requires uint16 RGB [0,65535]");
      return;
    case ColorState::kLinearCameraRgb:
    case ColorState::kLinearWorkingRgb:
      if (metadata.pixel_format != PixelFormat::kFloat32 ||
          metadata.channel_count != 3 ||
          metadata.cfa_pattern != CfaPattern::kNone) {
        throw std::invalid_argument(
            "linear RGB requires float32, three channels, and no CFA");
      }
      require_linear_range(metadata.numeric_range);
      return;
  }
  throw std::invalid_argument("unknown color state");
}

ValidatedLayout validate_metadata(const ImageMetadata& metadata) {
  if (!is_known(metadata.pixel_format) || !is_known(metadata.cfa_pattern) ||
      !is_known(metadata.color_state)) {
    throw std::invalid_argument("image metadata contains an unknown enum value");
  }
  if (metadata.width == 0 || metadata.height == 0 ||
      metadata.channel_count == 0) {
    throw std::invalid_argument(
        "image width, height, and channel count must be non-zero");
  }

  const std::size_t packed_row = checked_multiply(
      metadata.width, metadata.channel_count, "packed row element count");
  if (metadata.row_stride_elements < packed_row) {
    throw std::invalid_argument(
        "row stride must be at least width times channel count");
  }

  const std::size_t element_count =
      checked_multiply(metadata.row_stride_elements, metadata.height,
                       "image element count");
  const std::size_t bytes_per_element =
      metadata.pixel_format == PixelFormat::kUInt16 ? sizeof(std::uint16_t)
                                                    : sizeof(float);
  static_cast<void>(checked_multiply(element_count, bytes_per_element,
                                     "image storage byte count"));

  if (!std::isfinite(metadata.numeric_range.nominal_min) ||
      !std::isfinite(metadata.numeric_range.nominal_max) ||
      metadata.numeric_range.nominal_min >=
          metadata.numeric_range.nominal_max) {
    throw std::invalid_argument(
        "numeric range bounds must be finite and strictly increasing");
  }

  validate_state_combination(metadata);
  return ValidatedLayout{element_count};
}

std::size_t element_index(const ImageMetadata& metadata, std::size_t row,
                          std::size_t column, std::size_t channel) {
  if (row >= metadata.height || column >= metadata.width ||
      channel >= metadata.channel_count) {
    throw std::out_of_range("image pixel coordinate is out of range");
  }
  return row * metadata.row_stride_elements +
         column * metadata.channel_count + channel;
}

void validate_row(const ImageMetadata& metadata, std::size_t row) {
  if (row >= metadata.height) {
    throw std::out_of_range("image row is out of range");
  }
}

}  // namespace

ImageBuffer ImageBuffer::create(ImageMetadata metadata) {
  const ValidatedLayout layout = validate_metadata(metadata);
  if (metadata.pixel_format == PixelFormat::kUInt16) {
    return ImageBuffer{std::move(metadata), UInt16Storage(layout.element_count)};
  }
  return ImageBuffer{std::move(metadata), FloatStorage(layout.element_count)};
}

ImageBuffer ImageBuffer::from_uint16(ImageMetadata metadata,
                                     UInt16Storage storage) {
  const ValidatedLayout layout = validate_metadata(metadata);
  if (metadata.pixel_format != PixelFormat::kUInt16) {
    throw std::invalid_argument("uint16 storage requires uint16 pixel format");
  }
  if (storage.size() != layout.element_count) {
    throw std::invalid_argument("uint16 storage size does not match metadata");
  }
  return ImageBuffer{std::move(metadata), std::move(storage)};
}

ImageBuffer ImageBuffer::from_float(ImageMetadata metadata,
                                    FloatStorage storage) {
  const ValidatedLayout layout = validate_metadata(metadata);
  if (metadata.pixel_format != PixelFormat::kFloat32) {
    throw std::invalid_argument("float storage requires float32 pixel format");
  }
  if (storage.size() != layout.element_count) {
    throw std::invalid_argument("float storage size does not match metadata");
  }
  return ImageBuffer{std::move(metadata), std::move(storage)};
}

ImageBuffer::ImageBuffer(ImageMetadata metadata, Storage storage)
    : metadata_(std::move(metadata)), storage_(std::move(storage)) {}

const ImageMetadata& ImageBuffer::metadata() const noexcept { return metadata_; }

std::size_t ImageBuffer::element_count() const noexcept {
  return metadata_.row_stride_elements * metadata_.height;
}

std::size_t ImageBuffer::byte_size() const noexcept {
  const std::size_t bytes_per_element =
      metadata_.pixel_format == PixelFormat::kUInt16 ? sizeof(std::uint16_t)
                                                    : sizeof(float);
  return element_count() * bytes_per_element;
}

std::uint16_t* ImageBuffer::uint16_data() {
  if (!std::holds_alternative<UInt16Storage>(storage_)) {
    throw std::logic_error("image does not contain uint16 storage");
  }
  return std::get<UInt16Storage>(storage_).data();
}

const std::uint16_t* ImageBuffer::uint16_data() const {
  if (!std::holds_alternative<UInt16Storage>(storage_)) {
    throw std::logic_error("image does not contain uint16 storage");
  }
  return std::get<UInt16Storage>(storage_).data();
}

float* ImageBuffer::float_data() {
  if (!std::holds_alternative<FloatStorage>(storage_)) {
    throw std::logic_error("image does not contain float storage");
  }
  return std::get<FloatStorage>(storage_).data();
}

const float* ImageBuffer::float_data() const {
  if (!std::holds_alternative<FloatStorage>(storage_)) {
    throw std::logic_error("image does not contain float storage");
  }
  return std::get<FloatStorage>(storage_).data();
}

std::uint16_t* ImageBuffer::uint16_row(std::size_t row) {
  validate_row(metadata_, row);
  static_cast<void>(uint16_data());
  auto& storage = std::get<UInt16Storage>(storage_);
  return &storage.at(row * metadata_.row_stride_elements);
}

const std::uint16_t* ImageBuffer::uint16_row(std::size_t row) const {
  validate_row(metadata_, row);
  static_cast<void>(uint16_data());
  const auto& storage = std::get<UInt16Storage>(storage_);
  return &storage.at(row * metadata_.row_stride_elements);
}

float* ImageBuffer::float_row(std::size_t row) {
  validate_row(metadata_, row);
  static_cast<void>(float_data());
  auto& storage = std::get<FloatStorage>(storage_);
  return &storage.at(row * metadata_.row_stride_elements);
}

const float* ImageBuffer::float_row(std::size_t row) const {
  validate_row(metadata_, row);
  static_cast<void>(float_data());
  const auto& storage = std::get<FloatStorage>(storage_);
  return &storage.at(row * metadata_.row_stride_elements);
}

std::uint16_t& ImageBuffer::uint16_at(std::size_t row, std::size_t column,
                                      std::size_t channel) {
  static_cast<void>(uint16_data());
  return std::get<UInt16Storage>(storage_).at(
      element_index(metadata_, row, column, channel));
}

const std::uint16_t& ImageBuffer::uint16_at(std::size_t row,
                                            std::size_t column,
                                            std::size_t channel) const {
  static_cast<void>(uint16_data());
  return std::get<UInt16Storage>(storage_).at(
      element_index(metadata_, row, column, channel));
}

float& ImageBuffer::float_at(std::size_t row, std::size_t column,
                             std::size_t channel) {
  static_cast<void>(float_data());
  return std::get<FloatStorage>(storage_).at(
      element_index(metadata_, row, column, channel));
}

const float& ImageBuffer::float_at(std::size_t row, std::size_t column,
                                   std::size_t channel) const {
  static_cast<void>(float_data());
  return std::get<FloatStorage>(storage_).at(
      element_index(metadata_, row, column, channel));
}

}  // namespace mini_camera_raw
