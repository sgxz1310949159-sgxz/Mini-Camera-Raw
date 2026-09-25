#ifndef MINI_CAMERA_RAW_PNG_TEST_SUPPORT_H
#define MINI_CAMERA_RAW_PNG_TEST_SUPPORT_H
#include <gtest/gtest.h>
#include <zlib.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

#include "mini_camera_raw/image_buffer.h"
namespace mini_camera_raw::test_support {
inline std::vector<unsigned char> read(const std::filesystem::path& path) {
  std::ifstream f(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}
inline std::uint32_t be32(const unsigned char* p) {
  return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) |
         (std::uint32_t(p[2]) << 8) | p[3];
}
// Independent of libpng: verify chunks/CRC and inflate filter-none reference
// rows.
inline void verify_png(const std::filesystem::path& path,
                       const ImageBuffer& input) {
  const auto bytes = read(path);
  ASSERT_GE(bytes.size(), 8U);
  const unsigned char signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
  for (int i = 0; i < 8; ++i) EXPECT_EQ(bytes[i], signature[i]);
  std::vector<unsigned char> idat;
  bool ihdr = false, srgb = false, end = false;
  for (std::size_t p = 8; p < bytes.size();) {
    ASSERT_GE(bytes.size() - p, 12U);
    auto n = be32(&bytes[p]);
    ASSERT_LE(n, bytes.size() - p - 12);
    std::string type(reinterpret_cast<const char*>(&bytes[p + 4]), 4);
    EXPECT_EQ(crc32(0, &bytes[p + 4], n + 4), be32(&bytes[p + 8 + n]));
    const auto* data = &bytes[p + 8];
    if (type == "IHDR") {
      ASSERT_EQ(n, 13U);
      ihdr = true;
      EXPECT_EQ(be32(data), input.metadata().width);
      EXPECT_EQ(be32(data + 4), input.metadata().height);
      EXPECT_EQ(data[8], 16);
      EXPECT_EQ(data[9], 2);
      EXPECT_EQ(data[12], 0);
    } else if (type == "sRGB") {
      ASSERT_EQ(n, 1U);
      EXPECT_EQ(data[0], 1);
      srgb = true;
    } else if (type == "IDAT")
      idat.insert(idat.end(), data, data + n);
    else if (type == "IEND") {
      EXPECT_EQ(n, 0U);
      end = true;
      EXPECT_EQ(p + 12, bytes.size());
    } else
      ADD_FAILURE() << "Unexpected metadata chunk " << type;
    p += n + 12;
  }
  ASSERT_TRUE(ihdr && srgb && end);
  std::size_t row = input.metadata().width * 6 + 1;
  std::vector<unsigned char> raw(row * input.metadata().height);
  uLongf count = raw.size();
  ASSERT_EQ(uncompress(raw.data(), &count, idat.data(), idat.size()), Z_OK);
  ASSERT_EQ(count, raw.size());
  for (std::size_t y = 0; y < input.metadata().height; ++y) {
    ASSERT_EQ(raw[y * row], 0);
    for (std::size_t x = 0; x < input.metadata().width * 3; ++x) {
      std::size_t p = y * row + 1 + x * 2;
      EXPECT_EQ((unsigned(raw[p]) << 8) | raw[p + 1], input.uint16_row(y)[x]);
    }
  }
}
}  // namespace mini_camera_raw::test_support
#endif
