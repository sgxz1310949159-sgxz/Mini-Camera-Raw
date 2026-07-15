add_executable(mini_camera_raw_test about_test.cpp)
target_compile_features(mini_camera_raw_test PRIVATE cxx_std_17)
target_link_libraries(
  mini_camera_raw_test
  PRIVATE
    GTest::gtest_main
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
