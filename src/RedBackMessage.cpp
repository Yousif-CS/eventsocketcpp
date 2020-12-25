// A message type that contains a message as well as a header specifies the type of message it is

#include <memory>
#include <string>

#include "eventsocketcpp/RedBackMessage.h"
#include "Message.pb.h"

namespace RedBack {

		/*
			A wrapper class that wraps around protobuf implementation
		*/
		class MessageBody::MessageBodyImp {
		public:
			

			MessageBodyImp(EventSocket::Message m)
			:messageImp(m) {}

			MessageBodyImp()
			:MessageBodyImp(EventSocket::Message())
			{}

			// Copy constructor
			MessageBodyImp(const MessageBodyImp&) = default;

			// Copy assignment
			MessageBodyImp& operator=(const MessageBodyImp&) = default;

			EventSocket::MessageHeader* mutable_header(){
				return messageImp.mutable_header();
			}

			std::string * mutable_body() {
				return messageImp.mutable_body();
			}

			bool SerializeToString(std::string * output)
			{
				return messageImp.SerializeToString(output);
			}

			bool ParseFromString(const std::string& data)
			{
				messageImp.ParseFromString(data);
			}

			const std::string& body() const 
			{
				return messageImp.body();
			}

			const EventSocket::MessageHeader& header() const
			{
				return messageImp.header();
			}
			
			void set_body(const std::string& body)
			{
				messageImp.set_body(body);
				messageImp.mutable_header()->set_size(messageImp.body().size());
			}

			void set_body(const void * buffer, size_t size)
			{
				messageImp.set_body(buffer, size);
				messageImp.mutable_header()->set_size(size);
			}

		private:
			EventSocket::Message messageImp;
		};
		/**
		* A wrapper class to hide protobuf implementation
		*/	
	
		MessageBody::MessageBody()
		{
			messageBodyImp = std::make_unique<MessageBodyImp>();
		}
		
		MessageBody::MessageBody(const MessageBody& messageBody)
		: messageBodyImp(std::make_unique<MessageBodyImp>(*messageBody.messageBodyImp))
		{}

		MessageBody& MessageBody::operator=(const MessageBody& messageBody)
		{
			messageBodyImp = std::make_unique<MessageBodyImp>(*messageBody.messageBodyImp);
		}
		
		MessageBody::~MessageBody() = default;
		
		void MessageBody::setID(uint32_t id)
		{
			messageBodyImp->mutable_header()->set_id(id);
		}


		void MessageBody::setConfig(uint32_t conf)
		{
			messageBodyImp->mutable_header()->set_config(conf);
		}

		bool MessageBody::SerializeToString(std::string * output)
		{
			return messageBodyImp->SerializeToString(output);
		}

		bool MessageBody::ParseFromString(const std::string& data)
		{
			messageBodyImp->ParseFromString(data);
		}

		uint32_t MessageBody::header_id()
		{
			return messageBodyImp->header().id();
		}

		uint32_t MessageBody::header_config()
		{
			return messageBodyImp->header().config();
		}

		uint32_t MessageBody::header_size()
		{
			return messageBodyImp->header().size();
		}

		void MessageBody::set_header_size(uint32_t size)
		{
			messageBodyImp->mutable_header()->set_size(size);
		}

		const std::string& MessageBody::body() const
		{
			return messageBodyImp->body();
		}	

		std::string* MessageBody::mutable_body()
		{
			return messageBodyImp->mutable_body();
		}

		void MessageBody::set_body(std::string body)
		{
			messageBodyImp->set_body(body);
		}
		void MessageBody::set_body(const void * buffer, size_t size)
		{
			messageBodyImp->set_body(buffer, size);
		}
};
