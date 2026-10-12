add_library(mini_camera_raw_editor STATIC editor_pipeline.cpp editor_session.cpp editor_queue.cpp)
target_link_libraries(mini_camera_raw_editor PUBLIC mini_camera_raw::mini_camera_raw)
target_include_directories(mini_camera_raw_editor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

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

if(MINI_CAMERA_RAW_BUILD_GUI)
  find_package(Qt6 6.11 REQUIRED COMPONENTS Widgets)
  add_library(mini_camera_raw_gui_image STATIC gui_image.cpp)
  target_include_directories(mini_camera_raw_gui_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(mini_camera_raw_gui_image PUBLIC mini_camera_raw::mini_camera_raw Qt6::Gui)
  add_executable(mini_camera_raw_gui gui_main.cpp)
  set_target_properties(mini_camera_raw_gui PROPERTIES OUTPUT_NAME "mini-camera-raw-gui")
  if(APPLE)
    set_target_properties(mini_camera_raw_gui PROPERTIES
      MACOSX_BUNDLE TRUE
      MACOSX_BUNDLE_GUI_IDENTIFIER "org.minicameraraw.editor"
      MACOSX_BUNDLE_BUNDLE_NAME "Mini-Camera Raw")
  endif()
  target_compile_features(mini_camera_raw_gui PRIVATE cxx_std_17)
  target_link_libraries(mini_camera_raw_gui PRIVATE Qt6::Widgets mini_camera_raw_gui_image mini_camera_raw_editor)
endif()
