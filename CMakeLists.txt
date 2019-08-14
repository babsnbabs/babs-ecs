cmake_minimum_required(VERSION 3.9)

project(babs-ecs)

# compiler settings
# compiler is set with CC/CXX environment variables

if (WIN32)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic -W")
endif()


# External dependencies
# include/link directories get exported from this dependencies.cmake file

# for use in the rest of this CMakeLists.txt script
include(dependencies.cmake)

# Add libraries to the compilation/linking search paths
include_directories(
    SYSTEM
    ${DOCTEST_INCLUDE_DIR}
)

link_directories(
    ${PROJECT_SOURCE_DIR}/build
)

# tests
add_executable(tests
    src/tests.cpp
    src/bitfield/bitfield_tests.cpp
    src/manager_tests.cpp
    src/pubsub/eventmanager_tests.cpp
)
add_dependencies(tests doctest)

# benchmark
add_executable(babs-benchmark
    src/benchmark.cpp
)

# release - must be called explicitly with `make release`
set(RELEASE_SOURCES
    "README.md"
    "LICENSE"
    "src/*.hpp"
    "src/**/*.hpp"
)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(release
    WORKING_DIRECTORY "${ROOT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${ROOT_DIR}/build/release.zip" --format=zip ${RELEASE_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${ROOT_DIR}/build/release.tar.gz" ${RELEASE_SOURCES})
