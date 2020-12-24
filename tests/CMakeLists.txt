cmake_minimum_required(VERSION 3.15)

find_package(GTest REQUIRED)

add_executable(testMessage testMessage.cpp)

# To use CTests as a test orchestrator
add_test(NAME MessageTests COMMAND testMessage)

target_link_libraries(testMessage
		PRIVATE EventSocketCpp::${PROJECT_NAME} GTest::GTest GTest::Main)
