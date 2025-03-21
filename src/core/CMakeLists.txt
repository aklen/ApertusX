add_library(apertus_core SHARED
    di/DependencyInjection.cpp

    config/ConfigService.cpp
    event/EventService.cpp
    logger/LoggerService.cpp
    plugin/PluginService.cpp
    plugin/Plugin.cpp
    
    replica/ReplicaService.cpp
    replica/ReplicaFactory.cpp
    replica/Replica.cpp
    replica/replicas/AudioReplica.cpp
)

# Ensure correct include directories
target_include_directories(apertus_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core
)

# Link to Google Fruit (DI system)
target_link_libraries(apertus_core PUBLIC fruit)
