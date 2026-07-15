add_executable(mini_camera_raw_cli mini_camera_raw.cpp)
set_target_properties(
  mini_camera_raw_cli
  PROPERTIES OUTPUT_NAME "mini-camera-raw"
)

target_compile_features(mini_camera_raw_cli PRIVATE cxx_std_17)
target_link_libraries(
  mini_camera_raw_cli
  PRIVATE mini_camera_raw::mini_camera_raw
)
