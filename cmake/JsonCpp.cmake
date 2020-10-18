include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")

endif ()

include(ExternalProject)
set( jsoncpp_URL "https://github.com/open-source-parsers/jsoncpp")
set( JSONCPP "jsoncpp")

ExternalProject_Add (
	${JSONCPP}
	
	PREFIX 				${JSONCPP}
	GIT_REPOSITORY 		https://github.com/open-source-parsers/jsoncpp
	GIT_TAG			  	master
	BUILD_IN_SOURCE 1
	INSTALL_DIR    		"${CMAKE_CURRENT_BINARY_DIR}/ext/${EP_TAGLIB}"
)
