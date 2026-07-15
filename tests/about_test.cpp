#include "mini_camera_raw/about.h"

#include <gtest/gtest.h>

#include <string>

namespace mini_camera_raw {
namespace {

TEST(AboutTest, FormatsVersionBanner) {
  const std::string banner = version_banner();

  EXPECT_EQ("Mini-Camera Raw 0.1.0", banner);
  EXPECT_EQ(std::string::npos, banner.find('\n'));
}

}  // namespace
}  // namespace mini_camera_raw
