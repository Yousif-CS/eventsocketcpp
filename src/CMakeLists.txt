cmake_minimum_required(VERSION 3.10.0)
project(eventsocketcpp VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ./lib)

include_directories( 
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
		PRIVATE
			${CMAKE_SOURCE_DIR}/src
		)

include_directories(PUBLIC ${Boost_INCLUDE_DIRS})

include_directories(PUBLIC ${PROJECT_NAME} ${jsoncpp_INCLUDE_DIRS})

file(
	GLOB_RECURSE
	MY_SOURCE_FILES
	*
)

add_library(${PROJECT_NAME} ${MY_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
		PUBLIC
			${JSONCPP_LIBS}
		PRIVATE
			-lpthread) 


include(GNUInstallDirs)
# set(INSTALL_CONFIGDIR ${CMAKE_SOURCE_DIR}/cmake/EventSocketCpp)

install(
	TARGETS ${PROJECT_NAME}
	EXPORT eventsocketcpp-export
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

