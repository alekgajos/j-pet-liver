cmake_minimum_required(VERSION 3.2)

project(liver VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_PLATFORM_INDEPENDENT_CODE True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# additional libraries
include(FindPkgConfig)
include(ExternalProject)
include(FetchContent)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70 REQUIRED COMPONENTS
  chrono
  )

make_directory(external/)

add_subdirectory(external/json)
add_subdirectory(src/)
