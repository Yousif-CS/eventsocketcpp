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
		OnEvent(EventTypes::Hello, conn, [this, conn](RedBack::Message<EventTypes> msg){
			
			// log the message when a new message is received, and send it back

			std::string payload;

			msg >> payload;

			std::cout << "[" << conn->GetID() << "]" << " Sent: " << payload << std::endl;

			RedBack::Message<EventTypes> responseMsg;
			responseMsg.setID(EventTypes::World);
			responseMsg << "Hello from the server!";

			MessageClient(conn, responseMsg);
		});

		return true;

	}

	//When a client disconnect
	virtual bool OnDisconnect(std::shared_ptr<RedBack::Connection<EventTypes>> conn) override
	{
		std::cout << "[" << conn->GetID() << "]" << " Disconnected" << std::endl;
	}
	
};

int main(int argc, char *argv[]){

	EventServer server{60000};

	// run the context on 4 threads
	server.start(5);

	while(true)
	{
		server.update(5, true);
	}

	return 0;
}
