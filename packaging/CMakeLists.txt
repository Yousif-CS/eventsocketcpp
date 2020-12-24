# Export the targets to a script
include(GNUInstallDirs)

install(
	TARGETS ${PROJECT_NAME}
	EXPORT EventSocketCpp
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}/include/eventsocketcpp
		DESTINATION include)

# register package
export(PACKAGE EventSocketCpp)

install(
	EXPORT EventSocketCpp
	FILE
		EventSocketCppTargets.cmake
	NAMESPACE
		EventSocketCpp::
	DESTINATION
		lib/cmake
)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)

configure_package_config_file(${${PROJECT_NAME}_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake"
  INSTALL_DESTINATION lib/cmake
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

install (EXPORT EventSocketCpp
		DESTINATION lib/cmake/# "${CMAKE_CURRENT_BINARY_DIR}/cmake/EventSocketCppTargets.cmake"
        NAMESPACE EventSocketCpp::)

install(
	FILES 
		${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/EventSocketCppConfigVersion.cmake
	DESTINATION	lib/cmake    
)    
