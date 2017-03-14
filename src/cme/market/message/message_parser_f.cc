
#include "core/assist/logger.h"
#include "cme/market/message/message_parser_f.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    MessageParserF::MessageParserF()
    {
        // noop
    }

    MessageParserF::~MessageParserF()
    {
        // noop
    }

    void MessageParserF::Parse(const fh::cme::market::message::MdpMessage &message, std::vector<Book> &books)
    {
        // TODO  SecurityStatus30
        LOG_DEBUG("message type is f, TODO");
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
