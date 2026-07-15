#include "mini_camera_raw/about.h"

#include <string_view>

namespace mini_camera_raw {

std::string version_banner() {
  constexpr std::string_view kProjectName = "Mini-Camera Raw";
  return std::string{kProjectName} + " " + MINI_CAMERA_RAW_VERSION;
}

}  // namespace mini_camera_raw
