cmake_minimum_required(VERSION 3.15.0)

#add_library(${PROJECT_NAME} STATIC)
#add_dependencies(${PROJECT_NAME} protobuf-install)

# Actually try and find protobuf

# # Create .pb.cc and .pb.h files
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/Message.proto)
# add_custom_target(proto_deps DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# set_source_files_properties(${PROTO_SRCS} PROPERTIES GENERATED TRUE)
# set_source_files_properties(${PROTO_HDRS} PROPERTIES GENERATED TRUE)
# add_dependencies(${PROJECT_NAME} proto_deps)

# set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
# Add the sources

# Make it a dependency

find_package(Protobuf 3.14.0 REQUIRED)

add_library(${PROJECT_NAME} STATIC Message.pb.cc Message.pb.h RedBackMessage.cpp)

target_link_libraries(${PROJECT_NAME}
		PRIVATE
		$<BUILD_INTERFACE:protobuf::libprotobuf>
		-lpthread)

target_include_directories(${PROJECT_NAME} 
PUBLIC
$<INSTALL_INTERFACE:include>
$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
PRIVATE	
$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>)
		
# For avoid name clashes
add_library(EventSocketCpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
		
		