# load the cmake commands we need to use
include(ExternalProject)
find_package(Git REQUIRED)

# where we want to install our libraries
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

ExternalProject_Add(
    doctest
    PREFIX ${LIBS_DIR}/doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")