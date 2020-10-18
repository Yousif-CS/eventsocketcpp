#ifndef EVENTSOCKETH
#define EVENTSOCKETH

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <exception>
#include <map>

#include "jsoncpp/json/json.h"
#include "jsoncpp/json/value.h"

namespace RedBack {

	// An event based socket that allows to create new events and assign callbacks to them
	template<typename T>
	class EventSocket: public std::enable_shared_from_this<EventSocket<T>> {
	public:
		EventSocket(T& t)
		:t_{t}
		{
			default_callback_ = t.get_on_receive_callback();
			t.set_on_receive([this](std::string payload) {
				event_forward(payload);
			});
		}
		
		/*
		Set a callback to be called when the specified event is received
		*/
		void on_event(std::string eventName, std::function<void(std::string)> callback)
		{
			eventCallbacks_.insert(std::pair<std::string, std::function<void(std::string)>>(eventName, callback));
		}
		
		void set_default_callback(std::function<void(std::string)> callback)
		{
			default_callback_ = callback;
		}


		/*
		Emit an event with payload 
		*/
		void emit_event(std::string eventName, std::string payload){
			Json::Value root;
			root["eventName"] = eventName;
			root["payload"] = payload;
			Json::FastWriter writer;

			t_.send(writer.write(root));

		}

	private:

		// Forwards an event to the queue that callbacks
		//are listening on
		void event_forward(std::string payload) {
			Json::Value root;
			Json::Reader reader;
			//if its not a json object or the event is not found, use the default callback
			if (!reader.parse(payload.c_str(), root)) {

				default_callback_(payload);	
			
			} else if (eventCallbacks_.count(root["eventName"].asString()) == 0) {
				default_callback_(root["payload"].asString());
			
			} else {
			
				eventCallbacks_.at(root["eventName"].asString())(root["payload"].asString());
			
			}
#ifdef REDBACK_DEBUG
			std::cout << root["eventName"].asString() << std::endl;
#endif //REDBACK_DEBUG

		}

		std::map<std::string, std::function<void(std::string)>> eventCallbacks_;
		
		std::function<void(std::string)> default_callback_;

		T& t_;

	};
} // RedBack

#endif