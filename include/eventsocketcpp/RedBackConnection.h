// A connection class that contain vital information about the websocket connection
// and provides important operations of sending/receiving payload

// Important note: some operations provided are async which means
// they need to be handled by underlying context object that is
// passed down by the main client/server interface.

#pragma once

// forward declaration because of dependencies
namespace RedBack {
    template<typename T>
    class Connection;
}


#include <eventsocketcpp/RedBackCommon.h>
#include <eventsocketcpp/RedBackMessage.h>
#include <eventsocketcpp/RedBackTSQueue.h>
#include <eventsocketcpp/server/EventServerInterface.h>

namespace RedBack {

    template<typename T>
    class Connection: public std::enable_shared_from_this<Connection<T>>
    {
        public:
            
            // Specifies the type of the connection
            enum class owner {
                server,
                client
            };

            // For server interfaces
            Connection(owner parent, boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket socket, TSQueue<OwnedMessage<T>>& qIn)
            :asioContext(ioContext), qMessagesIn(qIn), ws(std::move(socket))
            {

                if (parent == owner::server)
                {
                    // Set suggested timeout settings for the websocket
                    ws.set_option(
                        websocket::stream_base::timeout::suggested(
                            beast::role_type::server));
                }
                ws.text(false);
                ownerType = parent;
            }


            // For client intefaces
            Connection(owner parent, boost::asio::io_context& ioContext, TSQueue<OwnedMessage<T>>& qIn)
            :asioContext(ioContext), qMessagesIn(qIn), ws(boost::asio::make_strand(ioContext))
            {
                if (parent == owner::client)
                {
                    // Set suggested timeout settings for the websocket
                    ws.set_option(
                        websocket::stream_base::timeout::suggested(
                        beast::role_type::client));
                }
                ws.text(false);
                ownerType = parent;
            }
            
            virtual ~Connection()
            {
                disconnect();
            }
            
            // A unique id for clients so that other clients can reference them
            uint32_t GetID() const
            {
                return id;
            }

            // Set their id
            void SetID(uint32_t _id) { id = _id; }

            // Connect to a server given the hostname and the port.
            void connectToServer(boost::asio::ip::tcp::resolver::results_type& endpoints, std::string urlPath, std::function<void(void)> clientOnConnect){
                
                // Does not make sense for a server to connect to another 
                if (ownerType == owner::server){
                    return;
                }

                // To connect to a specific url path
                _urlPath = urlPath;

                // Set the timeout for the operation
                beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

                //connect the lowest level (tcp_stream) of the websocket to the endpoint
                boost::beast::get_lowest_layer(ws).async_connect(endpoints,
                        beast::bind_front_handler(
                            &Connection<T>::OnConnect,
                            this->shared_from_this(), clientOnConnect));

            }

            // Connect to a client to start reading
            void startReading(){
                
                if (ownerType == owner::client)
                    return;

                if (ws.is_open()){

                    read();
                }

            }

            bool isConnected() const {
                
                return ws.is_open();

            }

            void disconnect(){

                if (isConnected())
                    boost::asio::post(asioContext, [this](){ ws.close(websocket::close_code::normal); });
            }
            
            // Asynchronously send a message using io_context
            void send(const Message<T>& msg)
            {

                boost::asio::post(asioContext, [this, msg](){
                    
                    bool isWriting = !qMessagesOut.isEmpty();

                    qMessagesOut.push_back(msg);

                    if (!isWriting)
                    {
                        write();
                    }
                });
            }

            // Asynchronously perform handshake as a server
            // ID: the id of the newly connected client
            void handshake(Server::EventServerInterface<T> * serverInterface)
            {
                if (ownerType == owner::client)
                    return;

                // Set the host field of the handshake request before accepting it
                ws.set_option(websocket::stream_base::decorator(
                    [](websocket::response_type& res) {
                        res.set(http::field::server, "RedBack-Event-Server 1.0");
                    }
                ));

                // Accept the handshake upgrade request received from the client
                ws.async_accept(beast::bind_front_handler(&Connection<T>::OnAccept, this->shared_from_this(), serverInterface));
            }

        protected:

            //A websocket connection object representing an endpoint
            websocket::stream<tcp_stream> ws;
            
            // A reference to the context which runs all of our asynchronous code
            boost::asio::io_context& asioContext;

            // This thread safe queue holds all messaging that need to be written to the
            // end point
            TSQueue<Message<T>> qMessagesOut;
            
            // This is the incoming queue of the parent object (server/client interface)
            // that handles all connections
            TSQueue<OwnedMessage<T>>& qMessagesIn;

            //A temporary object to store the message before it is constructed
            // i.e. when we are ready to construct an owned message and push it to the 
            // out queue for the server/client interface
            Message<T> msgTemporaryIn;

            // The owner type of the connection which helps us decide
            // which operations are needed based on the interface (client or server)
            // By default, it is a server
            owner ownerType = owner::server;
            
            // A unique id for each client
            uint32_t id = 0;
        
            // Async functions: run on the io_context

            // Perform handshake as a client
            void handshake(std::function<void(void)> clientOnConnect, std::string hostname){
                
                if (ownerType == owner::server)
                    return;
                    
                ws.async_handshake(hostname, _urlPath,
                [this, clientOnConnect](std::error_code ec){
                    
                    if (!ec){

                        // Now that we are connected, 
                        // invoke the OnConnect callback
                        clientOnConnect();
                        //start reading
                        read();
                    }
                
                });

            }

        private:

            // Async functions

            // Reads the header when an incoming message arrives, and 
            // resizes the message buffer to accomodate the payload
            void read(){
                
                ws.async_read(_buffer,
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec) {

                            // Deserialize the data structure
                            bool ret = msgTemporaryIn.ParseFromString(buffers_to_string(this->_buffer.data()));

                            // Error parsing the string
                            if (!ret)
                            {
                                std::cerr << "[" << id << "]" << " Error Parsing Message." << std::endl;
                                return;
                            }

                            addToIncomingMsgQueue();
                        }
                        else
                        {
                            // Read failed
                            std::cerr << "[" << id << "]" << " Reading Header Failed" << std::endl;
                            // Only if it is not connected do we try and close it;
                            if (!isConnected())
                                disconnect();
                        }
                    }
                );
            }

            // Asynchronously write a message from the message queue to the socket
            void write()
            {
                //nothing to write
                if (qMessagesOut.isEmpty())
                    return;

                // Serialize the message
                std::string data;
                Message<T> msg = qMessagesOut.front();
                msg.SerializeToString(&data);

                auto buffer = boost::asio::buffer(data);

                ws.async_write(buffer,
                    [this](std::error_code ec, size_t length)
                    {
                        if (!ec)
                        {

                            // The message is done
                            qMessagesOut.pop_front();

                            if (!qMessagesOut.isEmpty())
                                write();

                        }
                        else
                        {
                            std::cerr << "[" << id << "]" << " Writing Header Failed." << std::endl;
                            if (!isConnected())
                                disconnect();                        }
                        
                    });
            }

            // Add the temporary msg to the interface (client or server) queue 
            void addToIncomingMsgQueue(){

                // If the owner is a server, we add the current connection as the
                // owner to the message
                if (ownerType == owner::server)
                {
                    qMessagesIn.push_back(OwnedMessage<T>(this->shared_from_this(), msgTemporaryIn));
                }
                else
                {
                    // The owner is a client therefore we do not have other possible endpoints
                    // than the server
                    qMessagesIn.push_back(OwnedMessage<T>(nullptr, msgTemporaryIn));
                } 

                //We finally continue reading 
                read();
            }

            // Called when the connection as a client 
            void OnConnect(std::function<void(void)> clientOnConnect, beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep)
            {
                if (!ec)
                {
                    // Turn of the timeout in the tcp stream as websockets have their own
                    beast::get_lowest_layer(ws).expires_never();

                    // Set a decorator to change the User-Agent of the handshake
                    ws.set_option(websocket::stream_base::decorator(
                        [](websocket::request_type& req)
                        {
                            req.set(http::field::user_agent,
                                std::string("Event-Socket-Cpp-Client"));
                        }));


                    //Perform the handshake as a client
                    handshake(clientOnConnect, ep.address().to_string() + ":" + std::to_string(ep.port()));

                }

            }

            // Called when the handshake is accepted
            void OnAccept(Server::EventServerInterface<T>* serverInterface, beast::error_code ec)
            {
                if (!ec)
                {   
                    // After accepting, let the user perform any callbacks
                    if (serverInterface->OnConnect(this->shared_from_this()))
                    {
                        // Add the connection to our list of connections
                        serverInterface->addConnection(this->shared_from_this());

                        //Update the global ID now that this client has connected
                        serverInterface->addNewGlobalID();

                        this->startReading();

                        //start accepting new connections
                        serverInterface->waitForConnections();
                    }
                    else
                    {   
                        std::cerr << "[-----]" << " Connection refused." << std::endl;
                    }
                }
            }

        private:
            // buffer to temporarily read into
            beast::flat_buffer _buffer;

            // The subprotocol you want to connect to.
            std::string _urlPath{"/"};
    };
} // RedBack