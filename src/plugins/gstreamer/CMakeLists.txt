cmake_minimum_required(VERSION 3.10)
project(GStreamerPlugin)

# Find required packages
find_package(PkgConfig REQUIRED)

# Locate GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
if(NOT GSTREAMER_FOUND)
    message(FATAL_ERROR "GStreamer 1.0 not found! Please install the required package.")
endif()

# Locate libcurl (optional)
find_package(CURL)
if(CURL_FOUND)
    message(STATUS "libcurl found, enabling USE_LIBCURL")
    add_definitions(-DUSE_LIBCURL)
    set(USE_LIBCURL ON)
else()
    message(WARNING "libcurl not found, building without URL encoding support")
endif()

# Define the shared library target
add_library(apertus_plugin_gstreamer SHARED GStreamerPlugin.cpp)

# Set include directories
target_include_directories(apertus_plugin_gstreamer PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/helpers
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/plugins/gstreamer
    ${GSTREAMER_INCLUDE_DIRS}   # GStreamer include directories
)

# Create a list of libraries to link
set(TARGET_LIBRARIES
    apertus_core
    ${GSTREAMER_LIBRARIES}
)

# Append libcurl if available
if(USE_LIBCURL)
    list(APPEND TARGET_LIBRARIES CURL::libcurl)
endif()

# Link the required libraries to the target
target_link_libraries(apertus_plugin_gstreamer PUBLIC ${TARGET_LIBRARIES})

# Configure additional build settings
target_link_directories(apertus_plugin_gstreamer PUBLIC ${GSTREAMER_LIBRARY_DIRS})
target_compile_options(apertus_plugin_gstreamer PRIVATE ${GSTREAMER_CFLAGS_OTHER})

# Debug output
message(STATUS "Final linked libraries: ${TARGET_LIBRARIES}")
