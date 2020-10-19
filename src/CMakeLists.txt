cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ./lib)

file(
	GLOB_RECURSE
	MY_SOURCE_FILES
	*
)

add_library(${PROJECT_NAME} ${MY_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} 
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src
		)

# target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# target_include_directories(${PROJECT_NAME} PUBLIC ${jsoncpp_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME}
		PUBLIC
			jsoncpp -lpthread)


include(GNUInstallDirs)
# set(INSTALL_CONFIGDIR ${CMAKE_SOURCE_DIR}/cmake/EventSocketCpp)

install(
	TARGETS ${PROJECT_NAME}
	EXPORT eventsocketcpp-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

