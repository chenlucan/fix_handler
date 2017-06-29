
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

    std::pair<std::uint32_t, mktdata::SecurityTradingStatus::Value> MessageParserF::Parse(
            const fh::cme::market::message::MdpMessage &message)
    {
        std::uint16_t template_id = message.template_id();
        void *body = message.message_body().get();

        LOG_DEBUG("parse security status message.");

        // SecurityStatus30
        auto status = this->Parse_30((mktdata::SecurityStatus30 *)body);

        LOG_DEBUG("message type is f, template id is ", template_id, " done.");

        return status;
    }

    std::pair<std::uint32_t, mktdata::SecurityTradingStatus::Value> MessageParserF::Parse_30(
            const mktdata::SecurityStatus30 *message)
    {
        return {message->securityID(), message->securityTradingStatus()};
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
