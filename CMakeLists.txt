cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(openrct2)

# add source files

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# add_subdirectory(src)

add_executable(openrct2 src/platform/osx.c)
