
#include "sbe_decoder.h"

namespace rczg
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
        
        if(templateId == 30)    // SecurityStatus30
        {
            message = this->Decode_message<mktdata::MessageHeader, mktdata::SecurityStatus30>(*header);
        }
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            message = this->Decode_message<mktdata::MessageHeader, mktdata::MDIncrementalRefreshVolume37>(*header);
        }
        else if(templateId == 27)    // MDInstrumentDefinitionFuture27
        {
            message = this->Decode_message<mktdata::MessageHeader, mktdata::MDInstrumentDefinitionFuture27>(*header);
        }
        else if(templateId == 38)    // SnapshotFullRefresh38
        {
            message = this->Decode_message<mktdata::MessageHeader, mktdata::SnapshotFullRefresh38>(*header);
        }
        // TODO other messages
        
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
    
} // namespace rczg
