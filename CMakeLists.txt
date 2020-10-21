cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

enable_testing()

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)
include(thirdparty/JsonCpp.cmake)
include(thirdparty/Boost.cmake)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(thirdparty)





