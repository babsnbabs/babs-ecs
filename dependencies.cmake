# load the cmake commands we need to use
include(ExternalProject)

# where we want to install our libraries
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

# unit testing library
ExternalProject_Add(
    catch
    PREFIX ${LIBS_DIR}/catch
    URL "https://github.com/catchorg/Catch2/releases/download/v2.9.1/catch.hpp"
    TIMEOUT 10
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_DIR ${LIBS_DIR}/catch/src/catch
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

set(CATCH_INCLUDE_DIR ${LIBS_DIR}/catch/src/) # for use in CMakeLists.txt
