// #include "eventsocketcpp/server/CustomWebSocket.h"
// #include "eventsocketcpp/server/ListeningServer.h"
// #include "eventsocketcpp/EventSocket.h"

// #include "boost/asio.hpp"
#include <eventsocketcpp/server/EventServerInterface.h>

// This represents the events we can send
enum class EventTypes {
	Hello, World
};

// This is a customized EventServer
class EventServer: public RedBack::Server::EventServerInterface<EventTypes> 
{

public:
	using RedBack::Server::EventServerInterface<EventTypes>::EventServerInterface;

	// We setup our events here
	// We have to return true to indicate we want to actually accept the connection
	virtual bool OnConnect(std::shared_ptr<RedBack::Connection<EventTypes>> conn) override
	{
		OnEvent(EventTypes::Hello, conn, [this, &conn](RedBack::Message<EventTypes> msg){
			
			// log the message when a new message is received, and send it back

			std::string payload;

			// We have to provide a fixed size as we will be reading into a buffer
			payload.reserve(msg.header.size);
			
			msg >> payload;

			std::cout << "[" << conn->GetID() << "]" << " Sent: " << payload << std::endl;

			RedBack::Message<EventTypes> responseMsg;
			responseMsg.header.id = EventTypes::World;
			msg << "Hello from the server!";

			MessageClient(conn->shared_from_this(), msg);
		});

		return true;

	}
};


int main(int argc, char *argv[]){

	EventServer server{60000};

	server.start();

	// server.OnEvent(EventTypes::Hello, [](RedBack::Message<EventTypes> msg){
	// 	std::cout << "Received: " << std::to_string(msg.body) << std::endl;
	// });

	while(true)
	{
		server.update(5, true);
	}

	return 0;
}
