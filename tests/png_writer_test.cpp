#include "mini_camera_raw/png_writer.h"

#include <gtest/gtest.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <zlib.h>

#include <csignal>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
namespace mini_camera_raw {
namespace {
class PngWriterTest : public ::testing::Test {
 protected:
  std::filesystem::path dir;
  void SetUp() override {
    auto pattern =
        (std::filesystem::temp_directory_path() / "mini-p5-XXXXXX").string();
    std::vector<char> writable(pattern.begin(), pattern.end());
    writable.push_back(0);
    char* created = mkdtemp(writable.data());
    ASSERT_NE(created, nullptr);
    dir = created;
  }
  void TearDown() override { std::filesystem::remove_all(dir); }
};
ImageBuffer encoded(std::size_t width = 3, std::size_t height = 2,
                    std::size_t padding = 2) {
  ImageMetadata m{width,
                  height,
                  3,
                  width * 3 + padding,
                  PixelFormat::kUInt16,
                  CfaPattern::kNone,
                  ColorState::kEncodedRgb,
                  {0, 65535, false},
                  RgbColorSpace::kSrgb,
                  TransferFunction::kSrgb};
  auto image = ImageBuffer::create(m);
  for (std::size_t y = 0; y < height; ++y)
    for (std::size_t x = 0; x < width * 3; ++x)
      image.uint16_row(y)[x] = std::uint16_t((x * 10007 + y * 65535) % 65536);
  return image;
}
std::vector<unsigned char> read(const std::filesystem::path& path) {
  std::ifstream f(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}
std::uint32_t be32(const unsigned char* p) {
  return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) |
         (std::uint32_t(p[2]) << 8) | p[3];
}
// Independent of libpng: verify chunks/CRC and inflate filter-none reference
// rows.
void verify_png(const std::filesystem::path& path, const ImageBuffer& input) {
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
TEST_F(PngWriterTest, IndependentRoundTripOddPaddedAndWideRows) {
  for (std::size_t width : {1, 3, 11001}) {
    auto input = encoded(width);
    auto path = dir / (std::to_string(width) + ".png");
    write_png16(input, path.string());
    verify_png(path, input);
  }
}
TEST_F(PngWriterTest, RefusesExistingFilesSymlinksAndDirectories) {
  auto input = encoded();
  auto path = dir / "existing";
  {
    std::ofstream f(path);
    f << "original";
  }
  auto before = read(path);
  EXPECT_THROW(write_png16(input, path.string()), std::runtime_error);
  EXPECT_EQ(read(path), before);
  std::filesystem::create_symlink(path, dir / "link");
  EXPECT_THROW(write_png16(input, (dir / "link").string()), std::runtime_error);
  EXPECT_EQ(read(path), before);
  EXPECT_THROW(write_png16(input, dir.string()), std::runtime_error);
}
TEST_F(PngWriterTest, RejectsBadPathsAndNonEncodedInputBeforeCreation) {
  auto input = encoded();
  EXPECT_THROW(write_png16(input, ""), std::invalid_argument);
  EXPECT_THROW(write_png16(input, std::string("a\0b", 3)),
               std::invalid_argument);
  EXPECT_THROW(write_png16(input, (dir / "missing" / "out.png").string()),
               std::runtime_error);
  ImageMetadata m{1,
                  1,
                  3,
                  3,
                  PixelFormat::kFloat32,
                  CfaPattern::kNone,
                  ColorState::kLinearWorkingRgb,
                  {0, 1, true}};
  EXPECT_THROW(
      write_png16(ImageBuffer::create(m), (dir / "wrong.png").string()),
      std::invalid_argument);
  EXPECT_FALSE(std::filesystem::exists(dir / "wrong.png"));
}
void expect_write_failure(const ImageBuffer& input, const std::string& path,
                          bool require_encoder_failure) {
  const pid_t child = fork();
  ASSERT_GE(child, 0);
  if (child == 0) {
    std::signal(SIGXFSZ, SIG_IGN);
    struct rlimit limit{64, 64};
    if (setrlimit(RLIMIT_FSIZE, &limit) != 0) _exit(3);
    try {
      write_png16(input, path);
    } catch (const std::runtime_error& error) {
      if (require_encoder_failure &&
          std::string(error.what()) != "PNG encoding failed")
        _exit(5);
      _exit(std::filesystem::exists(path) ? 2 : 0);
    } catch (...) {
      _exit(4);
    }
    _exit(1);
  }
  int status = 0;
  ASSERT_EQ(waitpid(child, &status, 0), child);
  ASSERT_TRUE(WIFEXITED(status));
  EXPECT_EQ(WEXITSTATUS(status), 0);
  EXPECT_FALSE(std::filesystem::exists(path));
}
TEST_F(PngWriterTest, WriteFailureRemovesOnlyNewPartialFile) {
  expect_write_failure(encoded(100, 10), (dir / "limited.png").string(), false);
}
TEST_F(PngWriterTest, EncoderLongjmpFailureCleansResourcesAndFile) {
  auto input = encoded(1024, 32);
  std::uint32_t random = 1234567;
  for (std::size_t i = 0; i < input.element_count(); ++i) {
    random = random * 1664525U + 1013904223U;
    input.uint16_data()[i] = static_cast<std::uint16_t>(random >> 16);
  }
  expect_write_failure(input, (dir / "encoder-failure.png").string(), true);
}
}  // namespace
}  // namespace mini_camera_raw
