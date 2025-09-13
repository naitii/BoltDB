cmake_minimum_required(VERSION 3.10)
project(BoltDB)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Add executable
add_executable(boltdb
    main.cpp
    datastore.cpp
    persistence.cpp
    server.cpp
    http_server.cpp
)

# Link libraries
if(WIN32)
    target_link_libraries(boltdb ws2_32)
endif()

# Set output directory
set_target_properties(boltdb PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print build information
message(STATUS "Building BoltDB for ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
