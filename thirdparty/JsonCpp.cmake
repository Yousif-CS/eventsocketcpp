# A file that builds jsoncpp from its git repo

cmake_minimum_required(VERSION 3.11.0)

include(FetchContent)

FetchContent_Declare(
	jsoncpp
	GIT_REPOSITORY	https://github.com/open-source-parsers/jsoncpp.git
	GIT_TAG			master
	CMAKE_ARGS		"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/jsoncpp"
)

FetchContent_GetProperties(jsoncpp)

if (NOT jsoncpp_POPULATED)
	FetchContent_MakeAvailable(jsoncpp)
endif()
