cmake_minimum_required(VERSION 3.22)
project(ktxView
  DESCRIPTION "ktxView is an example project of the KTX2 texture usage"
  HOMEPAGE_URL "https://github.com/cedmundo/ktxView"
  VERSION "1.0.0"
  LANGUAGES C)

# Set global configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions("-Wall" "-g")

# HERE(cedmundo): Add Options if needed
# option(EXAMPLE "EXAMPLE" ON)

# HERE(cedmundo): Add vendor libraries
# add_subdirectory(vendor)

add_executable(ktxView)
target_sources(ktxView
  PRIVATE main.c)
