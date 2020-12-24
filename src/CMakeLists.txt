cmake_minimum_required(VERSION 3.15.0)

set(LIBRARY_OUTPUT_PATH ./lib)

#find_package(Protobuf CONFIG 3.14.0 REQUIRED)

#add_custom_target(proto_deps
#				  DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

add_library(${PROJECT_NAME} STATIC Message.pb.h Message.pb.cc RedBackMessage.cpp)

#add_dependencies(${PROJECT_NAME} proto_deps)

target_link_libraries(${PROJECT_NAME}
		PRIVATE
		$<BUILD_INTERFACE:libprotobuf>
		$<BUILD_INTERFACE:libprotobuf-lite>
		$<BUILD_INTERFACE:libprotoc>
		-lpthread)

target_include_directories(${PROJECT_NAME} 
		PUBLIC
            $<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
		PRIVATE	
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
			$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>)

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

# Get all the protobuf out of the build directory
# (FindProtobuf somehow cannot install them except there so we have to extract them)
#file (GLOB	Protobuf_files
#		"${CMAKE_BINARY_DIR}/src/*.pb.h")

#install(FILES
#		${Protobuf_files}
#		DESTINATION include/eventsocketcpp)
#install(FILES
#		${PROTO_HDRS}
#		DESTINATION include/eventsocketcpp)

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

