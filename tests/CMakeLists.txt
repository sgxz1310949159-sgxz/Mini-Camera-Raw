add_executable(
  mini_camera_raw_test
  about_test.cpp
  image_buffer_test.cpp
  demosaic_test.cpp
  white_balance_test.cpp
  normalize_test.cpp
  raw_decoder_test.cpp
)
target_compile_features(mini_camera_raw_test PRIVATE cxx_std_17)
target_include_directories(mini_camera_raw_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(
  mini_camera_raw_test
  PRIVATE
    GTest::gtest_main
    PkgConfig::LibRaw
    mini_camera_raw::mini_camera_raw
)

include(GoogleTest)
gtest_discover_tests(mini_camera_raw_test)

add_test(
  NAME Cli.VersionSmoke
  COMMAND $<TARGET_FILE:mini_camera_raw_cli> --version
)
set_tests_properties(
  Cli.VersionSmoke
  PROPERTIES PASS_REGULAR_EXPRESSION "Mini-Camera Raw 0\\.1\\.0"
)
