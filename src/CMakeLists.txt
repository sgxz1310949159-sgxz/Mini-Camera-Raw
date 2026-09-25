find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED IMPORTED_TARGET GLOBAL libraw>=0.21)
message(STATUS "LibRaw ${LibRaw_VERSION}")

find_package(PNG 1.6 REQUIRED)
message(STATUS "PNG ${PNG_VERSION_STRING}; zlib ${ZLIB_VERSION_STRING}")

add_library(
  mini_camera_raw STATIC
  tone.cpp
  histogram.cpp
  png_writer.cpp
  display_encode.cpp
  color_transform.cpp
  about.cpp
  image_buffer.cpp
  demosaic_bilinear.cpp
  white_balance.cpp
  normalize.cpp
  raw_decoder_libraw.cpp
)
add_library(mini_camera_raw::mini_camera_raw ALIAS mini_camera_raw)
target_link_libraries(mini_camera_raw PRIVATE PkgConfig::LibRaw PNG::PNG)

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
