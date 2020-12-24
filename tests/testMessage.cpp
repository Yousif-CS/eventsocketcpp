// Tests for the Message datatype

#include <gtest/gtest.h>
#include <eventsocketcpp/RedBackMessage.h>
#include <eventsocketcpp/RedBackConnection.h>
#include <eventsocketcpp/server/EventServerInterface.h>
#include <eventsocketcpp/client/EventClientInterface.h>

enum class EventTypes {
    Connect, Disconnect, Message,
};


TEST(MessageTest, HandlesReadsAndWritesStrings){
    RedBack::Message<EventTypes> message;

    std::string str = "Hello";
    
    message << str;

    str = "World";

    message >> str;

    ASSERT_EQ(str, "Hello");
}
