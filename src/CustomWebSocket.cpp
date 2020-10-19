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

	template<typename T>
	WebSocket<T>::WebSocket(T t) {
		configure(std::move(t));
		//std::thread(&WebSocket<T>::start, this, std::move(t)).detach();
	}

	template<typename T>
	void WebSocket<T>::send(std::string payload) {
		ws_->write(boost::asio::buffer(payload));
	}

	template<typename T>
	void WebSocket<T>::log(websocket::frame_type ft, boost::beast::string_view payload) {
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
	}

	template<typename T>
	void WebSocket<T>::configure(T t) {
		try {

			ws_ = std::make_unique<websocket::stream<T>>(std::move(t));
			
			// Default behaviour: echo back payload
			set_on_receive([this](std::string payload) {
#ifdef REDBACK_DEBUG
				std::cout << "Received: " << payload << std::endl;
#endif // _REDBACK_MEDIA_DEBUG

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
#endif // _REDBACK_MEDIA_DEBUG

			ws_->accept();
			reading_thread_ = std::make_unique<std::thread>(&WebSocket<T>::run, this);
			reading_thread_->detach();
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
	void WebSocket<T>::start(T t) {
		configure(std::move(t));
		run();
	}

	template<typename T>
	void WebSocket<T>::run() {

		try {
			for (;;) {
				beast::flat_buffer buffer;
				ws_->text(ws_->got_text());
				ws_->read(buffer);
				receive_callback_(buffers_to_string(buffer.data()));
			}
		}
		catch (beast::system_error const& e) {
			std::cout << "Error at reading payload: " << e.what() << std::endl;
		}
		catch (std::exception const& stder) {
			std::cout << "Error: " << stder.what() << std::endl;
		}
	}
} //RedBack

template class RedBack::WebSocket<tcp::socket>;
