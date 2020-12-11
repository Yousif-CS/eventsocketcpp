cmake_minimum_required(VERSION 3.11)

include(CMakeFindDependencyMacro)

find_dependency(Boost 1.73.0)
find_dependency(jsoncpp)
if(NOT TARGET EventSocketCpp::EventSocketCpp)
		include("${CMAKE_CURRENT_LIST_DIR}/EventSocketCppTargets.cmake")
endif()

set(EVENTSOCKETCPP_LIBRARIES EventSocketCpp::EventSocketCpp)
