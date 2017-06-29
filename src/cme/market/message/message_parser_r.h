#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_R_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_R_H__

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
    class MessageParserR
    {
        public:
        MessageParserR();
        virtual    ~MessageParserR();

        public:
            void Parse(const fh::cme::market::message::MdpMessage &message, std::vector<Book> &books);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserR);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_R_H__
