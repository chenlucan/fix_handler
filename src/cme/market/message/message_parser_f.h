#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__

#include "core/global.h"
#include "cme/market/message/book.h"
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
            void Parse(const fh::cme::market::message::MdpMessage &message, std::vector<Book> &books);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserF);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_F_H__
