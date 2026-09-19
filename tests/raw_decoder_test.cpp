#include "mini_camera_raw/raw_decoder.h"
#include "raw_decoder_internal.h"

#include <libraw/libraw.h>
#include <gtest/gtest.h>

#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>

namespace mini_camera_raw {
namespace {
// Generated sensor numbers, no camera file contents.
TEST(RawLayoutTest, CopiesOddActiveAreaAndOwnsStorage) {
  SensorGeometry g{6, 5, 1, 1, 3, 3, 16};
  std::vector<std::uint16_t> samples(40);
  for (unsigned i = 0; i < 40; ++i) samples[i] = i;
  auto output = detail::copy_active_mosaic(samples.data(), samples.size(), g, CfaPattern::kBggr);
  EXPECT_EQ(3U, output.metadata().row_stride_elements);
  EXPECT_EQ(CfaPattern::kBggr, output.metadata().cfa_pattern);
  EXPECT_EQ(9, output.uint16_at(0, 0, 0));
  EXPECT_EQ(27, output.uint16_at(2, 2, 0));
  samples[9] = 0;
  EXPECT_EQ(9, output.uint16_at(0, 0, 0));
}

TEST(RawLayoutTest, RejectsInvalidGeometryAndStorageBeforeReading) {
  const std::uint16_t sample = 0;
  const SensorGeometry valid{1, 1, 0, 0, 1, 1, 2};
  EXPECT_THROW(detail::copy_active_mosaic(nullptr, 1, valid, CfaPattern::kRggb), std::invalid_argument);
  EXPECT_THROW(detail::copy_active_mosaic(&sample, 0, valid, CfaPattern::kRggb), std::invalid_argument);
  for (const auto& g : {SensorGeometry{0,1,0,0,1,1,2}, {1,0,0,0,1,1,2},
                        {1,1,0,0,0,1,2}, {1,1,0,0,1,0,2}, {1,1,2,0,1,1,2},
                        {1,1,0,2,1,1,2}, {2,1,1,0,2,1,4}, {1,2,0,1,1,2,2},
                        {1,1,0,0,1,1,1}, {2,1,0,0,1,1,2}, {1,1,0,0,1,1,0}}) {
    EXPECT_THROW(detail::copy_active_mosaic(&sample, 1, g, CfaPattern::kRggb), std::invalid_argument);
  }
}

TEST(RawLayoutTest, ChecksMultiplicationOverflowBeforeReading) {
  const std::uint16_t sample = 0;
  const auto max = std::numeric_limits<std::size_t>::max();
  for (const auto& g : {SensorGeometry{max,1,0,0,1,1,2}, {1,max,0,0,1,1,4}}) {
    EXPECT_THROW(detail::copy_active_mosaic(&sample, max, g, CfaPattern::kRggb), std::length_error);
  }
}

class RawDecoderTest : public ::testing::Test {
 protected:
  LibRaw decoder;
  std::vector<std::uint16_t> pixels = std::vector<std::uint16_t>(40, 512);
  void SetUp() override {
    auto& d = decoder.imgdata;
    std::strcpy(d.idata.make, "Sony");
    std::strcpy(d.idata.model, "ILCE-7CM2");
    std::strcpy(d.idata.cdesc, "RGBG");
    d.idata.raw_count = 1;
    d.idata.colors = 3;
    set_tile({0, 1, 3, 2});
    d.sizes.raw_width = 6; d.sizes.raw_height = 5;
    d.sizes.width = 3; d.sizes.height = 3;
    d.sizes.left_margin = 1; d.sizes.top_margin = 1;
    d.sizes.raw_pitch = 16; d.sizes.pixel_aspect = 1;
    d.sizes.flip = 6;
    d.rawdata.raw_image = pixels.data();
    d.color.black = 100;
    d.color.maximum = 1100;
    d.color.raw_bps = 14;
    for (int i = 0; i < 4; ++i) {
      d.color.cblack[i] = 10 * i;
      d.color.cam_mul[i] = i + 1;
    }
    d.color.rgb_cam[0][0] = 1;
    d.color.rgb_cam[1][1] = 1;
    d.color.rgb_cam[2][2] = 1;
  }
  void TearDown() override { decoder.imgdata.rawdata.raw_image = nullptr; }
  void set_tile(std::array<unsigned, 4> tile) {
    unsigned filters = 0;
    for (unsigned y = 0; y < 8; ++y)
      for (unsigned x = 0; x < 2; ++x)
        filters |= tile[2 * (y % 2) + x] << (2 * (2 * y + x));
    decoder.imgdata.idata.filters = filters;
  }
};

TEST_F(RawDecoderTest, AllCfaUseVisiblePhaseWithoutDoubleMarginShift) {
  const std::array<std::array<unsigned, 4>, 4> tiles{{{0,1,3,2}, {2,3,1,0}, {1,0,2,3}, {3,2,0,1}}};
  const std::array<CfaPattern, 4> patterns{CfaPattern::kRggb,CfaPattern::kBggr,CfaPattern::kGrbg,CfaPattern::kGbrg};
  for (std::size_t i = 0; i < 4; ++i) {
    set_tile(tiles[i]);
    auto output = detail::copy_libraw_result(decoder);
    EXPECT_EQ(patterns[i], output.image.metadata().cfa_pattern);
    for (std::size_t p = 0; p < 4; ++p) {
      EXPECT_DOUBLE_EQ(100 + 10 * tiles[i][p], output.sensor.levels.black[p]);
      EXPECT_DOUBLE_EQ(1100, output.sensor.levels.white[p]);
      ASSERT_TRUE(output.sensor.camera_wb_tile);
      EXPECT_DOUBLE_EQ(tiles[i][p] + 1, (*output.sensor.camera_wb_tile)[p]);
    }
    EXPECT_EQ(14U, output.sensor.source_bits);
    EXPECT_EQ(6, output.sensor.orientation);
    EXPECT_EQ(1U, output.sensor.geometry.active_left);
    ASSERT_TRUE(output.sensor.camera_to_srgb);
    EXPECT_DOUBLE_EQ(1, (*output.sensor.camera_to_srgb)[10]);
    EXPECT_EQ(512, output.image.uint16_at(0, 0, 0));
    EXPECT_EQ(LibRaw::version(), output.sensor.libraw_version);
  }
}

TEST_F(RawDecoderTest, MissingOptionalMetadataStaysAbsent) {
  std::memset(decoder.imgdata.color.cam_mul, 0, sizeof(decoder.imgdata.color.cam_mul));
  std::memset(decoder.imgdata.color.rgb_cam, 0, sizeof(decoder.imgdata.color.rgb_cam));
  decoder.imgdata.color.raw_bps = 0;
  const auto output = detail::copy_libraw_result(decoder);
  EXPECT_FALSE(output.sensor.camera_wb_tile);
  EXPECT_FALSE(output.sensor.camera_to_srgb);
  EXPECT_EQ(0U, output.sensor.source_bits);
}

TEST_F(RawDecoderTest, RejectsUnsupportedCameraAndLayouts) {
  std::strcpy(decoder.imgdata.idata.model, "other");
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  std::strcpy(decoder.imgdata.idata.model, "ILCE-7CM2");
  for (unsigned filters : {0U, 9U, 1U, 0xffffffffU}) {
    decoder.imgdata.idata.filters = filters;
    EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  }
  set_tile({0,1,3,2});
  decoder.imgdata.idata.filters ^= 1U << 12;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
}

TEST_F(RawDecoderTest, RejectsSpatialBlackAndInvalidLevels) {
  decoder.imgdata.color.cblack[4] = 2;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  decoder.imgdata.color.cblack[4] = 0;
  decoder.imgdata.color.maximum = 100;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::invalid_argument);
  decoder.imgdata.color.maximum = 65536;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::invalid_argument);
}

TEST_F(RawDecoderTest, GreenIndexAliasesRequireEqualCalibration) {
  // Last odd row labels its first green as index 1 instead of index 3.
  decoder.imgdata.idata.filters &= ~(2U << 28);
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  decoder.imgdata.color.cblack[3] = decoder.imgdata.color.cblack[1];
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  decoder.imgdata.color.cam_mul[3] = decoder.imgdata.color.cam_mul[1];
  EXPECT_EQ(CfaPattern::kRggb,
            detail::copy_libraw_result(decoder).image.metadata().cfa_pattern);
}

TEST_F(RawDecoderTest, RejectsNonfiniteOptionalMetadata) {
  decoder.imgdata.color.cam_mul[0] = std::numeric_limits<float>::quiet_NaN();
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::invalid_argument);
  decoder.imgdata.color.cam_mul[0] = 1;
  decoder.imgdata.color.rgb_cam[0][0] = std::numeric_limits<float>::infinity();
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::invalid_argument);
}

TEST_F(RawDecoderTest, RejectsUnsupportedStorageAndFrameMetadata) {
  auto& d = decoder.imgdata;
  d.rawdata.raw_image = nullptr;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  d.rawdata.raw_image = pixels.data();
  float fp = 0;
  d.rawdata.float_image = &fp;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  d.rawdata.float_image = nullptr;
  d.idata.dng_version = 1;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  d.idata.dng_version = 0;
  d.idata.raw_count = 2;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  d.idata.raw_count = 1;
  d.sizes.pixel_aspect = 2;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
  d.sizes.pixel_aspect = 1;
  d.color.raw_bps = 32;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
}

TEST_F(RawDecoderTest, RejectsNonRgbgMatrixBasis) {
  std::strcpy(decoder.imgdata.idata.cdesc, "BGRG");
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::runtime_error);
}

TEST_F(RawDecoderTest, BlackAdditionCannotWrapAndWhiteIsNotSceneMaximum) {
  decoder.imgdata.color.data_maximum = 512;
  auto result = detail::copy_libraw_result(decoder);
  EXPECT_DOUBLE_EQ(1100, result.sensor.levels.white[0]);
  decoder.imgdata.color.black = std::numeric_limits<unsigned>::max();
  decoder.imgdata.color.cblack[0] = 101;
  EXPECT_THROW(detail::copy_libraw_result(decoder), std::invalid_argument);
}

TEST(RawLayoutTest, GeneratedCropsPreserveSamplesAcrossPitchChanges) {
  for (std::size_t top = 0; top < 2; ++top)
    for (std::size_t left = 0; left < 2; ++left)
      for (std::size_t width : {1U, 3U, 6U}) {
        const auto raw_width = width + 2;
        const auto stride = raw_width + 3;
        std::vector<std::uint16_t> samples(stride * 5, 65535);
        for (std::size_t y = 0; y < 5; ++y)
          for (std::size_t x = 0; x < raw_width; ++x) samples[y * stride + x] = 100 * y + x;
        SensorGeometry g{raw_width, 5, left, top, width, 3, stride * 2};
        auto out = detail::copy_active_mosaic(samples.data(), samples.size(), g, CfaPattern::kRggb);
        for (std::size_t y = 0; y < 3; ++y)
          for (std::size_t x = 0; x < width; ++x)
            EXPECT_EQ(100 * (y + top) + x + left, out.uint16_at(y, x, 0));
      }
}

TEST(RawFileTest, LibRawSyntheticUnpackResultSurvivesDecoderDestruction) {
  // Exercises real LibRaw unpack, not a Sony file decoder. Synthetic camera
  // identity is overridden only to exercise our narrow adapter afterward.
  auto result = [] {
    LibRaw decoder;
    std::vector<unsigned char> bytes(8 * 8 * 2);
    for (unsigned p = 0; p < 64; ++p) {
      const unsigned sample = 256 + p;
      bytes[2 * p] = sample & 255;
      bytes[2 * p + 1] = sample >> 8;
    }
    if (decoder.open_bayer(bytes.data(), static_cast<unsigned>(bytes.size()),
                           8, 8, 0, 0, 0, 0, 0, LIBRAW_OPENBAYER_RGGB,
                           0, 0, 0) != LIBRAW_SUCCESS ||
        decoder.unpack() != LIBRAW_SUCCESS)
      throw std::runtime_error("synthetic LibRaw unpack failed");
    std::strcpy(decoder.imgdata.idata.make, "Sony");
    std::strcpy(decoder.imgdata.idata.model, "ILCE-7CM2");
    return detail::copy_libraw_result(decoder);
  }();
  EXPECT_EQ(256, result.image.uint16_at(0, 0, 0));
  EXPECT_EQ(319, result.image.uint16_at(7, 7, 0));
  EXPECT_EQ(CfaPattern::kRggb, result.image.metadata().cfa_pattern);
  const auto linear = normalize(result.image, result.sensor.levels);
  EXPECT_NEAR(319.0 / 65535, linear.float_at(7, 7, 0), 1e-6);
}

TEST(RawFileTest, RejectsEmptyEmbeddedNullAndMissingPath) {
  EXPECT_THROW(static_cast<void>(decode_raw("")), std::invalid_argument);
  EXPECT_THROW(static_cast<void>(decode_raw(std::string("a\0b", 3))), std::invalid_argument);
  EXPECT_THROW(static_cast<void>(decode_raw("/nonexistent-mini-camera-raw-p3/input.ARW")), std::runtime_error);
}
}  // namespace
}  // namespace mini_camera_raw
