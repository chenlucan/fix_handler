#ifndef __MESSAGE_PARSER_R_H__
#define __MESSAGE_PARSER_R_H__

#include "global.h"
#include "book.h"
#include "mdp_message.h"

namespace rczg
{
	class MessageParserR
	{
		public:
		MessageParserR();
		virtual	~MessageParserR();

		public:
			void Parse(const rczg::MdpMessage &message, std::vector<Book> &books);

		private:

		private:
			DISALLOW_COPY_AND_ASSIGN(MessageParserR);
	};
}

#endif // __MESSAGE_PARSER_R_H__
