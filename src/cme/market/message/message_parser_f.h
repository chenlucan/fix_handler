#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__

#include "core/global.h"
#include "cme/market/message/mktdata.h"
#include "cme/market/message/mdp_message.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    class MessageParserF
    {
        public:
            MessageParserF();
            virtual ~MessageParserF();

        public:
            std::pair<std::uint32_t, mktdata::SecurityTradingStatus::Value> Parse(const fh::cme::market::message::MdpMessage &message);

        private:
            std::pair<std::uint32_t, mktdata::SecurityTradingStatus::Value> Parse_30(const mktdata::SecurityStatus30 *message);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserF);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__
