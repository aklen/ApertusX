add_executable(apertus main.cpp)

target_include_directories(apertus PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/plugins
)

# Link to shared core library
target_link_libraries(apertus PUBLIC apertus_core apertus_myplugin apertus_plugin_gstreamer)
