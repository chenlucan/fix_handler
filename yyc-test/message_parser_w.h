#ifndef __MESSAGE_PARSER_W_H__
#define __MESSAGE_PARSER_W_H__

#include "global.h"
#include "book.h"
#include "mdp_message.h"

namespace rczg
{
	class MessageParserW
	{
		public:
			MessageParserW();
			virtual ~MessageParserW();

		public:
			void Parse(const rczg::MdpMessage &message, std::vector<Book> &books);

		private:
			void Parse_38(mktdata::SnapshotFullRefresh38 *message, std::vector<Book> &books);
			void Parse_44(mktdata::SnapshotFullRefreshOrderBook44 *message, std::vector<Book> &books);

		private:
			DISALLOW_COPY_AND_ASSIGN(MessageParserW);
	};
}

#endif // __MESSAGE_PARSER_W_H__
