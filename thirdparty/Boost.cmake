cmake_minimum_required(VERSION 3.11)

include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")

endif ()

# include(ExternalProject)
include(FetchContent)
set( boost_URL "https://sourceforge.net/projects/boost/files/boost/1.73.0/boost_1_73_0.tar.gz")
set( BOOST "boost_1_73_0")

#ExternalProject_Add (
#	${BOOST}
#	PREFIX				${BOOST}
#	URL					${boost_URL}
#	BUILD_IN_SOURCE 1
#	CONFIGURE_COMMAND 	""
#	UPDATE_COMMAND    	""
#	INSTALL_COMMAND   	""
#
#	BUILD_ALWAYS 		OFF
#	INSTALL_DIR			${CMAKE_CURRENT_BINARY_DIR}/${BOOST}
#	
#	BUILD_COMMAND		""
#)

#set(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/${BOOST}/src/${BOOST}/)

#FetchContent_Declare(
#	${BOOST}
#	PREFIX 		${BOOST}
#	URL			${boost_URL}
#	CONFIGURE_COMMAND "./bootstrap"
#	BUILD_COMMAND "./b2"	
#)
#
#FetchContent_GetProperties(${BOOST})
#
#if (NOT ${BOOST}_POPULATED)
#	FetchContent_Populate(${BOOST})
#	# add_subdirectory(${${BOOST}_SOURCE_DIR} ${${BOOST}_BINARY_DIR})
#	# include_directories(${Boost_INCLUDE_DIRS})
#endif()
find_package(Boost 1.73.0 REQUIRED)
