# Live555.cmake - Live555 Library Integration
# Usage: Include this file in the root CMakeLists.txt with include(cmake/Live555.cmake)

set(LIVE555_DIR ${CMAKE_SOURCE_DIR}/external/live555)

# Check if we are using NO_OPENSSL mode
option(NO_OPENSSL "Disable OpenSSL support in Live555" ON)
add_compile_definitions(NO_OPENSSL=$<BOOL:${NO_OPENSSL}>)

# Include directories for Live555
include_directories(
    ${LIVE555_DIR}/UsageEnvironment
    ${LIVE555_DIR}/UsageEnvironment/include
    ${LIVE555_DIR}/BasicUsageEnvironment
    ${LIVE555_DIR}/BasicUsageEnvironment/include
    ${LIVE555_DIR}/groupsock
    ${LIVE555_DIR}/groupsock/include
    ${LIVE555_DIR}/liveMedia
    ${LIVE555_DIR}/liveMedia/include
)

# Import Live555 Static Libraries
add_library(livemedia STATIC IMPORTED)
set_target_properties(livemedia PROPERTIES
    IMPORTED_LOCATION ${LIVE555_DIR}/liveMedia/libliveMedia.a
)

add_library(groupsock STATIC IMPORTED)
set_target_properties(groupsock PROPERTIES
    IMPORTED_LOCATION ${LIVE555_DIR}/groupsock/libgroupsock.a
)

add_library(UsageEnvironment STATIC IMPORTED)
set_target_properties(UsageEnvironment PROPERTIES
    IMPORTED_LOCATION ${LIVE555_DIR}/UsageEnvironment/libUsageEnvironment.a
)

add_library(BasicUsageEnvironment STATIC IMPORTED)
set_target_properties(BasicUsageEnvironment PROPERTIES
    IMPORTED_LOCATION ${LIVE555_DIR}/BasicUsageEnvironment/libBasicUsageEnvironment.a
)

# # Function to link Live555 with a target
function(link_live555 target)
    target_link_libraries(${target} PUBLIC
        livemedia
        groupsock
        UsageEnvironment
        BasicUsageEnvironment
    )

     # Ensure NO_OPENSSL is defined for this target
     target_compile_definitions(${target} PUBLIC NO_OPENSSL=$<BOOL:${NO_OPENSSL}>)
endfunction()
