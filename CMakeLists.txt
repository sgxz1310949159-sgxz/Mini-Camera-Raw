cmake_minimum_required(VERSION 3.24)

project(
  MiniCameraRaw
  VERSION 0.1.0
  DESCRIPTION "A learning-first RAW processing pipeline"
  LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(apps)

include(CTest)

if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL
      https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    URL_HASH
      SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
  )
  FetchContent_MakeAvailable(googletest)

  add_subdirectory(tests)
endif()
