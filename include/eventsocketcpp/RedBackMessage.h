// A message type that contains a message as well as a header specifies the type of message it is

#include <eventsocketcpp/RedBackCommon.h>
//forward declare 
namespace RedBack {
	template<typename T>
	struct MessageHeader;

	template<typename T>
	struct Message;

	template<typename T>
	struct OwnedMessage;
	
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


    template<typename T>
    struct Message {
        
        Message()
        {
            messageImp.mutable_header()->set_config(static_cast<uint32_t>(header.config));
        }

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
            messageImp.mutable_header()->set_id(static_cast<uint32_t>(id));
        }

        void setConfig(Config conf)
        {
            header.config = conf;
            messageImp.mutable_header()->set_config(static_cast<uint32_t>(conf));
        }

        bool SerializeToString(std::string * output)
        {
            return messageImp.SerializeToString(output);
        }

        bool ParseFromString(const std::string& data)
        {
            messageImp.ParseFromString(data);
            // update the header information
            header.id = T(messageImp.header().id());
            header.config = Config(messageImp.header().config());
            header.size = messageImp.header().size();
        }

        //Returns the size of the message
        size_t size() const
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

            // Resize the vector to contain new data
            msg.messageImp.mutable_body()->resize(msg.size() + sizeof(DataType));

            // Actually copy the data into our message
            std::vector<char> dest;
            std::copy(msg.messageImp.body().begin(), msg.messageImp.body().end(), std::back_inserter(dest));

            std::memcpy(dest.data() + i, &data, sizeof(DataType));

            std::string payload{dest.data()};
            msg.messageImp.set_body(std::string(dest.data()));

			// Update the size;
            msg.header.size = msg.messageImp.body().size();
            msg.messageImp.mutable_header()->set_size(msg.header.size);

            // To chain the operation
            return msg;
        }

        //Dump the buffer data into a datatype of choice;
        template<typename DataType>
        friend Message<T>& operator >> (Message<T>& msg, DataType& data) 
        {
            // Check that the data is not too complex to copy
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be dumped into message buffer");

            // Save the location which points to the start of the memory to be read
            size_t i = msg.size() - sizeof(DataType);

            // Read the data into the variable
            std::memcpy(&data, msg.messageImp.body().data() + i, sizeof(DataType));

            // Resize the vector and adjust the header size;
            //std::vector<char> newBody;
            //std::copy(msg.messageImp.body().begin(),msg.messageImp.body().begin() + i, std::back_inserter(newBody)); 
            msg.messageImp.mutable_body()->resize(i);
            msg.header.size = i;
            msg.messageImp.mutable_header()->set_size(msg.header.size);
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
            msg.messageImp.mutable_header()->set_size(0);

            // You cannot chain this operation, so we do not return a string
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
