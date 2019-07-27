cmake_minimum_required(VERSION 3.9)

project(babs-ecs)

# compiler settings
# compiler is set with CC/CXX environment variables


if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W3")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pedantic -W")
endif(WIN32)


# External dependencies
# include/link directories get exported from this dependencies.cmake file
# for use in the rest of this CMakeLists.txt script
include(dependencies.cmake)

# Add libraries to the compilation/linking search paths
include_directories(
    SYSTEM
    ${CATCH_INCLUDE_DIR}
)

link_directories(
    ${PROJECT_SOURCE_DIR}/build
)

# common source files - things that get tested basically
set(TESTED_SOURCES)

# tests
add_executable(tests
    src/tests.cpp
    ${TESTED_SOURCES}
	src/bitfield_tests.cpp
	src/manager_tests.cpp
)
add_dependencies(tests catch)

# main
add_executable(babs-ecs
    src/main.cpp
    ${TESTED_SOURCES}
)
add_dependencies(babs-ecs tests)

