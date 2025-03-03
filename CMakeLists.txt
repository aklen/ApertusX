cmake_minimum_required(VERSION 3.10)
project(Apertus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Google Fruit dependency
add_subdirectory(external/fruit)

# Core services (DI, event, logger, config, plugin)
add_subdirectory(src/core)

# Plugins
add_subdirectory(src/plugins/myplugin)
add_subdirectory(src/plugins/gstreamer)

# Main application
add_subdirectory(src/main)
