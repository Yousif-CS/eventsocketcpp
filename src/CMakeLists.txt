cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ./lib)

find_package(Protobuf 3.14.0 REQUIRED)

# Here we generate the .cc and .h files from the .proto files defined in the repo
file (GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

message("Generated Protobuf source and header files ${PROTO_SRCS} ${PROTO_HDRS}.")

SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_library(${PROJECT_NAME} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(${PROJECT_NAME}
		INTERFACE
			-lpthread ${Protobuf_LIBRARIES})

target_include_directories(${PROJECT_NAME} 
		INTERFACE
            $<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
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

message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

# Get all the protobuf out of the build directory
# (FindProtobuf somehow cannot install them except there so we have to extract them)
file (GLOB	Protobuf_files
		"${CMAKE_BINARY_DIR}/src/*.pb.h")

install(FILES
		${Protobuf_files}
		DESTINATION include/eventsocketcpp)
install(FILES
		${PROTO_HDRS}
		DESTINATION include/eventsocketcpp)

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

