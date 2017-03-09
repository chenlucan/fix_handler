
#include "core/assist/logger.h"
#include "cme/market/message/message_parser_d.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    MessageParserD::MessageParserD()
    {
        // noop
    }

    MessageParserD::~MessageParserD()
    {
        // noop
    }

    void MessageParserD::Parse(const MdpMessage &message, std::vector<Instrument> &instruments)
    {
        std::uint16_t template_id = message.template_id();
        void *body = message.message_body().get();

        LOG_DEBUG("parse definition message to instruments");

        switch(template_id)
        {
            case 27:        // MDInstrumentDefinitionFuture27
                this->Parse_27((mktdata::MDInstrumentDefinitionFuture27 *)body, instruments);
                break;
            case 29:    // MDInstrumentDefinitionSpread29
                this->Parse_29((mktdata::MDInstrumentDefinitionSpread29 *)body, instruments);
                break;
            case 41:    // MDInstrumentDefinitionOption41
                this->Parse_41((mktdata::MDInstrumentDefinitionOption41 *)body, instruments);
                break;
            default:
                break;
        }

        // set each instruments packet_seq_num
        std::for_each(instruments.begin(), instruments.end(), [&message, template_id](Instrument &i){
            i.packet_seq_num = message.packet_seq_num();
            i.template_id = template_id;
        });

        LOG_DEBUG("message type is d, template id is ", template_id, " done.");
    }

    void MessageParserD::Parse_27(mktdata::MDInstrumentDefinitionFuture27 *message, std::vector<Instrument> &instruments)
    {
        instruments.push_back(this->Parse_X(message));
    }

    void MessageParserD::Parse_29(mktdata::MDInstrumentDefinitionSpread29 *message, std::vector<Instrument> &instruments)
    {
        instruments.push_back(this->Parse_X(message));
    }

    void MessageParserD::Parse_41(mktdata::MDInstrumentDefinitionOption41 *message, std::vector<Instrument> &instruments)
    {
        instruments.push_back(this->Parse_X(message));
    }

    template <typename T>
    Instrument MessageParserD::Parse_X(T *message)
    {
        Instrument i;
        i.appId = message->applID();
        i.securityID = message->securityID();
        i.depthGBI = 0;
        i.depthGBX = 0;

        // 重要：必须要跳过下面这个 group 后才能正确取到 NoMDFeedTypes 字段值
        auto noEvents = message->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();
        }

        auto noMDFeedTypes = message->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();
            std::string mdFeedType = noMDFeedTypes.getMDFeedTypeAsString();
            std::uint8_t marketDepth = noMDFeedTypes.marketDepth();
            if(mdFeedType == "GBI")
            {
                i.depthGBI = marketDepth;
            }
            else if(mdFeedType == "GBX")
            {
                i.depthGBX = marketDepth;
            }
        }

        return i;
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
