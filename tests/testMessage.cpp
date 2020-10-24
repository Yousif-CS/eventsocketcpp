// Tests for the Message datatype

#include <gtest/gtest.h>
#include <eventsocketcpp/RedBackMessage.h>
#include <eventsocketcpp/RedBackConnection.h>
#include <eventsocketcpp/server/EventServerInterface.h>
#include <eventsocketcpp/client/EventClientInterface.h>

enum class EventTypes {
    Connect, Disconnect, Message,
};


TEST(MessageTest, HandlesReadsAndWrites){
    RedBack::Message<EventTypes> message;

    int a = 4;
    float b = 3.5f;
    std::string str = "Hello";
    
    message << a << b << str;

    a = 3;
    b = 4.0f;
    str = "World";

    str.reserve(4);
    
    message >> str >> b >> a;

    ASSERT_EQ(a, 4);
    ASSERT_EQ(b, 3.5f);
    ASSERT_EQ(str, "Hello");
}