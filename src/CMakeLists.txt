cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ./lib)

file(
	GLOB_RECURSE
	MY_SOURCE_FILES
	ListeningServer.cpp
	CustomWebSocketClient.cpp
	CustomWebSocket.cpp
)

add_library(${PROJECT_NAME} STATIC ${MY_SOURCE_FILES})

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} 
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
		PRIVATE		
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
		)

# target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# target_include_directories(${PROJECT_NAME} PUBLIC ${jsoncpp_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME}
		PUBLIC
			jsoncpp -lpthread)

# Export the targets to a script

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

install(
	EXPORT eventsocketcpp-export
	FILE
		EventSocketCppTargets.cmake
	NAMESPACE
		EventSocketCpp::
	DESTINATION
		cmake/eventsocketcpp
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES 
		${CMAKE_SOURCE_DIR}/cmake/EventSocketCppConfig.cmake
		${CMAKE_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	DESTINATION	lib/cmake/eventsocketcpp
)

export(EXPORT eventsocketcpp-export
		FILE cmake/eventsocketcpp/EventSocketCppTargets.cmake
		NAMESPACE EventSocketCpp::)

# register package
export(PACKAGE EventSocketCpp)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

