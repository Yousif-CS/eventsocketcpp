#include "eventsocketcpp/server/CustomWebSocket.h"
#include "eventsocketcpp/server/ListeningServer.h"
#include "eventsocketcpp/EventSocket.h"

#include "boost/asio.hpp"

int main(int argc, char *argv[]){

    // Initialize the server to listen on the localhost, port 3000
	if (argc < 2){
		std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
		return 0;
	}
	
    RedBack::Server::ListeningServer server{"127.0.0.1", static_cast<unsigned short>(atoi(argv[1]))};
	
	std::cout << "Server Started Listening on port: " << argv[1] << std::endl;
	
	while (true){
    	// Keep accepting connections; the default websocket receive
    	// callback is to echo back the message, we can override that however
        auto websocket = server.accept();
    
		websocket->set_on_receive([websocket](std::string payload) {
			std::cout << "Received: " << payload << std::endl;
        });

		RedBack::EventSocket<RedBack::Server::WebSocket<boost::asio::ip::tcp::socket>> es{*websocket};
		es.emit_event("Connect", "Hello from the server!");
        
    }

	return 0;
}
