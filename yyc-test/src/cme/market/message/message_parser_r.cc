
#include "core/assist/logger.h"
#include "cme/market/message/message_parser_r.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    MessageParserR::MessageParserR()
    {
        // noop
    }

    MessageParserR::~MessageParserR()
    {
        // noop
    }

    void MessageParserR::Parse(const fh::cme::market::message::MdpMessage &message, std::vector<Book> &books)
    {
        // TODO  QuoteRequest39
        LOG_DEBUG("message type is R, TODO");
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
