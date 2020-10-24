// A simple event socket client demonstrating usage

// #include <iostream>

// #include "eventsocketcpp/client/CustomWebSocketClient.h"
// #include "eventsocketcpp/EventSocket.h"

#include <eventsocketcpp/client/EventClientInterface.h>

// This decribes the events that could be sent to the server
enum class EventTypes {
    Hello, World
};

// Here we override the OnConnect function to listen to events
class EventClient: public RedBack::Client::EventClientInterface<EventTypes>
{
public:
    // Here we assign some event listeners when we connect to the server
    virtual void OnConnect() override
    {
        OnEvent(EventTypes::World, [](RedBack::Message<EventTypes> msg){
            
            std::string payload;
            msg >> payload;

            std::cout << "[SERVER]" << " Sent: " <<  payload << std::endl; 
        });


        RedBack::Message<EventTypes> msg;
        msg.header.id = EventTypes::Hello;
        msg << "Hello!";

        send(msg);
    }
};

int main(int argc, char* argv[]){


    EventClient client;

    client.connect("127.0.0.1", 60000);

    while(true);
    // if (argc < 3){
    //     std::cout << "Usage: " << argv[0] << " [host] [port]" << std::endl;
    //     return 0;
    // }

    // RedBack::Client::WebSocket ws{argv[1], static_cast<unsigned short>(atoi(argv[2]))};
    // RedBack::EventSocket<RedBack::Client::WebSocket> es{ws};
    

    // es.on_event("Connect", [](std::string payload){
    //     std::cout << "Received event: Connected, payload: " << payload << std::endl;
    // });


}