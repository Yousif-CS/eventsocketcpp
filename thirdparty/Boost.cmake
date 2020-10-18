include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")

endif ()

include(ExternalProject)
set( boost_URL "https://sourceforge.net/projects/boost/files/boost/1.73.0/boost_1_73_0.tar.gz")
set( BOOST "boost_1_73_0")

ExternalProject_Add (
	${BOOST}
	PREFIX				${BOOST}
	URL					${boost_URL}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND 	""
	UPDATE_COMMAND    	""
	INSTALL_COMMAND   	""

	BUILD_ALWAYS 		OFF
	INSTALL_DIR			${CMAKE_CURRENT_BINARY_DIR}/${BOOST}
	
	BUILD_COMMAND		""
)

set(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/${BOOST}/src/${BOOST}/)
