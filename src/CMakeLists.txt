add_library(
  mini_camera_raw STATIC
  about.cpp
  image_buffer.cpp
)
add_library(mini_camera_raw::mini_camera_raw ALIAS mini_camera_raw)

target_compile_features(mini_camera_raw PUBLIC cxx_std_17)

target_compile_definitions(
  mini_camera_raw
  PRIVATE MINI_CAMERA_RAW_VERSION="${PROJECT_VERSION}"
)

target_include_directories(
  mini_camera_raw
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
