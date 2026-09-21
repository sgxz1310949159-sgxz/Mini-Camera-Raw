#include "mini_camera_raw/png_writer.h"

#include <fcntl.h>
#include <png.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <limits>
#include <stdexcept>
#include <vector>
namespace mini_camera_raw {
namespace {
// C-style helper: longjmp never crosses a C++ object with a nontrivial
// destructor.
void png_failure(png_structp png, png_const_charp) { png_longjmp(png, 1); }
void png_warning_ignored(png_structp, png_const_charp) {}
bool encode_file(FILE* file, const std::uint16_t* samples, std::size_t stride,
                 png_uint_32 width, png_uint_32 height, unsigned char* row) {
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                            png_failure, png_warning_ignored);
  if (!png) return false;
  png_infop info = png_create_info_struct(png);
  if (!info) {
    png_destroy_write_struct(&png, nullptr);
    return false;
  }
  if (setjmp(png_jmpbuf(png))) {
    png_destroy_write_struct(&png, &info);
    return false;
  }
  png_init_io(png, file);
  png_set_IHDR(png, info, width, height, 16, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_set_sRGB(png, info, PNG_sRGB_INTENT_RELATIVE);
  // First scalar writer chooses filter-none to keep scanline evidence simple.
  png_set_filter(png, PNG_FILTER_TYPE_BASE, PNG_FILTER_NONE);
  png_write_info(png, info);
  for (png_uint_32 y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < std::size_t(width) * 3; ++x) {
      const auto value = samples[std::size_t(y) * stride + x];
      row[2 * x] = static_cast<unsigned char>(value >> 8);
      row[2 * x + 1] = static_cast<unsigned char>(value & 255);
    }
    png_write_row(png, row);
  }
  png_write_end(png, info);
  png_destroy_write_struct(&png, &info);
  return true;
}
class NewOutput {
 public:
  explicit NewOutput(const std::string& path) : path_(path) {
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0)
      throw std::runtime_error("cannot exclusively create PNG output");
    if (::fstat(fd, &identity_) != 0) {
      ::close(fd);
      // Without identity evidence, do not unlink a possibly replaced pathname.
      throw std::runtime_error(
          "cannot inspect new PNG output; file may remain");
    }
    stream_ = ::fdopen(fd, "wb");
    if (!stream_) {
      ::close(fd);
      remove_owned_file();
      throw std::runtime_error("cannot open PNG output stream");
    }
  }
  ~NewOutput() {
    if (stream_) std::fclose(stream_);
    if (!complete_) remove_owned_file();
  }
  NewOutput(const NewOutput&) = delete;
  NewOutput& operator=(const NewOutput&) = delete;
  FILE* stream() const { return stream_; }
  void finish() {
    const int flushed = std::fflush(stream_);
    const int closed = std::fclose(stream_);
    stream_ = nullptr;
    if (flushed != 0 || closed != 0)
      throw std::runtime_error("cannot finish PNG output");
    complete_ = true;
  }

 private:
  void remove_owned_file() noexcept {
    struct stat current{};
    if (::lstat(path_.c_str(), &current) == 0 &&
        current.st_dev == identity_.st_dev &&
        current.st_ino == identity_.st_ino && S_ISREG(current.st_mode))
      ::unlink(path_.c_str());
  }
  const std::string& path_;
  struct stat identity_{};
  FILE* stream_ = nullptr;
  bool complete_ = false;
};
}  // namespace
const char* png_library_version() noexcept {
  return png_get_libpng_ver(nullptr);
}
void write_png16(const ImageBuffer& encoded, const std::string& path) {
  const auto& m = encoded.metadata();
  if (m.color_state != ColorState::kEncodedRgb ||
      m.rgb_color_space != RgbColorSpace::kSrgb ||
      m.transfer_function != TransferFunction::kSrgb)
    throw std::invalid_argument("PNG output requires encoded sRGB RGB16");
  if (path.empty() || path.find('\0') != std::string::npos)
    throw std::invalid_argument("invalid PNG output path");
  if (m.width > PNG_UINT_31_MAX || m.height > PNG_UINT_31_MAX ||
      m.width > std::numeric_limits<std::size_t>::max() / 6)
    throw std::length_error("PNG dimensions or row size exceed format limits");
  std::vector<unsigned char> row(m.width * 6);
  NewOutput file(path);
  if (!encode_file(file.stream(), encoded.uint16_data(), m.row_stride_elements,
                   static_cast<png_uint_32>(m.width),
                   static_cast<png_uint_32>(m.height), row.data()))
    throw std::runtime_error("PNG encoding failed");
  file.finish();
}
}  // namespace mini_camera_raw
