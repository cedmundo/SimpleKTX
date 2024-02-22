cmake_minimum_required(VERSION 3.22)
project(ktxView
  DESCRIPTION "ktxView is an example project of the KTX2 texture usage"
  HOMEPAGE_URL "https://github.com/cedmundo/ktxView"
  VERSION "1.0.0"
  LANGUAGES C CXX)

# Set global configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions("-Wall" "-g")

# HERE(cedmundo): Add Options if needed
# option(EXAMPLE "EXAMPLE" ON)

add_subdirectory(vendor)

# Main executable
add_executable(ktxView)
target_sources(ktxView
  INTERFACE ktxView.h
  PRIVATE ktxView.c main.c)
target_link_libraries(ktxView
  PRIVATE glfw glad ktx_read)

# Copy assets dir
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
if(EXISTS "${ASSETS_DIR}")
  add_custom_command(
    TARGET ktxView PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/assets")

  add_custom_command(
    TARGET ktxView PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()
