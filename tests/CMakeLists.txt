find_package(ZLIB REQUIRED)

add_executable(
  mini_camera_raw_test
  render_pipeline_test.cpp
  cli_pipeline_test.cpp
  tone_test.cpp
  histogram_test.cpp
  png_writer_test.cpp
  display_encode_test.cpp
  color_transform_test.cpp
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
    ZLIB::ZLIB
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

target_include_directories(mini_camera_raw_test PRIVATE "${PROJECT_SOURCE_DIR}/apps")

target_link_libraries(mini_camera_raw_test PRIVATE mini_camera_raw_render)

add_test(NAME Cli.Arguments COMMAND ${CMAKE_COMMAND}
  -DCLI=$<TARGET_FILE:mini_camera_raw_cli>
  -DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/cli-arguments
  -P ${CMAKE_CURRENT_SOURCE_DIR}/cli_test.cmake)

add_executable(mini_camera_raw_cli_fixture ../apps/mini_camera_raw.cpp cli_fixture_decoder.cpp)
target_link_libraries(mini_camera_raw_cli_fixture PRIVATE mini_camera_raw_render)
target_compile_definitions(mini_camera_raw_test PRIVATE CLI_FIXTURE_PATH="$<TARGET_FILE:mini_camera_raw_cli_fixture>")
add_dependencies(mini_camera_raw_test mini_camera_raw_cli_fixture)
