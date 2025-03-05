add_library(apertus_myplugin SHARED MyPlugin.cpp)

target_include_directories(apertus_myplugin PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/
)

# Link to core shared library
target_link_libraries(apertus_myplugin PUBLIC apertus_core)
