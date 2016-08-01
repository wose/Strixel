cmake_minimum_required(VERSION 3.1.0)

project(Stixel)

if(MSVC)
  set(CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "" FORCE)
endif()

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)

option(ENABLE_SOLUTION_FOLDERS "Solution folder in Visual Studio or in other IDEs" MSVC_IDE)

if(ENABLE_SOLUTION_FOLDERS)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
endif()

add_definitions(-std=c++14)

add_subdirectory(src)

if(NOT NO_TESTS)
  include(CTest)
  add_subdirectory(3rdparty/googletest)
  enable_testing()
  add_subdirectory(tests)
else()
  message(STATUS "Tests disabled")
endif()
