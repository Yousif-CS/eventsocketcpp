cmake_minimum_required(VERSION 3.15)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server 
		PRIVATE
		EventSocketCpp::${PROJECT_NAME})


target_link_libraries(client
		PRIVATE
		EventSocketCpp::${PROJECT_NAME})

target_compile_options(server PRIVATE -ftemplate-backtrace-limit=0)
