#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "png_test_support.h"
namespace mini_camera_raw {
namespace {
class CliPipelineTest : public ::testing::Test {
 protected:
  std::filesystem::path dir;
  void SetUp() override {
    auto pattern =
        (std::filesystem::temp_directory_path() / "mini-p6-XXXXXX").string();
    std::vector<char> buffer(pattern.begin(), pattern.end());
    buffer.push_back(0);
    char* path = mkdtemp(buffer.data());
    ASSERT_NE(path, nullptr);
    dir = path;
  }
  void TearDown() override { std::filesystem::remove_all(dir); }
  int run(std::vector<std::string> args) {
    args.insert(args.begin(), CLI_FIXTURE_PATH);
    std::vector<char*> argv;
    for (auto& arg : args) argv.push_back(arg.data());
    argv.push_back(nullptr);
    const auto log = (dir / "log.txt").string();
    int fd = open(log.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) throw std::runtime_error("cannot create test log");
    pid_t child = fork();
    if (child < 0) {
      close(fd);
      throw std::runtime_error("fork failed");
    }
    if (child == 0) {
      if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
        _exit(126);
      close(fd);
      execv(argv[0], argv.data());
      _exit(127);
    }
    close(fd);
    int status = 0;
    if (waitpid(child, &status, 0) != child || !WIFEXITED(status)) return -1;
    return WEXITSTATUS(status);
  }
};
TEST_F(CliPipelineTest, SyntheticDecodeToPngAndLabeledHistograms) {
  for (const std::string mode : {"preserve", "camera-clip"})
    for (int ev : {-2, 0, 2})
      for (int strength : {0, 1}) {
        auto path = dir / (mode + std::to_string(ev) +
                           std::to_string(strength) + ".png");
        ASSERT_EQ(
            run({"--input", "synthetic", "--output", path.string(),
                 "--highlights", mode, "--ev", std::to_string(ev),
                 "--tone-strength", std::to_string(strength), "--histogram"}),
            0);
        double rgb[] = {.5 * std::exp2(ev), .25 * std::exp2(ev),
                        .375 * std::exp2(ev)};
        // Fixture camera values are below 1, so the P5 baseline is unchanged.
        const double y = (87098. / 409605) * rgb[0] +
                         (175762. / 245763) * rgb[1] +
                         (12673. / 175545) * rgb[2];
        ImageMetadata m{3,
                        3,
                        3,
                        9,
                        PixelFormat::kUInt16,
                        CfaPattern::kNone,
                        ColorState::kEncodedRgb,
                        {0, 65535, false},
                        RgbColorSpace::kSrgb,
                        TransferFunction::kSrgb};
        auto expected = ImageBuffer::create(m);
        for (int row = 0; row < 3; ++row)
          for (int x = 0; x < 3; ++x)
            for (int c = 0; c < 3; ++c) {
              // Independent numeric oracle with the documented float stage
              // boundary.
              double value = std::clamp(
                  double(float(rgb[c] / (1 + strength * y))), 0., 1.);
              double encoded = value <= .0031308
                                   ? 12.92 * value
                                   : 1.055 * std::pow(value, 1. / 2.4) - .055;
              expected.uint16_at(row, x, c) =
                  std::uint16_t(std::floor(65535 * encoded + .5));
            }
        test_support::verify_png(path, expected);
        const auto bytes = test_support::read(dir / "log.txt");
        const std::string text(bytes.begin(), bytes.end());
        EXPECT_NE(text.find("stage=rendered branch=" + mode),
                  std::string::npos);
        EXPECT_NE(text.find("stage=after-exposure branch=preserve"),
                  std::string::npos);
        const auto before = test_support::read(path);
        EXPECT_NE(run({"--input", "synthetic", "--output", path.string(),
                       "--highlights", mode}),
                  0);
        EXPECT_EQ(test_support::read(path), before);
      }
}
TEST_F(CliPipelineTest, SaturatedNegativeExposureUsesCorrectedCameraBranch) {
  auto path = dir / "saturated.png";
  ASSERT_EQ(run({"--input", "saturated", "--output", path.string(),
                 "--highlights", "camera-clip", "--ev", "-2"}),
            0);
  ImageMetadata m{3,
                  3,
                  3,
                  9,
                  PixelFormat::kUInt16,
                  CfaPattern::kNone,
                  ColorState::kEncodedRgb,
                  {0, 65535, false},
                  RgbColorSpace::kSrgb,
                  TransferFunction::kSrgb};
  // P5 clips (2,1,1.5) to neutral 1, then EV-2 gives linear .25.
  const auto value = static_cast<std::uint16_t>(
      std::floor(65535 * (1.055 * std::pow(.25, 1. / 2.4) - .055) + .5));
  auto expected =
      ImageBuffer::from_uint16(m, ImageBuffer::UInt16Storage(27, value));
  test_support::verify_png(path, expected);
}
TEST_F(CliPipelineTest, MissingMetadataAndDecodeFailureCreateNoOutput) {
  for (const std::string input :
       {"missing-wb", "missing-matrix", "unsupported"}) {
    auto path = dir / (input + ".png");
    EXPECT_NE(run({"--input", input, "--output", path.string(), "--highlights",
                   "preserve"}),
              0);
    EXPECT_FALSE(std::filesystem::exists(path));
  }
}
}  // namespace
}  // namespace mini_camera_raw
