//A server interface to handle multiple incoming connections
// This server is asynchronous; meaning it runs on the io_context 
// which also means there are no extra threads involved per new connection

#include <eventsocketcpp/RedBackCommon.h>
#include <eventsocketcpp/RedBackMessage.h>
#include <eventsocketcpp/RedBackTSQueue.h>
#include <eventsocketcpp/RedBackConnection.h>

#pragma once

namespace RedBack 
{
    namespace Server
    {
        template<typename T>
        class EventServerInterface
        {
        
        public:

            explicit EventServerInterface(uint16_t port)
            :asioAcceptor(asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
            {}

            //TODO:
            virtual ~EventServerInterface()
            {}

            // Start accepting connections Asynchronously
            bool start()
            {
                try
                {
                    // Start waiting for client connections
                    // asynchronously
                    waitForConnections();
                    
                    // Start the context to perform the tasks
                    threadContext = std::thread([this]() { asioContext.run(); });

                    return true;
                }
                catch(std::exception& e)
                {
                    std::cerr << "[SERVER] Exception: " << e.what() << std::endl;
                    return false;
                }
            }

            //Stop the current server
            void stop()
            {
                asioContext.stop();

                if (threadContext.joinable()) threadContext.join();

            }
            

            //Async: Try to accept incoming connections
            void waitForConnections()
            {
                
                asioAcceptor.async_accept( 
                    [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
                    {
                        if (!ec)
                        {
                            // Connection request
                            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

                            std::shared_ptr<Connection<T>> connection = std::make_shared<Connection<T>>(Connection<T>::owner::server, asioContext, std::move(socket), qMessagesIn);
                            // Check whether the user wants to accept connections
                            
                            // Give them an id and if the user does not want to accept the connection, take it back
                            connection->SetID(globalID);

                            if (OnConnect(connection))
                            {
                                // Add the connection to our list of connections
                                deqConnections.push_front(connection);

                                //perform handshake and assign them an id
                                connection->handshake(globalID);

                                globalID++;
                            }
                            else
                            {   
                                std::cerr << "[-----]" << " Connection refused." << std::endl;
                            }
                            
                        }
                        else 
                        {
                            std::cerr << "[SERVER] Error Accepting Connection." << std::endl;
                        }

                    });
            }

            void MessageClient(std::shared_ptr<Connection<T>> connection, const Message<T>& msg)
            {
                //If the client is not connected, we dispose of them
                if (connection->isConnected())
                {
                    connection->send(msg);
                }
                else
                {
                    OnDisconnect(connection);

                    //Release associated resources
                    
                    //Release associated event callbacks, if any
                    std::lock_guard<std::mutex> lock{callbackMux};
                    if (callbacks.find(connection->GetID()) != callbacks.end())
                    {
                        callbacks.erase(connection->GetID());
                    }
                    
                    //Remove the client from any room he is in.
                    removeParticipant(connection);

                    //Remove connection from queue
                    connection.reset();

                    deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), connection), deqConnections.end());
                }
                
            }

            void MessageRoom(const Message<T>& msg, uint32_t roomID, std::shared_ptr<Connection<T>> except)
            {
                // checks if an invalid client (disconnected exists)
                bool bDisconnectedClient = false;

                std::lock_guard<std::mutex> lock{roomMux};

                // check if roomID actually exists
                if (rooms.find(roomID) == rooms.end())
                    return;                

                // Message the clients
                for(std::shared_ptr<Connection<T>> participant: rooms.at(roomID))
                {
                    if (participant && participant->isConnected())
                    {
                        // Do not message the client that sent the message
                        if (participant != except)
                            participant->send(msg);  
        
                    }
                    else
                    {
                        // Flag this client as disconnected
                        // Perform the on disconnect callback
                        OnDisconnect(participant);

                        participant.reset();

                        bDisconnectedClient = true;
                    }
                }
                // Remove all disconnected clients from the room, and delete the room if empty

                rooms[roomID].erase(nullptr);

                //rooms[roomID].erase(std::remove(rooms[roomID].begin(), rooms[roomID].end(), nullptr), rooms[roomID].end());

                if (rooms.at(roomID).empty())
                {
                    rooms.erase(roomID);
                }
            }
            // Message all clients except (possibly) a certain one
            void MessageAllClients(const Message<T>& msg, std::shared_ptr<Connection<T>> except = nullptr)
            {
                bool bInvalidClientExists = false;


                // Send to all clients
                for(auto& client: deqConnections)
                {
                    if (client && client->isConnected())
                    {
                        if (client != except)
                        {
                            client->send(msg);
                        }
                    }
                    else
                    {
                        // Remove the client; possibly disconnected
                        OnDisconnect(client);

                        client.reset();
                        
                        // So that we later can remove them
                        bInvalidClientExists = true;    
                    }
                    
                }

                if (bInvalidClientExists)
                {
                    deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), nullptr), deqConnections.end());
                }

            }
            
            // Force all messages to be processed
            void update(size_t nMaxMessages = -1, bool bWait = false)
            {
                if (bWait) qMessagesIn.wait();

                //process at most nMaxMessages, and check if there are special requests
                for(size_t nMessages = 0; nMessages < nMaxMessages && !qMessagesIn.isEmpty(); nMessages++)
                {
                    OwnedMessage<T> ownedmsg = qMessagesIn.front();

                    //Parse requests, if any, 
                    parseRequests(ownedmsg);

                    // Perform callback, if any (for example to invoke certain events)
                    OnMessage(ownedmsg.owner, ownedmsg.message);

                    qMessagesIn.pop_front();
                }
            }

            // Assign a callback to a specific connection which is called upon receiving a certain event  
            bool OnEvent(T eventid, std::shared_ptr<Connection<T>> connection, std::function<void(Message<T>)> callback)
            {
                std::lock_guard<std::mutex> lock{callbackMux};

                uint32_t id = connection->GetID();
                // This connection has no previously assigned events
                if (callbacks.find(connection->GetID()) == callbacks.end())
                {
                    //Create a map containing all of the events of that specific connection
                    std::map<T,std::function<void(Message<T>)>> events;
                    
                    //insert the event and callback
                    events.insert({eventid, callback});
                    
                    //insert that event map into our connection-event map
                    callbacks.insert({connection->GetID(), events});

                }
                else
                {
                    // insert the callback
                    callbacks.at(connection->GetID()).insert({eventid, callback});
                }
                return true;
            }

        protected:

            // When a new connection is requested, you can
            // override this function to perform some custom operation.
            // Return true if you want the connection
            virtual bool OnConnect(std::shared_ptr<Connection<T>> connection)
            {
                return true;
            }

            // Called when a connection is disconnected.
            // Override to customize
            virtual bool OnDisconnect(std::shared_ptr<Connection<T>> connection)
            {

            }

            virtual bool OnMessage(std::shared_ptr<Connection<T>> connection, Message<T> msg)
            {   

                //Acquire a lock to the events map
                std::lock_guard<std::mutex> lock{callbackMux};
                
                uint32_t id = connection->GetID();
                //Check if a certain callback has been assigned to the message header event
                if (callbacks.find(connection->GetID()) != callbacks.end())
                {
                    auto events = callbacks.at(connection->GetID());
                    
                    if (events.find(msg.header.id) != events.end())
                    {
                        events.at(msg.header.id)(msg);
                    }
                }
                return true;
            }

            // parse and perform the requests
            // Changes the message object as some requests
            // store some data in the body required to process
            // them
            virtual void parseRequests(OwnedMessage<T>& owned_msg)
            {
                switch(owned_msg.message.header.config)
                {
                    case Config::BroadcastAll:
                    {

                        // It is important to set the request to NONE
                        // so that the message does not keep getting
                        // broadcasted
                        owned_msg.message.header.config = Config::Broadcasted;
                        owned_msg.message << owned_msg.owner->GetID();
                        MessageAllClients(owned_msg.message, owned_msg.owner);
                        break;
                    
                    }
                    case Config::CreateRoom:
                    {
                        
                        // Create a room and send back the room id
                        // So that the client can then forward
                        // it to other clients to join it
                        Message<T> msg;
                        msg.header = owned_msg.message.header; 
                        msg.header.config = Config::CreateRoomResponse;
                        msg << createRoom(owned_msg.owner);
                        MessageClient(owned_msg.owner, msg);
                        break;

                    }
                    // TODO: Broadcast to a room
                    case Config::BroadcastRoom:
                    {
                        //Set the flag as broadcasted
                        owned_msg.message.header.config = Config::Broadcasted;

                        // Get the room id from the payload
                        uint32_t roomID;
                        owned_msg.message >> roomID;

                        // Add the broadcaster id to the payload
                        owned_msg.message << owned_msg.owner->GetID();
                        
                        // sent to all room participants
                        MessageRoom(owned_msg.message, roomID, owned_msg.owner);

                        break;
                    }

                    // TODO: Join a room
                    case Config::JoinRoom:
                    {
                        // Get the room id
                        uint32_t roomID; 
                        owned_msg.message >> roomID;
                        addToRoom(roomID, owned_msg.owner);
                        break;

                    }

                    // TODO: Forward payload to a client
                    case Config::Forward:
                    {

                        // Get the id of the recipient
                        uint32_t recpID;
                        owned_msg.message >> recpID;
                        
                        //Set the flag to be forwarded;
                        owned_msg.message.header.config = Config::Forwarded;

                        //Sent to client
                        MessageClient(getConnection(recpID), owned_msg.message);
                        break;
                    }

                    // Default, there are no suitable requests, just return
                    default:
                        return;
                }
            }
            boost::asio::io_context asioContext;
            std::thread threadContext;

            boost::asio::ip::tcp::acceptor asioAcceptor;
            
            //All current connection
            std::deque<std::shared_ptr<Connection<T>>> deqConnections;

            //All unique global ids start from this
            uint32_t globalID = 10000;
            
            //Current room id 
            uint32_t _roomID = 20000;

            // A queue to hold all current owned messages
            // yet to be processed
            TSQueue<OwnedMessage<T>> qMessagesIn;

            // A map to hold the callbacks to events and connections
            std::map<uint32_t, std::map<T, std::function<void(Message<T>)>>> callbacks;

            // A map to hold all the rooms and all the clients connected to it
            std::map<uint32_t, std::set<std::shared_ptr<Connection<T>>>> rooms;

        private:

            // Create a new room, add the creator and return its id
            uint32_t createRoom(std::shared_ptr<Connection<T>> connection)
            {
                std::lock_guard<std::mutex> lock{roomMux};

                rooms.insert({_roomID, std::set<std::shared_ptr<Connection<T>>>()});

                uint32_t oldID = _roomID++;

                addToRoom(oldID, connection);

                return oldID;
            }

            // Add a new connection to the room
            void addToRoom(uint32_t rID, std::shared_ptr<Connection<T>> connection)
            {
                std::lock_guard<std::mutex> lock{roomMux};

                //no room to add to 
                if (rooms.find(rID) == rooms.end())
                    return;

                rooms.at(rID).insert(connection);
            }

            // Get the connection from the id
            std::shared_ptr<Connection<T>> getConnection(uint32_t id)
            {
                auto it = std::find_if(deqConnections.begin(), deqConnections.end(), 
                    [this, id](std::shared_ptr<Connection<T>> conn){
                        if (conn->GetID() == id)
                            return true;
                        return false;
                    });
                
                // Connection not found
                if (it == deqConnections.end())
                    return nullptr;
             
                return *it;
            }

            // Remove the connection from any rooms he is in
            void removeParticipant(std::shared_ptr<Connection<T>> conn)
            {
                std::lock_guard<std::mutex> lock{roomMux};

                
                for(auto it = rooms.begin(); it != rooms.end() ; )
                {
                    it->second.erase(conn);

                    //delete the set all together if it is empty
                    if (it->second.empty())
                    {
                        rooms.erase(it);
                    }else
                    {
                        ++it;
                    }
                    
                }
            }

            // A mutex to access the event map;
            std::mutex callbackMux;
            std::mutex roomMux;

        };
    } //Server

} //RedBack
