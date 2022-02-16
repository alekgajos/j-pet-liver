cmake_minimum_required(VERSION 3.2)

set(EXECUTABLE liver)

project(${EXECUTABLE} VERSION 0.1
  LANGUAGES CXX)

set(SOURCES
  setup.cpp
  signals.cpp
  liver.cpp
  )

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(${EXECUTABLE} ${SOURCES})

set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

target_compile_options(
  ${EXECUTABLE}
  PRIVATE
  -Wall -Wextra -Werror -pedantic
  )

target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../external/unpacker/)

target_link_libraries(${EXECUTABLE} PRIVATE
  nlohmann_json::nlohmann_json
  Boost::chrono
  )
