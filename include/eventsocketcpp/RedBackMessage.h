// A message type that contains a message as well as a header specifies the type of message it is
#include <sstream>
#include <fstream>
#include <iostream>

#ifndef REDBACK_MESSAGE_H
#define REDBACK_MESSAGE_H

#include <eventsocketcpp/RedBackCommon.h>

//forward declare 
namespace RedBack {
	template<typename T>
	struct MessageHeader;

	template<typename T>
	struct Message;

	template<typename T>
	struct OwnedMessage;
	
	enum class Config {
		Forward, Forwarded, BroadcastAll, BroadcastRoom, Broadcasted,
		CreateRoom, CreateRoomResponse, JoinRoom, OnRoomJoined,
		None
	};
}

#include "eventsocketcpp/RedBackConnection.h"

namespace RedBack {

    // A header which contains what type of message it is
    // as well as the size of the body
    template<typename T>
    struct MessageHeader{

        T id {};
        Config config {Config::None};
        uint32_t size = 0;

    };
	

	class MessageBody {
	public:
		
		MessageBody();

        // Copy constructor
		MessageBody(const MessageBody&);

        // Copy assignment
        MessageBody& operator=(const MessageBody&);

		~MessageBody();

		void setID(uint32_t id);

		void setConfig(uint32_t conf);

		bool SerializeToString(std::string* output);

		bool ParseFromString(const std::string& data);
		
		uint32_t header_id();

		uint32_t header_config();

		uint32_t header_size();

		void set_header_size(uint32_t size);
	
        const std::string& body() const;

        std::string * mutable_body();

		
		void set_body(std::string body);

        void set_body(const void * buffer, size_t size);

	private:
		class MessageBodyImp;
		std::unique_ptr<MessageBodyImp> messageBodyImp;
	};
	
    template<typename T>
    struct Message {
        

        Message(T id)
        :Message()
        {    
            messageImp.setID(static_cast<uint32_t>(id));
        }

        Message(){
			messageImp.setConfig(static_cast<uint32_t>(Config::None));
			messageImp.set_body("");
		}
		
        // Copy assignment
        Message& operator=(const Message&) = default;

        // Copy constructor
        Message(const Message&) = default;
        
        // Getters
        T ID()
        {
            return header.id;
        }

        Config config()
        {
            return header.config;
        }

        // Setters
        void setID(T id)
        {
            header.id = id;
            messageImp.setID(static_cast<uint32_t>(id));
        }

        void setConfig(Config conf)
        {
            header.config = conf;
            messageImp.setConfig(static_cast<uint32_t>(conf));
        }

        bool SerializeToString(std::string * output)
        {
            return messageImp.SerializeToString(output);
        }

        bool ParseFromString(const std::string& data)
        {
            bool ret = messageImp.ParseFromString(data);

            // Crude error check :)
            if (!ret)
                return ret;

            // update the header information
            header.id = T(messageImp.header_id());
            header.config = Config(messageImp.header_config());
            header.size = messageImp.header_size();

            return true;
        }

        //Returns the size of the message
        size_t size()
        {
            return messageImp.body().size();
        }

        //Print out the message to stdout
        friend std::ostream& operator<< (std::ostream& os, const Message<T>& msg)
        {
            os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
            return os;
        }

        //Read data from any datatype into our message object
        template<typename DataType>
        friend Message<T>& operator<< (Message<T>& msg, const DataType& data)
        {

            // Check that the data is not too complex to copy
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be dumped into message buffer");

            // Keep track of the previous size as we will use this to insert data
            size_t i = msg.size();
            
            // a temporary buffer
            std::vector<uint8_t> bytes(msg.messageImp.body().begin(), msg.messageImp.body().end());
            bytes.resize(i + sizeof(DataType));

            memcpy(bytes.data() + i, &data, sizeof(DataType));

            msg.messageImp.set_body(bytes.data(), bytes.size());

			// Update the size;
            msg.header.size = msg.messageImp.body().size();

            // To chain the operation
            return msg;
        }

        // Reading a string into the message object
        // Here we assume we cannot store a mixture of strings and other datatypes
        // Therefore we need to wipe the whole buffer 
        friend Message<T>& operator<< (Message<T>& msg, const std::string& data)
        {
            msg.messageImp.set_body(data);
            msg.header.size = msg.messageImp.body().size();
        }

        //Dump the buffer data into a datatype of choice;
        template<typename DataType>
        friend Message<T>& operator >> (Message<T>& msg, DataType& data) 
        {
            // Check that the data is not too complex to copy
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be dumped into message buffer");

            // Save the location which points to the start of the memory to be read
            int i = msg.size() - sizeof(DataType);

            if (i < 0)
                throw std::runtime_error("There is not enough data to read");

            std::memcpy(&data, msg.messageImp.body().data() + i, sizeof(DataType));

            // Resize the vector and adjust the header size;
            msg.messageImp.mutable_body()->resize(i);
            msg.header.size = i;
            msg.messageImp.set_header_size(msg.header.size);
            // return the message object for chaining
            return msg;
        } 
        
        // Read the whole message into a string 
        friend Message<T>& operator >> (Message<T>& msg, std::string& data) 
        {
            
            data.resize(msg.header.size);
            
            std::memcpy(&data[0], msg.messageImp.body().data(), msg.header.size);
            
            // reset the message buffer            
            msg.messageImp.set_body(std::string());
            msg.header.size = 0;

            // You cannot chain this operation, so we do not return the message object
        }
    private:
        MessageHeader<T> header {};
		MessageBody messageImp;
	};

    // An owned message is a message that belongs to a certain connection.
    // This is needed because now we can have one queue for all the messages received
    // and we can process it all on one thread

    //Forward declare class
    template<typename T>
    class Connection;

    template<typename T>
    struct OwnedMessage {
        Message<T> message;
        std::shared_ptr<Connection<T>> owner = nullptr;

        OwnedMessage(std::shared_ptr<Connection<T>> _owner, Message<T> _message)
        :message(_message), owner(_owner)
        {}
        
        // A wrapper for writing the message to stdout
        friend std::ostream& operator<< (std::ostream& os, const OwnedMessage& msg)
        {
            os << msg.message;

            return os;
        }
    };

} // RedBack

#endif
