// Implementation of the client websocket

#include <iostream>
#include <thread>

#include "eventsocketcpp/client/CustomWebSocketClient.h"

#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core/buffers_to_string.hpp"

namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

namespace RedBack {
    namespace Client {
		boost::asio::io_context ioc;

        void WebSocket::configure(){

            // Default behavior is nothing
            set_on_receive([](std::string payload){
#ifdef REDBACK_DEBUG
                std::cout << "Received: " << std::endl;
#endif //REDBACK_DEBUG
            });

            // create the io context object which is crucial to the creation of websocket streams
            ws_ = std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(ioc);

        }

        void WebSocket::connect(std::string host, unsigned short port){
            boost::asio::ip::tcp::resolver resolver{RedBack::Client::ioc};

            // Do a host search
            auto const results = resolver.resolve(host, std::to_string(port));
            
            // Connect to the resolved host
            auto ep = boost::asio::connect(ws_->next_layer(), results);

            // Host HTTP header during the WebSocket handshake.
            // See https://tools.ietf.org/html/rfc7230#section-5.4
            host += ':' + std::to_string(ep.port());
  
            // Set a decorator to change the User-Agent of the handshake
            ws_->set_option(websocket::stream_base::decorator(
                  [](websocket::request_type& req)
                    {
                    req.set(boost::beast::http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            "RedBack-Client 1.0");
                    }));


            // Perform the websocket handshake
            ws_->handshake(host, "/");

            // Start listening 
            std::thread(&WebSocket::run, this, std::move(exitSignal_.get_future())).detach();
        }

        void WebSocket::run(std::future<void> exitFuture){
            try {
				while(exitFuture.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
					beast::flat_buffer buffer;
					ws_->text(ws_->got_text());
					ws_->async_read(buffer, [this, &buffer](error_code const& ec, std::size_t nbytes){
                        if (ec){
							std::cerr << "Could not read payload" << std:: endl;
							return;
						}
						receive_callback_(buffers_to_string(buffer.data()));
					});
				}
			}
			catch (beast::system_error const& e) {
				std::cerr << "Error at reading payload: " << e.what() << std::endl;
			}
			catch (std::exception const& stder) {
				std::cerr << "Error: " << stder.what() << std::endl;
			}
        }

        void WebSocket::send(std::string payload){
            ws_->write(boost::asio::buffer(payload));
        }
    } // Client
} // RedBack
