#configure_file(CMakeLists.txt.jsoncpp
#		jsoncpp/CMakeLists.txt
#		@ONLY)
#set(jsoncpp_work_dir "${CMAKE_BINARY_DIR}/third_party/jsoncpp")
#set(jsoncpp_build_dir "${CMAKE_BINARY_DIR}/jsoncpp-build")
#set(jsoncpp_lib_dir "${CMAKE_BINARY_DIR}/third_party/jsoncpp-lib")

#execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#		WORKING_DIRECTORY ${jsoncpp_work_dir})

#message(STATUS "INSTALL DIR is ${CMAKE_INSTALL_PREFIX}")
#set(CMAKE_INSTALL_COMPONENT "${CMAKE_BINARY_DIR}/jsoncpp-lib")
#message(STATUS "INSTALL COMPONENT is ${CMAKE_INSTALL_COMPONENT}")
#execute_process(COMMAND ${CMAKE_COMMAND} --build .
#		WORKING_DIRECTORY ${jsoncpp_work_dir})
