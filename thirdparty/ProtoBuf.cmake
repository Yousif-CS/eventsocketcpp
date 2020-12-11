# Installing and linking protobuf to the current project

cmake_minimum_required(VERSION 3.11)


# using FetchContent
include(FetchContent)


FetchContent_Declare (
    protobuf
    GIT_REPOSITORY      https://github.com/protocolbuffers/protobuf.git
    SOURCE_SUBDIR       cmake
)

FetchContent_GetProperties(protobuf)

if (NOT protobuf_POPULATED)
    FetchContent_MakeAvailable(protobuf)
endif()