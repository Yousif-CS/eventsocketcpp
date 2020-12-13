cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)


set(LIBRARY_OUTPUT_PATH ./lib)

find_package(Protobuf 3.13.0 REQUIRED)

# Here we generate the .cc and .h files from the .proto files defined in the repo
file (GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_custom_target(proto_dep DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

add_library(${PROJECT_NAME} STATIC example.cpp)
add_dependencies(${PROJECT_NAME} proto_dep)

target_link_libraries(${PROJECT_NAME}
		PUBLIC
			-lpthread ${Protobuf_LIBRARIES})

target_include_directories(${PROJECT_NAME} 
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
		PRIVATE		
			$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
		)

			

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

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/eventsocketcpp
		DESTINATION include)


# Get all the protobuf out of the build directory
# (FindProtobuf somehow cannot install them except there so we have to extract them)
file (GLOB	Protobuf_files
		"${CMAKE_BINARY_DIR}/src/*.pb.h")

install(FILES
		${Protobuf_files}
		DESTINATION include/eventsocketcpp)

# export(EXPORT EventSocketCpp
# 		FILE cmake/eventsocketcpp/EventSocketCppTargets.cmake
# 		NAMESPACE EventSocketCpp::)

# # register package
# export(PACKAGE EventSocketCpp)

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

