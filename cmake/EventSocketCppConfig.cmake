cmake_minimum_required(VERSION 3.11)

# @PACKAGE_INIT@
# get_filename_component(EventSocketCpp_CMAKE_DIR "@PACKAGE_CMAKE_INSTALL_LIBDIR@" PATH)
include(CMakeFindDependencyMacro)

# list(APPEND CMAKE_MODULE_PATH ${EventSocketCpp_CMAKE_DIR})
find_dependency(Boost 1.73.0)
# @PACKAGE_INIT@
if(NOT TARGET EventSocketCpp::EventSocketCpp)
		# message(STATUS "PACKAGE_CMAKE_INSTALL_LIBDIR = @PACKAGE_CMAKE_INSTALL_LIBDIR@")
		include("${CMAKE_CURRENT_LIST_DIR}/EventSocketCppTargets.cmake")
endif()

set(EVENTSOCKETCPP_LIBRARIES EventSocketCpp::EventSocketCpp)
