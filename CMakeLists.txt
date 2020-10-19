cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)
include(thirdparty/JsonCpp.cmake)
include(thirdparty/Boost.cmake)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(thirdparty)

# Export the targets to a script
include(GNUInstallDirs)
install(
	EXPORT eventsocketcpp-export
	FILE
		EventSocketCppTargets.cmake
	NAMESPACE
		EventSocketCpp::
	DESTINATION
		lib/cmake/eventsocketcpp
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	EventSocketCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)
install(DIRECTORY include/eventsocketcpp
		DESTINATION include)

#configure_package_config_file(
#	${CMAKE_CURRENT_SOURCE_DIR}/cmake/EventSocketCppConfig.cmake.in
#	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake
#	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/eventsocketcpp
#	PATH_VARS
#	CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_PREFIX
#)
# message(STATUS "CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")
# Install the config, configversion and custom find modules
install(
	FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/EventSocketCppConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	DESTINATION	lib/cmake/eventsocketcpp
)

export(EXPORT eventsocketcpp-export
		FILE cmake/eventsocketcpp/EventSocketCppTargets.cmake
		NAMESPACE EventSocketCpp::)

# register package
export(PACKAGE EventSocketCpp)


