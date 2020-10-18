// A listening server which handles incoming connection requests
#include <iostream>
#include "boost/asio.hpp"
#include "ListeningServer.h"

namespace net = boost::asio;


namespace RedBack {

	/*
	produces a websocket object that is ready to send/receive connections
	*/
	std::shared_ptr<RedBack::WebSocket<tcp::socket>> ListeningServer::accept() {
		for (;;) {
			
			//this will be the socket that holds the connection
			tcp::socket socket{ io_context_ };
			
			static tcp::acceptor acceptor{ io_context_, {net::ip::make_address(host_), port_} };
			//blocks until we receive a new connection
			acceptor.accept(socket);

			//return a new WebSocket object
			std::shared_ptr<WebSocket<tcp::socket>> ws = std::make_shared<WebSocket<tcp::socket>>(std::move(socket));
			websockets.push_back(ws->getPtr());
			return ws->getPtr();
		}
	}

} // RedBack