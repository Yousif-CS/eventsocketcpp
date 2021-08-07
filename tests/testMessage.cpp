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

TEST(MessageTest, HandlesReadsAndWritesNotStrings) {
	RedBack::Message<EventTypes> message;

	int a = 4; uint32_t b = 3; float c = 3.44;

	message << a << b << c;

	int d = 0; uint32_t e = 0; float f = 0;

	message >> f >> e >> d;

	ASSERT_EQ(c, f);
	ASSERT_EQ(b, e);
	ASSERT_EQ(a, d);
}


TEST(MessageTest, SerializingAndDeserializing) {
	RedBack::Message<EventTypes> message(EventTypes::Message);

	int a = 4; uint32_t b = 3; float c = 3.44;

	message << a << b << c;

	std::string out;
	message.SerializeToString(&out);

	message = RedBack::Message<EventTypes>(EventTypes::Message);

	message.ParseFromString(out);

	int d = 0; uint32_t e = 0; float f = 0;

	message >> f >> e >> d;

	ASSERT_EQ(c, f);
	ASSERT_EQ(b, e);
	ASSERT_EQ(a, d);
}

TEST(MessageTest, CannotReadWhenNotEnoughData) {
    RedBack::Message<EventTypes> message(EventTypes::Message);


    int a = 4;
    int b = 0;
    message << a;
    message >> b;

    auto invalidFunc = [&]() {
        message >> b;
    };

    ASSERT_THROW(invalidFunc(), std::runtime_error);
}