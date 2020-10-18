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
set(INSTALL_CONFIGDIR ${CMAKE_SOURCE_DIR}/cmake/EventSocketCpp)
install(TARGETS eventsocketcpp
	EXPORT eventsocketcpp-export
	LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
	ARCHIVE DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

# Export the targets to a script
install(EXPORT eventsocketcpp-export
		FILE
			EventSocketCppTargets.cmake
		NAMESPACE
			EventSocketCpp::
		DESTINATION
			${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/EventSocketCppConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake		
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	DESTINATION	${INSTALL_CONFIGDIR}
)

export(EXPORT eventsocketcpp-export
		FILE ${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppTargets.cmake
		NAMESPACE EventSocketCpp::)

# register package
export(PACKAGE EventSocketCpp)

