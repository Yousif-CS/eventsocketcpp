# eventsocketcpp
A websocket event based library that allows for event based network programming written in C++.
The API provides both client and server support. However, the implementation is purely on top of websockets
which is language agnostic. 


-----------------------------------------------------------------------
## Prequisites:
  - CMake v.3.10.2 or above.
-----------------------------------------------------------------------
## Dependencies:
  - boost v.1.73.0 or above.

-----------------------------------------------------------------------
## How to use the library:

  1- Run the build script "INSTALL.sh" which will install the required dependencies above.
  2- change your directory to build/ and type the following command:
   ```cmake ..``` which will generate the required libeventsocket.a on linux.
  3- link against the library and include the header files and start hacking :D
