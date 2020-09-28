
#------------------------------------------------------------------------------#
# CMake settings
#------------------------------------------------------------------------------#

# Required CMake version
cmake_minimum_required(VERSION 2.8)

# Name your project here
project("windows_on_cluster")

#------------------------------------------------------------------------------------#
# Variables visible to the user
#------------------------------------------------------------------------------------#
set(ENABLE_MPI 0 CACHE BOOL "If set, the program is compiled with MPI support")
set(VERBOSE_MAKE 0 CACHE BOOL "Set appropriate compiler and cmake flags to enable verbose output from compilation")

#------------------------------------------------------------------------------------#
# Internal dependencies
#------------------------------------------------------------------------------------#
include_directories("master")
include_directories("slave")

#------------------------------------------------------------------------------------#
# MPI
#------------------------------------------------------------------------------------#
if(ENABLE_MPI)
    find_package(MPI)

    include_directories("${MPI_INCLUDE_PATH}")

    if(MPI_C_COMPILE_FLAGS)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPI_C_COMPILE_FLAGS}")
    endif()

    if(MPI_CXX_COMPILE_FLAGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
    endif()

    if(MPI_CXX_LINK_FLAGS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS}")
    endif()
endif()

#------------------------------------------------------------------------------------#
# Compiler settings
#------------------------------------------------------------------------------------#
set(GLORIA_DEFINITIONS "")

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

set(ENABLE_WARNINGS ${VERBOSE_MAKE})
if(VERBOSE_MAKE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

if(ENABLE_MPI)
    list(APPEND GLORIA_DEFINITIONS "ENABLE_MPI=1")
else()
    list(APPEND GLORIA_DEFINITIONS "ENABLE_MPI=0")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(ENABLE_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if(NOT("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_LESS "2.8.12"))
    add_compile_options("-std=c++11")
else()
    add_definitions("-std=c++11")
endif()

# Define a preprocessor macro to recognize debug builds
IF(CMAKE_BUILD_TYPE_LOWER MATCHES "debug" OR CMAKE_BUILD_TYPE_LOWER MATCHES "debinfo")
    list(APPEND GLORIA_DEFINITIONS "ENABLE_DEBUG=1")
else()
    list(APPEND GLORIA_DEFINITIONS "ENABLE_DEBUG=0")
endif()

IF(NOT CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    list(APPEND GLORIA_DEFINITIONS "NDEBUG")
endif()

# Add the definitions
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${GLORIA_DEFINITIONS})

#------------------------------------------------------------------------------------#
# Versions
#------------------------------------------------------------------------------------#

# Master
set(VERSION_HEADER_FILENAME "${PROJECT_SOURCE_DIR}/src/master/master_version.hpp")
string(TOUPPER "MASTER_VERSION" VERSION_DEFINE_NAME)

file(READ "${VERSION_HEADER_FILENAME}" header)

string(REGEX REPLACE ".*#[ \t]*define[ \t]*${VERSION_DEFINE_NAME}[ \t]*\"([^\n]*)\".*" "\\1" match "${header}")
set(MASTER_VERSION "${match}")

STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+(-[0-9A-Za-z-]+)?" "\\1" match "${MASTER_VERSION}")
set(MASTER_MAJOR_VERSION "${match}")

STRING(REGEX REPLACE "^[0-9]+\\.([0-9])+\\.[0-9]+(-[0-9A-Za-z-]+)?" "\\1" match "${MASTER_VERSION}")
set(MASTER_MINOR_VERSION "${match}")

STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)(-[0-9A-Za-z-]+)?" "\\1" match "${MASTER_VERSION}")
set(MASTER_PATCH_VERSION "${match}")

STRING(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+-([0-9A-Za-z-]+)" match "${MASTER_VERSION}")
if(NOT match STREQUAL "")
    STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+-([0-9A-Za-z-]+)" "\\1" match "${MASTER_VERSION}")
    set(MASTER_TAG_VERSION "${match}")
else()
    set(MASTER_TAG_VERSION "")
endif()

# Slave
set(VERSION_HEADER_FILENAME "${PROJECT_SOURCE_DIR}/src/slave/slave_version.hpp")
string(TOUPPER "SLAVE_VERSION" VERSION_DEFINE_NAME)

file(READ "${VERSION_HEADER_FILENAME}" header)

string(REGEX REPLACE ".*#[ \t]*define[ \t]*${VERSION_DEFINE_NAME}[ \t]*\"([^\n]*)\".*" "\\1" match "${header}")
set(SLAVE_VERSION "${match}")

STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+(-[0-9A-Za-z-]+)?" "\\1" match "${SLAVE_VERSION}")
set(SLAVE_MAJOR_VERSION "${match}")

STRING(REGEX REPLACE "^[0-9]+\\.([0-9])+\\.[0-9]+(-[0-9A-Za-z-]+)?" "\\1" match "${SLAVE_VERSION}")
set(SLAVE_MINOR_VERSION "${match}")

STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)(-[0-9A-Za-z-]+)?" "\\1" match "${SLAVE_VERSION}")
set(SLAVE_PATCH_VERSION "${match}")

STRING(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+-([0-9A-Za-z-]+)" match "${SLAVE_VERSION}")
if(NOT match STREQUAL "")
    STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+-([0-9A-Za-z-]+)" "\\1" match "${SLAVE_VERSION}")
    set(SLAVE_TAG_VERSION "${match}")
else()
    set(SLAVE_TAG_VERSION "")
endif()

#------------------------------------------------------------------------------------#
# Subdirectories
#------------------------------------------------------------------------------------#

# Main program
add_subdirectory(src)

# Docs
#add_subdirectory(doc)

# Tests
#enable_testing()
#add_subdirectory(test)
