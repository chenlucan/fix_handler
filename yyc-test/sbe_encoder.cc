
#include "sbe_encoder.h"
#include "utility.h"

namespace rczg
{
    
    SBEEncoder::SBEEncoder() : m_encoded_length(0)
    {
        // noop
    }

    void SBEEncoder::Start_encode(std::uint16_t templateId)
    {
        mktdata::MessageHeader header;
        std::size_t encodeHeaderLength = 0;
        std::size_t encodeMessageLength = 0;

        if(templateId == 30)    // SecurityStatus30
        {
            mktdata::SecurityStatus30 message;
            encodeHeaderLength = this->Encode_header<mktdata::SecurityStatus30>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            mktdata::MDIncrementalRefreshVolume37 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshVolume37>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        // TODO other messages
        
        m_encoded_length = encodeHeaderLength + encodeMessageLength;
    }
    
    template <typename MessageType> 
    std::size_t SBEEncoder::Encode_header(mktdata::MessageHeader &header)
    {
        header.wrap(m_buffer, 0, MESSAGE_HEADER_VERSION, sizeof(m_buffer))
              .blockLength(MessageType::sbeBlockLength())
              .templateId(MessageType::sbeTemplateId())
              .schemaId(MessageType::sbeSchemaId())
              .version(MessageType::sbeSchemaVersion());

        return header.encodedLength();
    }

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshVolume37 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(rczg::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);
        
        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& entries = message.noMDEntriesCount(3);
        entries.next().mDEntrySize(71).securityID(72).rptSeq(73).mDUpdateAction(mktdata::MDUpdateAction::New);  // 0
        entries.next().mDEntrySize(61).securityID(62).rptSeq(63).mDUpdateAction(mktdata::MDUpdateAction::Change); // 1
        entries.next().mDEntrySize(51).securityID(52).rptSeq(53).mDUpdateAction(mktdata::MDUpdateAction::Delete); // 2

        return message.encodedLength();
    }
    
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SecurityStatus30 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(rczg::utility::Current_time_ns())
               .putSecurityGroup("ABCDEF")
               .putAsset("+-*/")
               .securityID(654)
               .tradeDate(2017);
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(false)
               .lastQuoteMsg(false)
               .lastStatsMsg(false)
               .lastImpliedMsg(false)
               .recoveryMsg(false)
               .reserved(true)
               .endOfEvent(true);
        
        message.securityTradingStatus(mktdata::SecurityTradingStatus::Close) // 4
               .haltReason(mktdata::HaltReason::InstrumentExpiration) // 4
               .securityTradingEvent(mktdata::SecurityTradingEvent::ResetStatistics); // 4

        return message.encodedLength();
    }

    std::pair<char*, std::size_t> SBEEncoder::Encoded_buffer()
    {
        return std::make_pair(m_buffer, m_encoded_length);
    }

    std::string SBEEncoder::Encoded_hex_str() const
    {
        return rczg::utility::Hex_str(m_buffer, m_encoded_length);
    }

    SBEEncoder::~SBEEncoder()
    {
        // noop
    }
    
} // namespace rczg
    
