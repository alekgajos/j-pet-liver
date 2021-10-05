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

#enable_testing()
add_subdirectory(src/)
#add_subdirectory(tests)

make_directory(external/)

ExternalProject_Add(djpet_software
  GIT_REPOSITORY https://aleksander_gajos@bitbucket.org/fpgafais/djpet_software.git
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/external/
  GIT_TAG c2846e15
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/external/
  INSTALL_COMMAND ""
  )

FetchContent_MakeAvailable(json)

