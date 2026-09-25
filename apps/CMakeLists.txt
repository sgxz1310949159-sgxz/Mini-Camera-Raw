add_library(mini_camera_raw_render STATIC render_pipeline.cpp)
target_link_libraries(mini_camera_raw_render PUBLIC mini_camera_raw::mini_camera_raw)
target_include_directories(mini_camera_raw_render PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(mini_camera_raw_cli mini_camera_raw.cpp)
set_target_properties(
  mini_camera_raw_cli
  PROPERTIES OUTPUT_NAME "mini-camera-raw"
)

target_compile_features(mini_camera_raw_cli PRIVATE cxx_std_17)
target_link_libraries(
  mini_camera_raw_cli
  PRIVATE mini_camera_raw_render
)
