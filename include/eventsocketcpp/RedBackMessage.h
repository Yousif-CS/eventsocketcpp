// A message type that contains a message as well as a header specifies the type of message it is

#pragma once

#include <eventsocketcpp/RedBackCommon.h>


namespace RedBack {
    template<typename T>
    struct OwnedMessage;

    template<typename T>
    struct MessageHeader;

    template<typename T>
    struct Message;

    struct MetaConfig;

    enum class Config;
}

#include <eventsocketcpp/RedBackConnection.h>


namespace RedBack {
    

    // Holds the config meta information such as broadcaster id, forwarder id, room ids.
    struct MetaConfig {
        uint32_t roomID = 0;
        uint32_t broadcasterID = 0;
        uint32_t forwarderID = 0;
    };

    // All possible configurations to the server/client
    enum class Config {
        Forward, Forwarded, BroadcastAll, BroadcastRoom, Broadcasted, CreateRoom, CreateRoomResponse, JoinRoom, None
    };

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
        
        MessageHeader<T> header {};

        std::vector<unsigned char> body;

        //Returns the size of the message
        size_t size() const
        {
            return body.size();
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
            msg.body.resize(msg.size() + sizeof(DataType));

            // Actually copy the data into our message
            std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

            // Update the size;
            msg.header.size = msg.size();

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
            std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

            // Resize the vector and adjust the header size;
            msg.body.resize(i);
            msg.header.size = msg.size();

            // return the message object for chaining
            return msg;
        } 
        
        // Read the whole message into a string 
        friend Message<T>& operator >> (Message<T>& msg, std::string& data) 
        {
            char tmp[msg.header.size];


            std::memcpy(tmp, msg.body.data(), msg.header.size);
            
            data = std::string(tmp);
            // Resize the vector and adjust the header size;
            msg.body.resize(0);
            msg.header.size = 0;

            // You cannot chain this operation, so we do not return a string
        }   
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