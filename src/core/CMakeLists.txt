add_library(apertus_core SHARED
    config/ConfigService.cpp
    event/EventService.cpp
    logger/LoggerService.cpp
    plugin/PluginService.cpp
    plugin/Plugin.cpp
    replica/Replica.h
    replica/AudioReplica.h
    replica/ReplicaFactory.h
    replica/ReplicaRegistrar.h
    replica/ReplicaService.h
    di/DependencyInjection.cpp
)

# Ensure correct include directories
target_include_directories(apertus_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/interfaces
    ${CMAKE_SOURCE_DIR}/external/fruit/include
    ${CMAKE_SOURCE_DIR}/external/live555/liveMedia/include
    ${CMAKE_SOURCE_DIR}/src/core
)

# Link to Google Fruit (DI system)
target_link_libraries(apertus_core PUBLIC fruit)

# Link Live555 to apertus
link_live555(apertus_core)

# Define NO_OPENSSL for apertus_core-live555
# target_compile_definitions(apertus_core PUBLIC NO_OPENSSL=$<BOOL:${NO_OPENSSL}>)
target_compile_definitions(apertus_core PUBLIC NO_OPENSSL=1)
