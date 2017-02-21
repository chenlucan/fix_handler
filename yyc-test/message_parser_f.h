#ifndef __MESSAGE_PARSER_F_H__
#define __MESSAGE_PARSER_F_H__

#include "global.h"
#include "book.h"
#include "mdp_message.h"

namespace rczg
{
	class MessageParserF
	{
		public:
			MessageParserF();
			virtual ~MessageParserF();

		public:
			void Parse(const rczg::MdpMessage &message, std::vector<Book> &books);

		private:

		private:
			DISALLOW_COPY_AND_ASSIGN(MessageParserF);
	};
}

#endif // __MESSAGE_PARSER_F_H__
