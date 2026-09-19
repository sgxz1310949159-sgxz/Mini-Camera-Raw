#include "mini_camera_raw/raw_decoder.h"
#include "raw_decoder_internal.h"

#include <libraw/libraw.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mini_camera_raw {
namespace {
std::size_t checked_product(std::size_t a, std::size_t b) {
  if (a && b > std::numeric_limits<std::size_t>::max() / a)
    throw std::length_error("RAW layout multiplication overflows size_t");
  return a * b;
}

std::size_t validate_geometry(const SensorGeometry& g) {
  if (!g.raw_width || !g.raw_height || !g.active_width || !g.active_height ||
      g.active_left > g.raw_width || g.active_top > g.raw_height ||
      g.active_width > g.raw_width - g.active_left ||
      g.active_height > g.raw_height - g.active_top)
    throw std::invalid_argument("RAW active area is outside sensor dimensions");
  const auto minimum_pitch = checked_product(g.raw_width, sizeof(std::uint16_t));
  if (g.raw_pitch_bytes % sizeof(std::uint16_t) || g.raw_pitch_bytes < minimum_pitch)
    throw std::invalid_argument("RAW pitch is odd or shorter than sensor width");
  return checked_product(g.raw_pitch_bytes, g.raw_height) / sizeof(std::uint16_t);
}

template<std::size_t N>
std::string bounded_string(const char (&text)[N]) {
  return std::string(text, std::find(text, text + N, '\0'));
}

void require_supported(const libraw_data_t& d) {
  if (bounded_string(d.idata.make) != "Sony" ||
      bounded_string(d.idata.model) != "ILCE-7CM2" ||
      bounded_string(d.idata.cdesc) != "RGBG" ||
      d.idata.dng_version || d.idata.raw_count != 1 || d.idata.is_foveon ||
      d.idata.colors != 3 || d.idata.filters < 1000 || d.sizes.pixel_aspect != 1)
    throw std::runtime_error("unsupported RAW: requires native single-frame Sony ILCE-7CM2 RGB Bayer");
}

CfaPattern read_cfa(LibRaw& decoder, std::array<unsigned, 4>& indices) {
  std::string tile;
  for (float value : decoder.imgdata.color.cam_mul)
    if (!std::isfinite(value))
      throw std::invalid_argument("nonfinite camera white balance metadata");
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 2; ++x) {
      const int index = decoder.COLOR(y, x);
      if (index < 0 || index > 3)
        throw std::runtime_error("unsupported CFA color index");
      indices[2 * y + x] = static_cast<unsigned>(index);
      tile += decoder.imgdata.idata.cdesc[index];
    }
  }
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 2; ++x) {
      const auto actual = decoder.COLOR(y, x);
      const auto expected = indices[2 * (y % 2) + x];
      const auto& d = decoder.imgdata;
      // LibRaw may label equivalent greens as 1 or 3 (including open_bayer's
      // final encoded row). Alias only if the positional contract is unchanged.
      if (actual < 0 || actual > 3 ||
          d.idata.cdesc[actual] != d.idata.cdesc[expected] ||
          d.color.cblack[actual] != d.color.cblack[expected] ||
          d.color.cam_mul[actual] != d.color.cam_mul[expected])
        throw std::runtime_error("unsupported non-2x2 CFA layout");
    }
  }
  if (tile == "RGGB") return CfaPattern::kRggb;
  if (tile == "BGGR") return CfaPattern::kBggr;
  if (tile == "GRBG") return CfaPattern::kGrbg;
  if (tile == "GBRG") return CfaPattern::kGbrg;
  throw std::runtime_error("unsupported CFA colors");
}

void check_libraw(int code, const char* stage) {
  if (code != LIBRAW_SUCCESS)
    throw std::runtime_error(std::string(stage) + ": " + LibRaw::strerror(code));
}

// LibRaw's default callback prints filenames; errors are instead surfaced by
// return codes and error_count(), without disclosing private paths.
// LibRaw 0.21 uses int offsets; 0.22 uses INT64. Deduce the callback ABI
// from set_dataerror_handler instead of fixing it to the local version.
template <typename Offset>
void quiet_data_error(void*, const char*, Offset) {}
}  // namespace

namespace detail {
ImageBuffer copy_active_mosaic(const std::uint16_t* samples, std::size_t count,
                              const SensorGeometry& g, CfaPattern cfa) {
  const auto required = validate_geometry(g);
  if (!samples || count < required)
    throw std::invalid_argument("RAW mosaic storage is missing or too short");
  auto output = ImageBuffer::create({g.active_width, g.active_height, 1,
      g.active_width, PixelFormat::kUInt16, cfa, ColorState::kRawBayer,
      {0, 65535, false}});
  const auto stride = g.raw_pitch_bytes / sizeof(std::uint16_t);
  for (std::size_t y = 0; y < g.active_height; ++y)
    std::copy_n(samples + (g.active_top + y) * stride + g.active_left,
                g.active_width, output.uint16_row(y));
  return output;
}

DecodedRaw copy_libraw_result(LibRaw& decoder) {
  const auto& d = decoder.imgdata;
  require_supported(d);
  if (!d.rawdata.raw_image || d.rawdata.color3_image || d.rawdata.color4_image ||
      d.rawdata.float_image || d.rawdata.float3_image || d.rawdata.float4_image)
    throw std::runtime_error("unsupported or missing integer mosaic storage");
  if (d.color.cblack[4] || d.color.cblack[5])
    throw std::runtime_error("spatial black correction tables are not supported");
  if (d.color.raw_bps > 16)
    throw std::runtime_error("unsupported reported source bit depth");
  std::array<unsigned, 4> indices{};
  const auto cfa = read_cfa(decoder, indices);
  SensorMetadata sensor{};
  const auto& s = d.sizes;
  sensor.geometry = {s.raw_width, s.raw_height, s.left_margin, s.top_margin,
                      s.width, s.height, s.raw_pitch};
  const auto count = validate_geometry(sensor.geometry);
  for (std::size_t p = 0; p < 4; ++p) {
    // Convert before addition, avoiding unsigned overflow in malformed metadata.
    sensor.levels.black[p] = static_cast<double>(d.color.black) + d.color.cblack[indices[p]];
    sensor.levels.white[p] = d.color.maximum;
    if (sensor.levels.white[p] > 65535 ||
        sensor.levels.black[p] >= sensor.levels.white[p])
      throw std::invalid_argument("invalid decoded sensor black/white levels");
  }
  std::array<double, 4> wb{};
  bool wb_present = true;
  for (std::size_t p = 0; p < 4; ++p) {
    wb[p] = d.color.cam_mul[indices[p]];
    if (wb[p] <= 0) wb_present = false;
  }
  if (wb_present) sensor.camera_wb_tile = wb;
  std::array<double, 12> matrix{};
  bool matrix_present = false;
  for (std::size_t row = 0; row < 3; ++row)
    for (std::size_t col = 0; col < 4; ++col) {
      const double value = d.color.rgb_cam[row][col];
      if (!std::isfinite(value))
        throw std::invalid_argument("nonfinite camera color matrix metadata");
      matrix[row * 4 + col] = value;
      matrix_present = matrix_present || value != 0;
    }
  if (matrix_present) sensor.camera_to_srgb = matrix;
  sensor.source_bits = d.color.raw_bps;
  sensor.orientation = s.flip;
  sensor.libraw_version = LibRaw::version();
  sensor.unpack_options = d.rawparams.options;
  auto image = copy_active_mosaic(d.rawdata.raw_image, count, sensor.geometry, cfa);
  return {std::move(image), std::move(sensor)};
}
}  // namespace detail

DecodedRaw decode_raw(const std::string& path) {
  if (path.empty() || path.find('\0') != std::string::npos)
    throw std::invalid_argument("RAW path must be nonempty and contain no NUL");
  LibRaw decoder;
  decoder.set_dataerror_handler(quiet_data_error, nullptr);
  decoder.imgdata.rawparams.use_rawspeed = 0;
  decoder.imgdata.rawparams.shot_select = 0;
  decoder.imgdata.rawparams.options &= ~LIBRAW_RAWOPTIONS_CONVERTFLOAT_TO_INT;
  check_libraw(decoder.open_file(path.c_str()), "RAW open failed");
  require_supported(decoder.imgdata);
  if (decoder.is_floating_point())
    throw std::runtime_error("floating RAW input is not supported");
  check_libraw(decoder.unpack(), "RAW unpack failed");
  if (decoder.error_count())
    throw std::runtime_error("RAW unpack reported corrupt data");
  return detail::copy_libraw_result(decoder);
}
}  // namespace mini_camera_raw
