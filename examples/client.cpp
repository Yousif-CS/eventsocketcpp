// A simple event socket client demonstrating usage

// #include <iostream>

// #include "eventsocketcpp/client/CustomWebSocketClient.h"
// #include "eventsocketcpp/EventSocket.h"

#include <eventsocketcpp/client/EventClientInterface.h>
#include <cstdlib>
#include <atomic>

// This decribes the events that could be sent to the server
enum class EventTypes {
    Hello, World, RandomNumber
};

// Here we override the OnConnect function to listen to events
class EventClient: public RedBack::Client::EventClientInterface<EventTypes>
{
protected:
    // Here we assign some event listeners when we connect to the server
    virtual void OnConnect() override
    {
        OnEvent(EventTypes::World, [this](RedBack::Message<EventTypes> msg){
            
            std::string payload;
            msg >> payload;

            std::cout << "[SERVER]" << " Sent: " <<  payload << std::endl; 
            
            //For example, when I receive event World, i will try and create a room
            createRoom();
        });

        RedBack::Message<EventTypes> msg;
        msg.header.id = EventTypes::Hello;
        msg << "Hello!";

        send(msg);

    }


    // Override: when a client forwards a message to you 
    virtual void OnForward(RedBack::Message<EventTypes> msg, uint32_t forwarderID)
    {
        std::string payload;
        msg >> payload;
        std::cout << "[" << forwarderID << "]" << " Sent: " << payload << std::endl;
    }

    // Override: when a client broadcasts a message
    virtual void OnBroadCast(RedBack::Message<EventTypes> msg, uint32_t broadcasterID)
    {
        if (msg.header.id == EventTypes::RandomNumber)
        {
            int random;
            msg >> random;
            std::cout << "[" << broadcasterID << "]" << " Broadcasted a random number: " << random << std::endl;
        }

    }

    // Override: when a room is created, the server will send its id to all clients
    // and this callback is received 
    virtual void OnRoomCreated(RedBack::Message<EventTypes> msg, uint32_t roomID)
    {
        // Try to join the room
        joinRoom(roomID);

        // The OnJoinedRoom Callback will be invoked
    }

    // Override: when a room is joined
    virtual void OnRoomJoined(uint32_t roomID)
    {
        // Try to send a random number to indicate a different client each time :D
        RedBack::Message<EventTypes> msg;
        msg.header.id = EventTypes::RandomNumber;

        int max;
        max = 1000; //set the upper bound to generate the random number
        srand(time(0));

        msg << rand()%max;
        broadcastRoom(msg, roomID);

    }
};


// Just to wait for input before disconnecting the 
std::atomic<bool> stop{false};


void PressAnyKeyLoop()
{
    while(!stop.load());
}

int main(int argc, char* argv[]){


    EventClient client;

    client.connect("127.0.0.1", 60000);

    // Here we can keep processing messages blocking the main thread, 
    // or better, we can read messages on a seperate thread;
    std::thread processMessages([&client]() { client.update(5, true); });
    std::thread waitForKeyPress(PressAnyKeyLoop);

    processMessages.join();
    waitForKeyPress.join();
    
    //wait for key process
    char newline[2];
    std::cin.getline(newline, 1);

    stop = true;

    std::cout << "Disconnecting..." << std::endl;
}