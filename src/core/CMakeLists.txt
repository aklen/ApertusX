add_library(apertus_core SHARED
    config/ConfigService.cpp
    event/EventService.cpp
    logger/LoggerService.cpp
    plugin/PluginService.cpp
    plugin/Plugin.cpp
    di/DependencyInjection.cpp
)

# Ensure correct include directories
target_include_directories(apertus_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core
)

# Link to Google Fruit (DI system)
target_link_libraries(apertus_core PUBLIC fruit)
