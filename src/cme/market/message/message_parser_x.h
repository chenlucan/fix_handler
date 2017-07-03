#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_X_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_X_H__

#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/mktdata.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    class MessageParserX
    {
        public:
            MessageParserX();
            virtual ~MessageParserX();

        public:
            void Parse(const fh::cme::market::message::MdpMessage &message, std::vector<fh::cme::market::message::Book> &books);

        private:
            void Parse_4(mktdata::ChannelReset4 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_32(mktdata::MDIncrementalRefreshBook32 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_33(mktdata::MDIncrementalRefreshDailyStatistics33 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_34(mktdata::MDIncrementalRefreshLimitsBanding34 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_35(mktdata::MDIncrementalRefreshSessionStatistics35 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_36(mktdata::MDIncrementalRefreshTrade36 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_37(mktdata::MDIncrementalRefreshVolume37 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_42(mktdata::MDIncrementalRefreshTradeSummary42 *message, std::vector<fh::cme::market::message::Book> &books);
            void Parse_43(mktdata::MDIncrementalRefreshOrderBook43 *message, std::vector<fh::cme::market::message::Book> &books);
            void Set_match_event_indicator(const mktdata::MatchEventIndicator& matchEventIndicator, std::vector<fh::cme::market::message::Book> &books);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserX);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_X_H__
