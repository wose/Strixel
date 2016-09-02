set(strixel_SRC
  main.cpp
  AlgSimple.cpp
  Preview.cpp
  Solution.cpp
  ../3rdparty/imgui/examples/libs/gl3w/GL/gl3w.c
  ../3rdparty/imgui/imgui_draw.cpp
  ../3rdparty/imgui/imgui.cpp
  ../3rdparty/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
  )

set(strixel_HDR
  AlgSimple.h
  Preview.h
  Solution.h
  )

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/imgui/)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/imgui/examples/libs/gl3w)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/imgui/examples/opengl3_example/)

# add_definitions(-DIMGUI_DISABLE_TEST_WINDOW)

add_executable(strixel ${strixel_SRC})
target_link_libraries(strixel ${OPENGL_LIBRARIES} ${OpenCV_LIBS} glfw dl pugixml)
