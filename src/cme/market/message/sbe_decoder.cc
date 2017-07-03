
#include "cme/market/message/sbe_decoder.h"
#include "cme/market/message/mktdata.h"

#define MESSAGE_HEADER_VERSION 0     // mdp sbe message header version

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    SBEDecoder::SBEDecoder(char *buffer, std::size_t buffer_length)
        : m_buffer(buffer), m_buffer_length(buffer_length)
    {
    }

    // decode buffer to mdp message
    // return shared pointer of message header and message body
    // message header and body's data are same as input buffer
    std::pair<std::shared_ptr<void>, std::shared_ptr<void>> SBEDecoder::Start_decode()
    {
        auto header = this->Decode_header<mktdata::MessageHeader>();

        std::uint16_t templateId = header->templateId();
        std::shared_ptr<void> message;

        switch(templateId)
        {
            case 4:    // ChannelReset4: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::ChannelReset4>(*header);
                break;
            case 12:    // AdminHeartbeat12: 0
                message = this->Decode_message<mktdata::MessageHeader, mktdata::AdminHeartbeat12>(*header);
                break;
            case 15:    // AdminLogin15: A
                message = this->Decode_message<mktdata::MessageHeader, mktdata::AdminLogin15>(*header);
                break;
            case 16:    // AdminLogout16: 5
                message = this->Decode_message<mktdata::MessageHeader, mktdata::AdminLogout16>(*header);
                break;
            case 27:    // MDInstrumentDefinitionFuture27: d
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDInstrumentDefinitionFuture27>(*header);
                break;
            case 29:    // MDInstrumentDefinitionSpread29: d
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDInstrumentDefinitionSpread29>(*header);
                break;
            case 30:    // SecurityStatus30: f
                message = this->Decode_message<mktdata::MessageHeader, mktdata::SecurityStatus30>(*header);
                break;
            case 32:    // MDIncrementalRefreshBook32: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshBook32>(*header);
                break;
            case 33:    // MDIncrementalRefreshDailyStatistics33: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshDailyStatistics33>(*header);
                break;
            case 34:    // MDIncrementalRefreshLimitsBanding34: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshLimitsBanding34>(*header);
                break;
            case 35:    // MDIncrementalRefreshSessionStatistics35: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshSessionStatistics35>(*header);
                break;
            case 36:    // MDIncrementalRefreshTrade36: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshTrade36>(*header);
                break;
            case 37:    // MDIncrementalRefreshVolume37: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshVolume37>(*header);
                break;
            case 38:    // SnapshotFullRefresh38: W
                message = this->Decode_message<mktdata::MessageHeader, mktdata::SnapshotFullRefresh38>(*header);
                break;
            case 39:    // QuoteRequest39: R
                message = this->Decode_message<mktdata::MessageHeader, mktdata::QuoteRequest39>(*header);
                break;
            case 41:    // MDInstrumentDefinitionOption41: d
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDInstrumentDefinitionOption41>(*header);
                break;
            case 42:    // MDIncrementalRefreshTradeSummary42: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshTradeSummary42>(*header);
                break;
            case 43:    // MDIncrementalRefreshOrderBook43: X
                message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshOrderBook43>(*header);
                break;
            case 44:    // SnapshotFullRefreshOrderBook44: W
                message = this->Decode_message<mktdata::MessageHeader, mktdata::SnapshotFullRefreshOrderBook44>(*header);
                break;
            default:
                // noop
                break;
        }

        return std::make_pair(header, message);
    }

    template <typename HeaderType> 
    std::shared_ptr<HeaderType> SBEDecoder::Decode_header()
    {
        std::shared_ptr<HeaderType> header(new HeaderType());
        header->wrap(m_buffer, 0, MESSAGE_HEADER_VERSION, m_buffer_length);
        return header;
    }

    template <typename HeaderType, typename MessageType> 
    std::shared_ptr<MessageType> SBEDecoder::Decode_message(const HeaderType &header)
    {
        std::shared_ptr<MessageType> message(new MessageType());
        message->wrapForDecode(m_buffer, header.encodedLength(), header.blockLength(), header.version(), m_buffer_length);
        return message;
    }

    SBEDecoder::~SBEDecoder()
    {
        // noop
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
