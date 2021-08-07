cmake_minimum_required(VERSION 3.15)

# Include google tests
include(FetchContent)
FetchContent_Declare(
		googletest
		# Specify the commit you depend on and update it regularly.
		GIT_REPOSITORY https://github.com/google/googletest.git
)

FetchContent_MakeAvailable(googletest)


add_executable(testMessage testMessage.cpp)

# To use CTests as a test orchestrator
add_test(NAME MessageTests COMMAND testMessage)

target_link_libraries(testMessage
		PRIVATE EventSocketCpp::${PROJECT_NAME} gtest_main)
