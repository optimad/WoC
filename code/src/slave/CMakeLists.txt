
#------------------------------------------------------------------------------#
# CMake settings
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 2.8)

#------------------------------------------------------------------------------------#
# External dependencies
#------------------------------------------------------------------------------------#
find_package(CURL REQUIRED)

#------------------------------------------------------------------------------#
# Application settings
#------------------------------------------------------------------------------#
set(SLAVE_EXECUTABLE_NAME slave CACHE INTERNAL "Executable name of the slave application" FORCE)

include_directories(${CURL_INCLUDE_DIRS})

file(GLOB sources "slave.cpp")
add_executable(${SLAVE_EXECUTABLE_NAME} ${sources})

target_link_libraries(${SLAVE_EXECUTABLE_NAME} "${CURL_LIBRARIES}")
if(WIN32)
    target_link_libraries(${SLAVE_EXECUTABLE_NAME} wldap32 wsock32 ws2_32)
endif()

add_definitions(-DCURL_STATICLIB)

install(TARGETS ${SLAVE_EXECUTABLE_NAME} DESTINATION bin)
