find_package(ZLIB REQUIRED)

add_executable(
  mini_camera_raw_test
  editor_queue_test.cpp
  editor_session_test.cpp
  editor_pipeline_test.cpp
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

target_link_libraries(mini_camera_raw_test PRIVATE mini_camera_raw_render mini_camera_raw_editor)

add_test(NAME Cli.Arguments COMMAND ${CMAKE_COMMAND}
  -DCLI=$<TARGET_FILE:mini_camera_raw_cli>
  -DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/cli-arguments
  -P ${CMAKE_CURRENT_SOURCE_DIR}/cli_test.cmake)

add_executable(mini_camera_raw_cli_fixture ../apps/mini_camera_raw.cpp cli_fixture_decoder.cpp)
target_link_libraries(mini_camera_raw_cli_fixture PRIVATE mini_camera_raw_render mini_camera_raw_editor)
target_compile_definitions(mini_camera_raw_test PRIVATE CLI_FIXTURE_PATH="$<TARGET_FILE:mini_camera_raw_cli_fixture>")
add_dependencies(mini_camera_raw_test mini_camera_raw_cli_fixture)

if(MINI_CAMERA_RAW_BUILD_GUI)
  add_executable(mini_camera_raw_gui_image_test gui_image_test.cpp)
  target_link_libraries(mini_camera_raw_gui_image_test PRIVATE mini_camera_raw_gui_image GTest::gtest_main)
  gtest_discover_tests(mini_camera_raw_gui_image_test)
endif()

if(MINI_CAMERA_RAW_BUILD_GUI)
  find_package(Qt6 6.11 REQUIRED COMPONENTS Widgets)
  add_executable(mini_camera_raw_gui_editor_test gui_editor_test.cpp cli_fixture_decoder.cpp)
  target_link_libraries(mini_camera_raw_gui_editor_test PRIVATE
    Qt6::Widgets mini_camera_raw_gui_image mini_camera_raw_editor GTest::gtest)
  gtest_discover_tests(mini_camera_raw_gui_editor_test)
endif()
