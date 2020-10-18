# A file that builds jsoncpp from its git repo

cmake_minimum_required(VERSION 3.0.0)

include(ExternalProject)

ExternalProject_Add(
	jsoncpp
	PREFIX			jsoncpp
	GIT_REPOSITORY	https://github.com/open-source-parsers/jsoncpp.git
	GIT_TAG			master
	SOURCE_DIR		"${CMAKE_BINARY_DIR}/jsoncpp-src"
	BINARY_DIR		"${CMAKE_BINARY_DIR}/jsoncpp-build"
	INSTALL_DIR		"${CMAKE_BINARY_DIR}/jsoncpp-lib"
	CMAKE_ARGS		"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/jsoncpp"
)

set(JSONCPP_LIBS ${CMAKE_BINARY_DIR}/jsoncpp/lib/libjsoncpp_static.a)
