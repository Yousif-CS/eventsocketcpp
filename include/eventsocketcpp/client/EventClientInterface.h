// A client interface that handles one connection

#pragma once

#include <eventsocketcpp/RedBackCommon.h>
#include <eventsocketcpp/RedBackConnection.h>
#include <eventsocketcpp/RedBackMessage.h>
#include <eventsocketcpp/RedBackTSQueue.h>



namespace RedBack
{
    namespace Client
    {
        template<typename T>
        class EventClientInterface
        {
        public:

            explicit EventClientInterface()
            {}

            virtual ~EventClientInterface()
            {   
                disconnect();
            }

            // Connect to a server given the host and port
            // Returns true if the operation is successful
            bool connect(std::string host, uint16_t port)
            {
                try 
                {

                    // Resolves the host to an endpoint
                    boost::asio::ip::tcp::resolver resolver{asioContext};

                    // Resolve and connect
                    resolver.async_resolve(host, std::to_string(port),
                    [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type endpoints){

                        // Create the connection
                        connection = std::make_shared<Connection<T>>(Connection<T>::owner::client, asioContext, qMessagesIn);

                        // Connect to the endpoint
                        connection->connectToServer(endpoints, [this](){ OnConnect(); });

                    });

                    // Start the asynchrononous context
                    threadContext = std::thread([this]{ asioContext.run(); });
                    return true;

                }
                catch(std::exception& e)
                {
                    std::cerr<< "[CLIENT] Exception: " << e.what() << std::endl;
                    return false;
                }
            }

            // Process the received messages
            void update(size_t nMaxMessages = -1, bool bWait = false)
            {
                // Wait until we receive messages
                if (bWait) qMessagesIn.wait();

                // Start reading messages
                for(size_t nMessages = 0; nMessages < nMaxMessages && !qMessagesIn.isEmpty(); nMessages++)
                {
                    OwnedMessage<T> msg = qMessagesIn.front();

                    // Parse the message for specific configurations 
                    // such as a response to create room, forwarded message
                    // It also calls the OnMessage callback after configuration
                    parseConfigs(msg.message);

                    //Pop the message out
                    qMessagesIn.pop_front();
                }
            }

            bool isConnected()
            {
                if (connection)
                {
                    return connection->isConnected();
                }else
                {
                    return false;
                }
                
            }
			
			std::shared_ptr<Connection<T>> GetConnection() const {
				return connection;
			}

            void disconnect()
            {
                connection->disconnect();

                // Stop the context and its thread
                asioContext.stop();
                
                if (threadContext.joinable())
                    threadContext.join();

                connection.reset();
            }

            void send(const Message<T>& msg)
            {
                if (isConnected())
                    connection->send(msg);
            }

            // broadcast message to all connected clients on the server
            void broadcast(Message<T>& msg)
            {
                msg.setConfig(Config::BroadcastAll);

                send(msg);
            }

            // broadcast a message to all clients in a certain room
            void broadcastRoom(Message<T>& msg, uint32_t roomID)
            {
                msg.setConfig(Config::BroadcastRoom);

                msg << roomID;

                send(msg);
                
            }

            // create a room on the server,
            // If the room is created, the OnRoomCreated callback is called
            void createRoom()
            {
                Message<T> msg;
                msg.setConfig(Config::CreateRoom);
                send(msg);
            }

            // join a created room

            void joinRoom(uint32_t roomID)
            {
                Message<T> msg;
                
                msg.setConfig(Config::JoinRoom);

                msg << roomID;

                send(msg);
            }

            // 
            // forward a message to a client with id 
            void forward(Message<T>& msg, uint32_t id)
            {
                msg.setConfig(Config::Forward);
                msg << id;

                send(msg);
            }

            TSQueue<T> incomingMsgs()
            {
                return qMessagesIn;
            }

        protected:
            // A callback to received messages
            void OnMessage(Message<T> msg)
            {
                // Check if there are custom callbacks assigned 
                if (callbacks.count(msg.ID()) != 0)
                {
                    callbacks.at(msg.ID())(msg);
                }
            }

            // Assign a callback to be invoked when a particular event t happens
            void OnEvent(T t, std::function<void(Message<T> msg)> callback)
            {
                callbacks.insert({t, callback});
            }

            // Override: when the connection is disconnected
            virtual void OnDisconnect()
            {

            }

            // Override: when connected
            virtual void OnConnect()
            {

            }


            // Override: when a client forwards a message to you 
            virtual void OnForward(Message<T> msg, uint32_t forwarderID)
            {

            }

            // Override: when a client broadcasts a message
            virtual void OnBroadCast(Message<T> msg, uint32_t broadcasterID)
            {

            }

            // Override: when a room is created
            virtual void OnRoomCreated(uint32_t roomID)
            {

            }

            // Override: when a room is joined
            virtual void OnRoomJoined(uint32_t roomID)
            {

            }

            // Parse the different configurations of the message
            bool parseConfigs(Message<T> msg)
            {
                switch(msg.config())
                {
                    case Config::Broadcasted:
                    {
                        // broadcaster id
                        uint32_t bID;
                        msg >> bID;
                        OnBroadCast(msg, bID);
                        break;
                    }

                    case Config::CreateRoomResponse:
                    {
                        // the room id
                        uint32_t roomID;
                        msg >> roomID;
                        OnRoomCreated(roomID);
                        break;
                    }

                    case Config::OnRoomJoined:
                    {

                        // invoke the callback
                        uint32_t roomID;
                        msg >> roomID;
                        OnRoomJoined(roomID);
                        break;
                    }
                    case Config::Forwarded:
                    {
                        
                        // forwarder id
                        uint32_t fID;
                        msg >> fID;
                        OnForward(msg, fID);
                        break;
                    }
                    default:
                        OnMessage(msg);
                }

            }


            // The context that handles all asynchronous operations
            boost::asio::io_context asioContext;
            
            // The thread that handles running the context
            std::thread threadContext;

            // The current connection object
            std::shared_ptr<Connection<T>> connection;

            private:
                // Contains all incoming messages
                TSQueue<OwnedMessage<T>> qMessagesIn;

                // The map that contains all event callbacks 
                std::map<T, std::function<void(Message<T>)>> callbacks;
        };
    } // Client
} //RedBack
