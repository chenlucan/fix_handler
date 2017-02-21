#ifndef __MESSAGE_PARSER_X_H__
#define __MESSAGE_PARSER_X_H__

#include "global.h"
#include "book.h"
#include "mdp_message.h"

namespace rczg
{
	class MessageParserX
	{
		public:
			MessageParserX();
			virtual ~MessageParserX();

		public:
			void Parse(const rczg::MdpMessage &message, std::vector<Book> &books);

		private:
			void Parse_4(mktdata::ChannelReset4 *message, std::vector<Book> &books);
			void Parse_32(mktdata::MDIncrementalRefreshBook32 *message, std::vector<Book> &books);
			void Parse_33(mktdata::MDIncrementalRefreshDailyStatistics33 *message, std::vector<Book> &books);
			void Parse_34(mktdata::MDIncrementalRefreshLimitsBanding34 *message, std::vector<Book> &books);
			void Parse_35(mktdata::MDIncrementalRefreshSessionStatistics35 *message, std::vector<Book> &books);
			void Parse_36(mktdata::MDIncrementalRefreshTrade36 *message, std::vector<Book> &books);
			void Parse_37(mktdata::MDIncrementalRefreshVolume37 *message, std::vector<Book> &books);
			void Parse_42(mktdata::MDIncrementalRefreshTradeSummary42 *message, std::vector<Book> &books);
			void Parse_43(mktdata::MDIncrementalRefreshOrderBook43 *message, std::vector<Book> &books);
			void Set_match_event_indicator(const mktdata::MatchEventIndicator& matchEventIndicator, std::vector<Book> &books);

		private:
			DISALLOW_COPY_AND_ASSIGN(MessageParserX);
	};
}

#endif // __MESSAGE_PARSER_X_H__
