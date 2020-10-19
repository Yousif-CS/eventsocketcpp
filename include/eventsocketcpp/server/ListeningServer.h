#ifndef LISTENINGSERVERH
#define LISTENINGSERVERH

#include <iostream>
#include <vector>
#include <string>

#include "boost/asio.hpp"
#include "CustomWebSocket.h"

using tcp = boost::asio::ip::tcp;
namespace net = boost::asio;
using namespace boost::beast::websocket;

namespace RedBack {

/*
	A server that accepts incoming websocket connections
*/
class ListeningServer {

	public:

		ListeningServer(std::string host, unsigned short port)
			:host_(host), port_(port)
		{
		}

		ListeningServer(const ListeningServer&) = delete;
		ListeningServer& operator=(const ListeningServer&) = delete;

		std::shared_ptr<WebSocket<tcp::socket>> accept();

	private:

		std::vector<std::shared_ptr<WebSocket<tcp::socket>>> websockets;
		net::io_context io_context_{ 1 };
		unsigned short port_;
		std::string host_;
	};
} // RedBack
#endif
