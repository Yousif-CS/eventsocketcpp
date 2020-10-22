// A websocket client class

#include "boost/beast/core.hpp"
#include "boost/beast/websocket.hpp"

using tcp = boost::asio::ip::tcp;
namespace net = boost::asio;
using namespace boost::beast::websocket;

#include <future>

namespace RedBack {
    namespace Client {

        class WebSocket: public std::enable_shared_from_this<WebSocket>{
        public:
            
            WebSocket(std::string host, unsigned short port){
                configure();
                connect(host, port);
            }

			// Send payload to the other end 
			void send(std::string payload);

            void set_on_receive(std::function<void(std::string payload)> callback){
                receive_callback_ = callback;
            }

            std::function<void(std::string)> get_on_receive_callback(){ return receive_callback_; }

            void close() { ws_->close(boost::beast::websocket::close_code::normal); }

            ~WebSocket() {
            }

        private:

			// Setup the callbacks
			void configure();
            
            // Resolve host and connect
            void connect(std::string host, unsigned short port);

			// Run by a seperate thread to keep listening for messages
			void run();

			std::function < void(std::string payload) > receive_callback_;
			std::unique_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws_;
            
        };
    } // Client
} // RedBack
