cmake_minimum_required(VERSION 3.15.0)

add_library(${PROJECT_NAME} STATIC Message.pb.cc Message.pb.h RedBackMessage.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	protobuf::libprotobuf
	-lpthread)

target_include_directories(${PROJECT_NAME} 
	PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
	PRIVATE	
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
	$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>)
		
# To avoid name clashes
add_library(EventSocketCpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
		
		
