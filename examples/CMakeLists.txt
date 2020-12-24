cmake_minimum_required(VERSION 3.11)
project(eventsocketcpp-example)

# dependencies
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ~/.local/lib/cmake)

if(NOT TARGET EventSocketCpp::EventSocketCpp)
	find_package(EventSocketCpp CONFIG REQUIRED)
endif()

# create the target
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server 
		PRIVATE
		EventSocketCpp::EventSocketCpp)


target_link_libraries(client
		PRIVATE
		EventSocketCpp::EventSocketCpp)

target_compile_options(server PRIVATE -ftemplate-backtrace-limit=0)
