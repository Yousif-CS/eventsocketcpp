cmake_minimum_required(VERSION 3.11)
project(EventSocketCppTests)



list(APPEND CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ~/.local/lib/cmake)

find_package(GTest REQUIRED)

if(NOT TARGET EventSocketCpp::EventSocketCpp)
	find_package(EventSocketCpp CONFIG REQUIRED)
endif()

file(
    GLOB_RECURSE
    TEST_FILES
    testMessage.cpp
)


add_executable(${PROJECT_NAME} ${TEST_FILES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                    PRIVATE EventSocketCpp::EventSocketCpp GTest::GTest GTest::Main)
