cmake_minimum_required(VERSION 3.11)
project(eventsocketcpp-example)

# dependencies
# enable_testing()

# find_package(GTest MODULE REQUIRED)

if(NOT TARGET EventSocketCpp::EventSocketCpp)
	find_package(EventSocketCpp CONFIG REQUIRED)
endif()

# create the target
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server 
		PUBLIC
		EventSocketCpp::EventSocketCpp)


target_link_libraries(client
		PUBLIC
		EventSocketCpp::EventSocketCpp)

target_compile_options(server PRIVATE -ftemplate-backtrace-limit=0)