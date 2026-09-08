#ifndef MINI_CAMERA_RAW_IMAGE_BUFFER_H
#define MINI_CAMERA_RAW_IMAGE_BUFFER_H

#include "mini_camera_raw/image_metadata.h"

#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

namespace mini_camera_raw {

class ImageBuffer {
 public:
  using UInt16Storage = std::vector<std::uint16_t>;
  using FloatStorage = std::vector<float>;

  // Allocates zero-initialized storage after validating the complete contract.
  static ImageBuffer create(ImageMetadata metadata);
  // Takes ownership of caller-provided storage after validating its type/size.
  static ImageBuffer from_uint16(ImageMetadata metadata,
                                 UInt16Storage storage);
  static ImageBuffer from_float(ImageMetadata metadata, FloatStorage storage);

  [[nodiscard]] const ImageMetadata& metadata() const noexcept;
  [[nodiscard]] std::size_t element_count() const noexcept;
  [[nodiscard]] std::size_t byte_size() const noexcept;

  // Typed pointers permit sample mutation without exposing vector resizing.
  std::uint16_t* uint16_data();
  const std::uint16_t* uint16_data() const;
  float* float_data();
  const float* float_data() const;

  // Row pointers include any declared row-end padding in their spacing.
  std::uint16_t* uint16_row(std::size_t row);
  const std::uint16_t* uint16_row(std::size_t row) const;
  float* float_row(std::size_t row);
  const float* float_row(std::size_t row) const;

  std::uint16_t& uint16_at(std::size_t row, std::size_t column,
                           std::size_t channel);
  const std::uint16_t& uint16_at(std::size_t row, std::size_t column,
                                 std::size_t channel) const;
  float& float_at(std::size_t row, std::size_t column, std::size_t channel);
  const float& float_at(std::size_t row, std::size_t column,
                        std::size_t channel) const;

 private:
  using Storage = std::variant<UInt16Storage, FloatStorage>;

  ImageBuffer(ImageMetadata metadata, Storage storage);

  ImageMetadata metadata_;
  Storage storage_;
};

}  // namespace mini_camera_raw

#endif
