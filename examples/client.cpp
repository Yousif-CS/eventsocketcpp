// A simple event socket client demonstrating usage

#include <iostream>

#include "eventsocketcpp/client/CustomWebSocketClient.h"
#include "eventsocketcpp/EventSocket.h"


int main(int argc, char* argv[]){

    if (argc < 3){
        std::cout << "Usage: " << argv[0] << " [host] [port]" << std::endl;
        return 0;
    }

    RedBack::Client::WebSocket ws{argv[1], static_cast<unsigned short>(atoi(argv[2]))};
    RedBack::EventSocket<RedBack::Client::WebSocket> es{ws};
    

    es.on_event("Connect", [](std::string payload){
        std::cout << "Received event: Connected, payload: " << payload << std::endl;
    });
}