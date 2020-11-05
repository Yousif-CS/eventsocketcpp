cmake_minimum_required(VERSION 3.11.0)
project(eventsocketcpp VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ./lib)

include(FindProtobuf)
find_package(Protobuf REQUIRED)


# Here we generate the .cc and .h files from the .proto files defined in the repo
file (GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_custom_target(proto_dep DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

file(
	GLOB_RECURSE
	MY_SOURCE_FILES
	ListeningServer.cpp
	CustomWebSocketClient.cpp
	CustomWebSocket.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)


add_library(${PROJECT_NAME} STATIC ${MY_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} proto_dep)

target_link_libraries(${PROJECT_NAME}
		PUBLIC
			jsoncpp -lpthread ${Protobuf_LIBRARIES})

target_include_directories(${PROJECT_NAME} 
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
		PRIVATE		
			$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
			$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
		)

			

# Export the targets to a script

include(GNUInstallDirs)

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
		lib/cmake/eventsocketcpp
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

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/eventsocketcpp
		DESTINATION include)

# Get all the protobuf out of the build directory
# (FindProtobuf somehow cannot install them except there so we have to extract them)

file (GLOB	Protobuf_files
		"${CMAKE_BINARY_DIR}/src/*.pb.h")

install(FILES
		${Protobuf_files}
		DESTINATION include/eventsocketcpp)

export(EXPORT eventsocketcpp-export
		FILE cmake/eventsocketcpp/EventSocketCppTargets.cmake
		NAMESPACE EventSocketCpp::)

# register package
export(PACKAGE EventSocketCpp)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME EventSocketCpp)

