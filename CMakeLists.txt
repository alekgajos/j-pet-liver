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


add_subdirectory(src/)

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.70 REQUIRED COMPONENTS
  chrono
  )

make_directory(external/)

# ExternalProject_Add(djpet_software
#   GIT_REPOSITORY https://aleksander_gajos@bitbucket.org/fpgafais/djpet_software.git
#   PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/external/
r #   GIT_TAG c2846e15
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND ""
#   INSTALL_COMMAND ""
#   )

add_subdirectory(external/json)
