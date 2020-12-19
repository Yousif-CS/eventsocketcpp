# Installing and linking protobuf to the current project

cmake_minimum_required(VERSION 3.11)


# using FetchContent
include(FetchContent)

FetchContent_Declare (
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
    GIT_TAG        v3.14.0
    SOURCE_SUBDIR       cmake
)

FetchContent_GetProperties(protobuf)

if (NOT protobuf_POPULATED)
		#    FetchContent_Populate(protobuf)
		#add_subdirectory(${protobuf_SOURCE_DIR}/cmake ${protobuf_BINARY_DIR})
	FetchContent_MakeAvailable(protobuf)
endif()
