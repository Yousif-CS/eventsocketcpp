#ifndef CUSTOMWEBSOCKETH
#define CUSTOMWEBSOCKETH

#include <thread>

#include "boost/beast/websocket.hpp"
#include "boost/beast/core.hpp"

namespace RedBack {

	/*
	 A wrapper around boost::websocket that simplifies it use
	*/
	template <typename T>
	class WebSocket: public std::enable_shared_from_this<WebSocket<T>>{

	public:
		// Create a websocket from an existing websocket wrapper 
		WebSocket(T t);

		// Send payload to the other end 
		void send(std::string payload);

		// Set a handler when the socket receives data
		void set_on_receive(std::function<void(std::string payload)> callback) { receive_callback_ = callback; }

		// Get the receive callback function
		std::function<void(std::string)> get_on_receive_callback() { return receive_callback_; }

		// Get a shared pointer to this object
		std::shared_ptr<WebSocket<T>> getPtr() {
			return this->shared_from_this();
		}

		// Close the connection
		void close() { ws_->close(boost::beast::websocket::close_code::normal); }

		virtual ~WebSocket() { }

	private:
		void log(boost::beast::websocket::frame_type, boost::beast::string_view);
		void configure(T t);
		// Start the websocket and setup for receiving messages
		void start(T t);
		void run();
		std::function < void(std::string payload) > receive_callback_;
		std::unique_ptr<boost::beast::websocket::stream<T>> ws_;

		//the thread that keeps reading data from the other end
		std::unique_ptr<std::thread> reading_thread_ = nullptr; 
	};

} // RedBack
#endif