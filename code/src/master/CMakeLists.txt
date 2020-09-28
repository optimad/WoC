
#------------------------------------------------------------------------------#
# CMake settings
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 2.8)

#------------------------------------------------------------------------------#
# Application settings
#------------------------------------------------------------------------------#

set(MASTER_EXECUTABLE_NAME master CACHE INTERNAL "Executable name of the master application" FORCE)

file(GLOB sources "master.cpp")
add_executable(${MASTER_EXECUTABLE_NAME} ${sources})

target_link_libraries(${MASTER_EXECUTABLE_NAME} common)

install(TARGETS ${MASTER_EXECUTABLE_NAME} DESTINATION bin)
