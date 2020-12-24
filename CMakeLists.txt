cmake_minimum_required(VERSION 3.15.0)
project(eventsocketcpp VERSION 0.1.0)

enable_testing()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)


add_subdirectory(thirdparty)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(examples)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	add_subdirectory(packaging)
	add_subdirectory(tests)
endif()



