cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

enable_testing()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)


add_subdirectory(thirdparty)
add_subdirectory(src)
add_subdirectory(include)



# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake"
  INSTALL_DESTINATION lib/cmake
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

export (EXPORT EventSocketCpp
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/EventSocketCppTargets.cmake"
        NAMESPACE EventSocketCpp::)

install(
	FILES 
		${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	DESTINATION	lib/cmake    
)    



