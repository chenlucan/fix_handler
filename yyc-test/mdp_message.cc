
#include "mdp_message.h"
#include "sbe_decoder.h"
#include "utility.h"

namespace rczg
{
    MdpMessage::MdpMessage(const char *buffer, std::uint16_t message_length, const size_t packet_length, 
                           std::uint32_t packet_seq_num, std::uint64_t packet_sending_time)
    : m_packet_length(packet_length), m_packet_seq_num(packet_seq_num), m_packet_sending_time(packet_sending_time), 
      m_message_length(message_length), m_buffer(&buffer[0], &buffer[message_length]),
      m_received_time(rczg::utility::Current_time_str())
    {
        rczg::SBEDecoder decoder(m_buffer.data(), m_message_length);
        auto sbe_message = decoder.Start_decode();
        m_message_header = sbe_message.first;
        m_message_body = sbe_message.second;
    }
    
    MdpMessage::MdpMessage(MdpMessage &&m)
    : m_packet_length(m.m_packet_length), m_packet_seq_num(m.m_packet_seq_num), m_packet_sending_time(m.m_packet_sending_time), 
      m_message_length(m.m_message_length), m_buffer(std::move(m.m_buffer)),    // must move sbe buffer, not copy
      m_message_header(m.m_message_header), m_message_body(m.m_message_body),
      m_received_time(std::move(m.m_received_time))
    {
        // noop
    }

    size_t MdpMessage::packet_length() const
    {
        return m_packet_length;
    }
    
    std::uint32_t MdpMessage::packet_seq_num() const
    {
        return m_packet_seq_num;
    }
    
    std::uint64_t MdpMessage::packet_sending_time() const
    {
        return m_packet_sending_time;
    }
    
    std::uint16_t MdpMessage::message_length() const
    {
        return m_message_length;
    }
    
    std::shared_ptr<void> MdpMessage::message_header() const
    {
        return m_message_header;
    }
    
    std::shared_ptr<void> MdpMessage::message_body() const
    {
        return m_message_body;
    }
    
    std::string MdpMessage::received_time() const
    {
        return m_received_time;
    }

    // serialize to send
    std::string MdpMessage::Serialize() const
    {
        // TODO serialize
        std::ostringstream ss;

        ss << "[" << m_received_time << "]";
        ss << " packet_len=" << std::setw(4) << m_packet_length;
        ss << " seq=" << std::setw(6) << m_packet_seq_num;
        ss << " time=" << std::setw(19) << m_packet_sending_time;
        ss << " - message_len=" << std::setw(6) << m_message_length << " : ";


        auto header = static_cast<mktdata::MessageHeader*>(m_message_header.get());
        std::uint16_t templateId = header->templateId();

        void *mdp_message = m_message_body.get();
        if(templateId == 30)    // SecurityStatus30
        {
            auto ss30 = static_cast<mktdata::SecurityStatus30*>(mdp_message);
            ss << "type=SecurityStatus30";
            ss << ", sbeSemanticType=" << ss30->sbeSemanticType();
            ss << ", transactTime=" << ss30->transactTime();
            //ss << ", securityTradingEvent=" << ss30->securityTradingEvent();
        }
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            auto md37 = static_cast<mktdata::MDIncrementalRefreshVolume37*>(mdp_message);
            ss << "type=MDIncrementalRefreshVolume37";
            ss << ", sbeSemanticType=" << md37->sbeSemanticType();
            ss << ", transactTime=" << md37->transactTime();
        }
        else if(templateId == 27)    // MDInstrumentDefinitionFuture27
        {
            auto md27 = static_cast<mktdata::MDInstrumentDefinitionFuture27*>(mdp_message);
            ss << "type=MDInstrumentDefinitionFuture27";
            ss << ", sbeSemanticType=" << md27->sbeSemanticType();
            ss << ", totNumReports=" << md27->totNumReports();
        }
        else if(templateId == 38)    // SnapshotFullRefresh38
        {
            auto md38 = static_cast<mktdata::SnapshotFullRefresh38*>(mdp_message);
            ss << "type=SnapshotFullRefresh38";
            ss << ", sbeSemanticType=" << md38->sbeSemanticType();
            ss << ", lastMsgSeqNumProcessed=" << md38->lastMsgSeqNumProcessed();
            ss << ", totNumReports=" << md38->totNumReports();
            ss << ", rptSeq=" << md38->rptSeq();
        }
        
        return ss.str();
    }
    
    MdpMessage::~MdpMessage()
    {
        // noop
    }
}

