#include "mini_camera_raw/image_buffer.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace mini_camera_raw {
namespace {

ImageMetadata raw_bayer_metadata(std::size_t width, std::size_t height,
                                 std::size_t row_stride_elements) {
  return ImageMetadata{width,
                       height,
                       1,
                       row_stride_elements,
                       PixelFormat::kUInt16,
                       CfaPattern::kRggb,
                       ColorState::kRawBayer,
                       NumericRange{0.0, 16383.0, false}};
}

ImageMetadata linear_working_rgb_metadata(
    std::size_t width, std::size_t height,
    std::size_t row_stride_elements) {
  return ImageMetadata{width,
                       height,
                       3,
                       row_stride_elements,
                       PixelFormat::kFloat32,
                       CfaPattern::kNone,
                       ColorState::kLinearWorkingRgb,
                       NumericRange{0.0, 1.0, true}};
}

TEST(ImageBufferTest, CreatesSinglePixelRawBayerBuffer) {
  const ImageMetadata metadata = raw_bayer_metadata(1, 1, 1);
  ImageBuffer image = ImageBuffer::from_uint16(metadata, {4095});

  EXPECT_EQ(1U, image.metadata().width);
  EXPECT_EQ(1U, image.metadata().height);
  EXPECT_EQ(1U, image.metadata().channel_count);
  EXPECT_EQ(1U, image.metadata().row_stride_elements);
  EXPECT_EQ(PixelFormat::kUInt16, image.metadata().pixel_format);
  EXPECT_EQ(CfaPattern::kRggb, image.metadata().cfa_pattern);
  EXPECT_EQ(ColorState::kRawBayer, image.metadata().color_state);
  EXPECT_DOUBLE_EQ(0.0, image.metadata().numeric_range.nominal_min);
  EXPECT_DOUBLE_EQ(16383.0, image.metadata().numeric_range.nominal_max);
  EXPECT_FALSE(image.metadata().numeric_range.allows_out_of_range);
  EXPECT_EQ(1U, image.element_count());
  EXPECT_EQ(sizeof(std::uint16_t), image.byte_size());
  EXPECT_EQ(4095, image.uint16_at(0, 0, 0));
}

TEST(ImageBufferTest, KeepsOddBayerRowsContiguousWithExplicitPadding) {
  const ImageMetadata metadata = raw_bayer_metadata(3, 2, 5);
  ImageBuffer image =
      ImageBuffer::from_uint16(metadata, {1, 2, 3, 90, 91, 4, 5, 6, 92, 93});

  EXPECT_EQ(10U, image.element_count());
  EXPECT_EQ(image.uint16_data(), image.uint16_row(0));
  EXPECT_EQ(image.uint16_data() + 5, image.uint16_row(1));
  EXPECT_EQ(6, image.uint16_at(1, 2, 0));
  EXPECT_EQ(90, image.uint16_data()[3]);
}

TEST(ImageBufferTest, CreatesOrdinaryTightlyPackedBayerBuffer) {
  const ImageMetadata metadata = raw_bayer_metadata(4, 4, 4);
  ImageBuffer image = ImageBuffer::create(metadata);

  EXPECT_EQ(16U, image.element_count());
  EXPECT_EQ(0, image.uint16_at(3, 3, 0));
}

TEST(ImageBufferTest, StoresLinearWorkingRgbWithoutClipping) {
  const ImageMetadata metadata = linear_working_rgb_metadata(2, 1, 6);
  ImageBuffer image =
      ImageBuffer::from_float(metadata, {-0.25F, 0.5F, 1.5F, 2.0F, 0.0F, 1.0F});

  EXPECT_EQ(PixelFormat::kFloat32, image.metadata().pixel_format);
  EXPECT_EQ(ColorState::kLinearWorkingRgb, image.metadata().color_state);
  EXPECT_TRUE(image.metadata().numeric_range.allows_out_of_range);
  EXPECT_FLOAT_EQ(-0.25F, image.float_at(0, 0, 0));
  EXPECT_FLOAT_EQ(1.5F, image.float_at(0, 0, 2));
  EXPECT_FLOAT_EQ(2.0F, image.float_at(0, 1, 0));
  EXPECT_EQ(6U * sizeof(float), image.byte_size());
}

TEST(ImageBufferTest, SupportsLinearFloatBayerState) {
  ImageMetadata metadata = raw_bayer_metadata(2, 2, 2);
  metadata.pixel_format = PixelFormat::kFloat32;
  metadata.color_state = ColorState::kLinearBayer;
  metadata.numeric_range = NumericRange{0.0, 1.0, true};

  ImageBuffer image = ImageBuffer::from_float(metadata, {0.0F, 0.5F, 1.0F, 1.5F});

  EXPECT_FLOAT_EQ(1.5F, image.float_at(1, 1, 0));
}

TEST(ImageBufferTest, SupportsLinearCameraRgbState) {
  ImageMetadata metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.color_state = ColorState::kLinearCameraRgb;

  ImageBuffer image = ImageBuffer::from_float(metadata, {0.1F, 0.2F, 0.3F});

  EXPECT_EQ(ColorState::kLinearCameraRgb, image.metadata().color_state);
  EXPECT_FLOAT_EQ(0.2F, image.float_at(0, 0, 1));
}

TEST(ImageBufferTest, ProvidesConstAndMutableTypedAccess) {
  ImageBuffer image = ImageBuffer::create(linear_working_rgb_metadata(1, 2, 3));
  image.float_at(1, 0, 2) = 0.75F;
  image.float_row(0)[1] = 0.25F;

  const ImageBuffer& const_image = image;
  EXPECT_FLOAT_EQ(0.75F, const_image.float_at(1, 0, 2));
  EXPECT_FLOAT_EQ(0.25F, const_image.float_row(0)[1]);
  EXPECT_NE(nullptr, const_image.float_data());
}

TEST(ImageBufferTest, RejectsZeroDimensionsAndChannels) {
  EXPECT_THROW(ImageBuffer::create(raw_bayer_metadata(0, 1, 1)),
               std::invalid_argument);
  EXPECT_THROW(ImageBuffer::create(raw_bayer_metadata(1, 0, 1)),
               std::invalid_argument);

  ImageMetadata metadata = raw_bayer_metadata(1, 1, 1);
  metadata.channel_count = 0;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);
}

TEST(ImageBufferTest, RejectsStrideShorterThanPackedRow) {
  EXPECT_THROW(ImageBuffer::create(linear_working_rgb_metadata(2, 1, 5)),
               std::invalid_argument);
}

TEST(ImageBufferTest, RejectsStorageSizeMismatch) {
  const ImageMetadata metadata = raw_bayer_metadata(2, 2, 2);

  EXPECT_THROW(ImageBuffer::from_uint16(metadata, {1, 2, 3}),
               std::invalid_argument);
  EXPECT_THROW(ImageBuffer::from_uint16(metadata, {1, 2, 3, 4, 5}),
               std::invalid_argument);
}

TEST(ImageBufferTest, RejectsStorageTypeMismatch) {
  const ImageMetadata raw_metadata = raw_bayer_metadata(1, 1, 1);
  const ImageMetadata float_metadata = linear_working_rgb_metadata(1, 1, 3);

  EXPECT_THROW(ImageBuffer::from_float(raw_metadata, {0.0F}),
               std::invalid_argument);
  EXPECT_THROW(ImageBuffer::from_uint16(float_metadata, {0, 0, 0}),
               std::invalid_argument);
}

TEST(ImageBufferTest, RejectsInvalidBayerAndRgbStateCombinations) {
  ImageMetadata metadata = raw_bayer_metadata(1, 1, 1);
  metadata.cfa_pattern = CfaPattern::kNone;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = raw_bayer_metadata(1, 1, 1);
  metadata.channel_count = 3;
  metadata.row_stride_elements = 3;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.cfa_pattern = CfaPattern::kBggr;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.channel_count = 1;
  metadata.row_stride_elements = 1;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);
}

TEST(ImageBufferTest, RejectsInvalidNumericRangeDeclarations) {
  ImageMetadata metadata = raw_bayer_metadata(1, 1, 1);
  metadata.numeric_range.nominal_max = 70000.0;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = raw_bayer_metadata(1, 1, 1);
  metadata.numeric_range.allows_out_of_range = true;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.numeric_range.nominal_min = -1.0;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.numeric_range.allows_out_of_range = false;
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = raw_bayer_metadata(1, 1, 1);
  metadata.numeric_range.nominal_max =
      std::numeric_limits<double>::quiet_NaN();
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = linear_working_rgb_metadata(1, 1, 3);
  metadata.numeric_range.nominal_max =
      std::numeric_limits<double>::infinity();
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);
}

TEST(ImageBufferTest, RejectsUnknownEnumValues) {
  ImageMetadata metadata = raw_bayer_metadata(1, 1, 1);
  metadata.pixel_format = static_cast<PixelFormat>(999);
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = raw_bayer_metadata(1, 1, 1);
  metadata.cfa_pattern = static_cast<CfaPattern>(999);
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);

  metadata = raw_bayer_metadata(1, 1, 1);
  metadata.color_state = static_cast<ColorState>(999);
  EXPECT_THROW(ImageBuffer::create(metadata), std::invalid_argument);
}

TEST(ImageBufferTest, DetectsPackedRowElementCountAndByteCountOverflow) {
  const std::size_t maximum = std::numeric_limits<std::size_t>::max();

  ImageMetadata metadata = linear_working_rgb_metadata(maximum, 1, maximum);
  EXPECT_THROW(ImageBuffer::from_float(metadata, {}), std::length_error);

  metadata = raw_bayer_metadata(1, 2, maximum);
  EXPECT_THROW(ImageBuffer::from_uint16(metadata, {}), std::length_error);

  metadata = raw_bayer_metadata(maximum / sizeof(std::uint16_t) + 1, 1,
                                maximum / sizeof(std::uint16_t) + 1);
  EXPECT_THROW(ImageBuffer::from_uint16(metadata, {}), std::length_error);
}

TEST(ImageBufferTest, RejectsOutOfBoundsAndWrongTypedAccess) {
  ImageBuffer image = ImageBuffer::create(linear_working_rgb_metadata(2, 2, 6));

  EXPECT_THROW(image.float_at(2, 0, 0), std::out_of_range);
  EXPECT_THROW(image.float_at(0, 2, 0), std::out_of_range);
  EXPECT_THROW(image.float_at(0, 0, 3), std::out_of_range);
  EXPECT_THROW(image.float_row(2), std::out_of_range);
  EXPECT_THROW(image.uint16_at(0, 0, 0), std::logic_error);
  EXPECT_THROW(image.uint16_row(0), std::logic_error);
  EXPECT_THROW(static_cast<void>(image.uint16_data()), std::logic_error);

  ImageBuffer raw_image = ImageBuffer::create(raw_bayer_metadata(1, 1, 1));
  EXPECT_THROW(static_cast<void>(raw_image.float_data()), std::logic_error);
}

TEST(ImageBufferTest, CopiesStorageIndependently) {
  ImageBuffer original =
      ImageBuffer::from_uint16(raw_bayer_metadata(2, 1, 2), {7, 8});
  ImageBuffer copy = original;

  copy.uint16_at(0, 0, 0) = 99;

  EXPECT_EQ(7, original.uint16_at(0, 0, 0));
  EXPECT_EQ(99, copy.uint16_at(0, 0, 0));
  EXPECT_NE(original.uint16_data(), copy.uint16_data());
}

TEST(ImageBufferTest, MovesOwnedStorageToDestination) {
  ImageBuffer source =
      ImageBuffer::from_float(linear_working_rgb_metadata(1, 1, 3),
                              {0.25F, 0.5F, 0.75F});
  const float* const source_data = source.float_data();

  ImageBuffer destination = std::move(source);

  EXPECT_EQ(source_data, destination.float_data());
  EXPECT_FLOAT_EQ(0.75F, destination.float_at(0, 0, 2));
}

}  // namespace
}  // namespace mini_camera_raw

namespace mini_camera_raw {
TEST(P5MetadataTest, PreservesLegacyUnknownAndChecksColorTransferPairs) {
  ImageMetadata m{1,
                  1,
                  3,
                  3,
                  PixelFormat::kFloat32,
                  CfaPattern::kNone,
                  ColorState::kLinearWorkingRgb,
                  {0, 1, true}};
  EXPECT_EQ(ImageBuffer::create(m).metadata().rgb_color_space,
            RgbColorSpace::kUnspecified);
  m.rgb_color_space = RgbColorSpace::kSrgb;
  EXPECT_THROW(ImageBuffer::create(m), std::invalid_argument);
  m.transfer_function = TransferFunction::kLinear;
  EXPECT_NO_THROW(ImageBuffer::create(m));
  m.transfer_function = TransferFunction::kSrgb;
  EXPECT_THROW(ImageBuffer::create(m), std::invalid_argument);
  m.transfer_function = TransferFunction::kLinear;
  m.color_state = ColorState::kLinearCameraRgb;
  EXPECT_THROW(ImageBuffer::create(m), std::invalid_argument);
  m.color_state = ColorState::kLinearWorkingRgb;
  m.rgb_color_space = static_cast<RgbColorSpace>(999);
  EXPECT_THROW(ImageBuffer::create(m), std::invalid_argument);
  m.rgb_color_space = RgbColorSpace::kSrgb;
  m.transfer_function = static_cast<TransferFunction>(999);
  EXPECT_THROW(ImageBuffer::create(m), std::invalid_argument);
}
TEST(P5MetadataTest, EncodedRgbRequiresExactIntegerAndColorContract) {
  ImageMetadata m{1,
                  1,
                  3,
                  3,
                  PixelFormat::kUInt16,
                  CfaPattern::kNone,
                  ColorState::kEncodedRgb,
                  {0, 65535, false},
                  RgbColorSpace::kSrgb,
                  TransferFunction::kSrgb};
  EXPECT_NO_THROW(ImageBuffer::from_uint16(m, {0, 32768, 65535}));
  auto bad = m;
  bad.numeric_range.allows_out_of_range = true;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
  bad = m;
  bad.numeric_range.nominal_max = 1;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
  bad = m;
  bad.rgb_color_space = RgbColorSpace::kUnspecified;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
  bad = m;
  bad.pixel_format = PixelFormat::kFloat32;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
  bad = m;
  bad.cfa_pattern = CfaPattern::kRggb;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
  bad = m;
  bad.channel_count = 1;
  EXPECT_THROW(ImageBuffer::create(bad), std::invalid_argument);
}
}  // namespace mini_camera_raw
