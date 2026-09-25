#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "mini_camera_raw/about.h"
#include "mini_camera_raw/demosaic.h"
#include "mini_camera_raw/display_encode.h"
#include "mini_camera_raw/png_writer.h"
#include "mini_camera_raw/white_balance.h"
#include "render_pipeline.h"
namespace {
using namespace mini_camera_raw;
struct Options {
  std::string input;
  std::string output;
  double ev = 0;
  double strength = 0;
  app::HighlightMode highlights = app::HighlightMode::kPreserve;
  bool histogram = false;
};
constexpr const char* usage =
    "Usage: mini-camera-raw --input INPUT --output OUTPUT\n"
    "       --highlights preserve|camera-clip [--ev EV] [--tone-strength S]\n"
    "       camera-clip: normal P5 baseline; preserve: uncorrected diagnostic\n"
    "       [--histogram]\n"
    "       mini-camera-raw --version | --help\n"
    "EV: [-20,20], default 0; S: [0,1], default 0 (off).\n"
    "Native Sony ILCE-7CM2 Bayer only. RGB16 sRGB PNG; no overwrite or "
    "rotation.\n";
double parameter(const std::string& text, double low, double high,
                 const char* name) {
  std::istringstream stream(text);
  stream.imbue(std::locale::classic());
  double value = 0;
  stream >> std::noskipws >> value;
  if (!stream || stream.peek() != std::char_traits<char>::eof() ||
      !std::isfinite(value) || value < low || value > high)
    throw std::invalid_argument(std::string("invalid ") + name + " parameter");
  return value;
}
Options parse(int argc, char* argv[]) {
  Options result;
  std::set<std::string_view> seen;
  for (int i = 1; i < argc; ++i) {
    const std::string_view key(argv[i]);
    if (key != "--input" && key != "--output" && key != "--ev" &&
        key != "--tone-strength" && key != "--highlights" &&
        key != "--histogram")
      throw std::invalid_argument("unknown option");
    if (!seen.insert(key).second)
      throw std::invalid_argument("duplicate option");
    if (key == "--histogram") {
      result.histogram = true;
      continue;
    }
    if (++i >= argc || std::string_view(argv[i]).empty() ||
        std::string_view(argv[i]).substr(0, 2) == "--")
      throw std::invalid_argument("missing option value");
    const std::string value(argv[i]);
    if (key == "--input")
      result.input = value;
    else if (key == "--output")
      result.output = value;
    else if (key == "--ev")
      result.ev = parameter(value, -20, 20, "EV");
    else if (key == "--tone-strength")
      result.strength = parameter(value, 0, 1, "tone");
    else if (value == "preserve")
      result.highlights = app::HighlightMode::kPreserve;
    else if (value == "camera-clip")
      result.highlights = app::HighlightMode::kCameraClip;
    else
      throw std::invalid_argument("invalid highlights mode");
  }
  if (result.input.empty() || result.output.empty() ||
      !seen.count("--highlights"))
    throw std::invalid_argument("input, output and highlights are required");
  return result;
}
void print_histogram(const char* stage, const char* branch, const char* state,
                     const RgbLuminanceHistogram& hist,
                     const Options& options) {
  std::cout << "histogram stage=" << stage << " branch=" << branch
            << " state=" << state
            << " space=srgb transfer=linear ev=" << options.ev
            << " strength=" << options.strength << '\n';
  const char* names[] = {"R", "G", "B", "Y"};
  for (int c = 0; c < 4; ++c) {
    const auto& channel = hist.channels[c];
    std::cout << names[c] << " min=" << channel.minimum
              << " max=" << channel.maximum
              << " below_zero=" << channel.below_zero
              << " above_one=" << channel.above_one << " bins=";
    for (std::size_t i = 0; i < channel.bins.size(); ++i)
      std::cout << (i == 0 ? "" : ",") << channel.bins[i];
    std::cout << '\n';
  }
}
}  // namespace
int main(int argc, char* argv[]) {
  using namespace mini_camera_raw;
  if (argc == 2 && std::string_view(argv[1]) == "--version") {
    std::cout << version_banner() << '\n';
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view(argv[1]) == "--help") {
    std::cout << usage;
    return EXIT_SUCCESS;
  }
  try {
    const auto options = parse(argc, argv);
    // Intermediate RAW/Bayer buffers leave scope before full RGB rendering.
    auto camera_and_matrix = [&] {
      auto decoded = decode_raw(options.input);
      if (!decoded.sensor.camera_wb_tile)
        throw std::invalid_argument("missing camera white balance metadata");
      if (decoded.sensor.orientation != 0)
        std::cerr << "Note: orientation metadata is not applied; output uses "
                     "sensor row order.\n";
      auto matrix = camera_to_working_matrix(decoded.sensor);
      auto normalized = normalize(decoded.image, decoded.sensor.levels);
      auto balanced = apply_white_balance(
          normalized, normalize_camera_wb(*decoded.sensor.camera_wb_tile,
                                          normalized.metadata().cfa_pattern));
      return std::make_pair(demosaic_bilinear(balanced), matrix);
    }();
    auto result = app::render_camera(
        camera_and_matrix.first, camera_and_matrix.second, options.ev,
        options.strength, options.highlights, options.histogram);
    write_png16(encode_srgb16(result.image), options.output);
    std::cout.imbue(std::locale::classic());
    std::cout << std::setprecision(17);
    if (result.histograms) {
      const auto& h = *result.histograms;
      print_histogram("before-exposure", "preserve", "linear-working",
                      h.before_exposure, options);
      print_histogram("after-exposure", "preserve", "linear-working",
                      h.after_exposure, options);
      print_histogram("after-tone", "preserve", "tone-mapped", h.after_tone,
                      options);
      print_histogram("rendered",
                      options.highlights == app::HighlightMode::kPreserve
                          ? "preserve"
                          : "camera-clip",
                      "tone-mapped", h.rendered, options);
    }
    std::cout << "Wrote RGB16 sRGB PNG.\n";
    return EXIT_SUCCESS;
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n' << usage;
    return EXIT_FAILURE;
  }
}
