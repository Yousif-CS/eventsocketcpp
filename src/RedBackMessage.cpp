// A message type that contains a message as well as a header specifies the type of message it is

#include <memory>
#include <string>

#include "eventsocketcpp/RedBackMessage.h"
#include "Message.pb.h"

namespace RedBack {
		class MessageBodyImp {
		public:
			EventSocket::MessageHeader* mutable_header(){
				return messageImp.mutable_header();
			}

			std::string* mutable_body() {
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

		size_t MessageBody::body_size()
		{
			return messageBodyImp->body().size();
		}

		void MessageBody::set_header_size(uint32_t size)
		{
			messageBodyImp->mutable_header()->set_size(size);
		}

		uint32_t MessageBody::resize(uint32_t size)
		{
			messageBodyImp->mutable_body()->resize(size);
		}

		std::string::const_iterator MessageBody::body_begin() const
		{
			return messageBodyImp->body().begin();
		}

		std::string::const_iterator MessageBody::body_end() const
		{
			return messageBodyImp->body().end();
		}

		const char * MessageBody::body_data()
		{
			return messageBodyImp->body().data();
		}


		void MessageBody::set_body(std::string body)
		{
			messageBodyImp->set_body(body);
		}
};
