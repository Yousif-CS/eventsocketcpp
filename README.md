# eventsocketcpp
A websocket event based library that allows for event based network programming written in C++.
The API provides both client and server support. However, the implementation is purely on top of websockets
which is language agnostic. 

## Currently Supported OS's
  - Linux
  
-----------------------------------------------------------------------
## Prequisites:
  - CMake v.3.10.2 or above.
-----------------------------------------------------------------------
## Dependencies:
  - boost v.1.73.0 or above.
  - protobuf
  
-----------------------------------------------------------------------
## How to use the library:

  Clone the repository or add it as a CMake ExternalProject. If you choose to clone it manually,
  then you can also use find_package(eventsocketcpp) in Cmake and it will take care of building it.
  
An example repository exists here: 
