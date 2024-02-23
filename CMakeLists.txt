cmake_minimum_required(VERSION 3.22)
project(SimpleKTX
  DESCRIPTION "SimpleKTX is an example project of the KTX2 texture usage"
  HOMEPAGE_URL "https://github.com/cedmundo/SimpleKTX"
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
add_executable(SimpleKTX)
target_sources(SimpleKTX
  INTERFACE app.h
  PRIVATE app.c main.c)
target_link_libraries(SimpleKTX
  PRIVATE glfw glad ktx_read)

# Copy assets dir
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
if(EXISTS "${ASSETS_DIR}")
  add_custom_command(
    TARGET SimpleKTX PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/assets")

  add_custom_command(
    TARGET SimpleKTX PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()
