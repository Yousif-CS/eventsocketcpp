#include <iostream>
#include <cstdlib>
#include <functional>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core/buffers_to_string.hpp"
#include "eventsocketcpp/server/CustomWebSocket.h"

namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;
using tcp = boost::asio::ip::tcp;

namespace RedBack {

	namespace Server {

		template<typename T>
		WebSocket<T>::WebSocket(T t) {
			configure(std::move(t));
		}

		template<typename T>
		void WebSocket<T>::send(std::string payload) {
			ws_->write(boost::asio::buffer(payload));
		}

		template<typename T>
		void WebSocket<T>::log(websocket::frame_type ft, boost::beast::string_view payload) {

	#ifdef REDBACK_DUBUG
			switch (ft) {
				case websocket::frame_type::close:
					std::cout << "Connection Closed: " << payload << std::endl;
					break;
				case websocket::frame_type::ping:
					std::cout << "Ping Received: " << payload << std::endl;
					break;
				case websocket::frame_type::pong:
					std::cout << "Pong Received" << payload << std::endl;
			}
	#endif // REDBACK_DEBUG
		}

		template<typename T>
		void WebSocket<T>::configure(T t) {
			try {

				ws_ = std::make_unique<websocket::stream<T>>(std::move(t));
				
				// Default behaviour: echo back payload
				set_on_receive([this](std::string payload) {
	#ifdef REDBACK_DEBUG
					std::cout << "Received: " << payload << std::endl;
	#endif // _REDBACK_DEBUG

					send(payload);
				});

				ws_->set_option(websocket::stream_base::decorator(
					[](websocket::response_type& res) {
						res.set(http::field::server, "RedBack-Media-Server 1.0");
					}
				));
				// Debug purposes
	#ifdef REDBACK_DEBUG
				ws_->control_callback(
					[this](websocket::frame_type ft, boost::beast::string_view payload) {
						log(ft, payload);
					}
				);
	#endif // _REDBACK_DEBUG

				ws_->accept();

				std::thread(&WebSocket<T>::run, this, std::move(exitSignal_.get_future())).detach();
			}

			catch (beast::system_error const& e) {
				if (e.code() != websocket::error::closed)
					std::cerr << "Error Establishing Websocket: " << e.what() << std::endl;
			}
			catch (std::exception const& e) {
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}

		template<typename T>
		void WebSocket<T>::run(std::future<void> exitFuture) {

			try {
				while(exitFuture.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
					beast::flat_buffer buffer;
					ws_->text(ws_->got_text());
					ws_->read(buffer);
					receive_callback_(buffers_to_string(buffer.data()));
				}
			}
			catch (beast::system_error const& e) {
				std::cerr << "Error at reading payload: " << e.what() << std::endl;
			}
			catch (std::exception const& stder) {
				std::cerr << "Error: " << stder.what() << std::endl;
			}
		}

	} // Server
} // Server
template class RedBack::Server::WebSocket<tcp::socket>;
