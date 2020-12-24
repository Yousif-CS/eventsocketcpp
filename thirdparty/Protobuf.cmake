# Installing and linking protobuf to the current project
cmake_minimum_required(VERSION 3.15)


# using FetchContent
include(FetchContent)

FetchContent_Declare (
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG        v3.14.0
	SOURCE_SUBDIR  cmake
)

FetchContent_GetProperties(protobuf)
if (NOT protobuf_POPULATED)
	option(protobuf_BUILD_TESTS OFF)
	message(STATUS "Fetching Protobuf v3.14.0")
	FetchContent_Populate(protobuf)
	#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${protobuf_SOURCE_DIR}/cmake)
	add_subdirectory(${protobuf_SOURCE_DIR}/cmake ${protobuf_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

