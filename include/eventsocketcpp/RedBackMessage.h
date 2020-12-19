// A message type that contains a message as well as a header specifies the type of message it is

#pragma once
// Forward declaration for dependencies
namespace RedBack {
    template<typename T>
    struct OwnedMessage;

    template<typename>
    struct MessageHeader;

    template<typename T>
    struct Message;

    struct MetaConfig;

    // All possible configurations to the server/client
    enum class Config {
        Forward, Forwarded, BroadcastAll, BroadcastRoom, Broadcasted,
        CreateRoom, CreateRoomResponse, JoinRoom, OnRoomJoined,
        None
    };
}


