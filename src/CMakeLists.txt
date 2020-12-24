cmake_minimum_required(VERSION 3.15.0)

set(LIBRARY_OUTPUT_PATH ./lib)

#find_package(Protobuf CONFIG 3.14.0 REQUIRED)

#add_custom_target(proto_deps
#				  DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

add_library(${PROJECT_NAME} STATIC Message.pb.h Message.pb.cc RedBackMessage.cpp)
add_library(EventSocketCpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

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


