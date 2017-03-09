#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_W_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_W_H__

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
    class MessageParserW
    {
        public:
            MessageParserW();
            virtual ~MessageParserW();

        public:
            void Parse(const fh::cme::market::message::MdpMessage &message, std::vector<Book> &books);

        private:
            void Parse_38(mktdata::SnapshotFullRefresh38 *message, std::vector<Book> &books);
            void Parse_44(mktdata::SnapshotFullRefreshOrderBook44 *message, std::vector<Book> &books);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserW);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_W_H__
