#include "mini_camera_raw/about.h"

#include <cstdlib>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
  if (argc == 2 && std::string_view{argv[1]} == "--version") {
    std::cout << mini_camera_raw::version_banner() << '\n';
    return EXIT_SUCCESS;
  }

  std::cerr << "Usage: mini-camera-raw --version\n";
  return EXIT_FAILURE;
}
