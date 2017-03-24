
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
        // SecurityStatus30 无需处理
        LOG_DEBUG("message type is f, discard");
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
