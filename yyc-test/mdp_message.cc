
#include "mdp_message.h"

namespace rczg
{
    MdpMessage::MdpMessage() 
    : packet_length(0), packet_seq_num(0), packet_sending_time(0), 
      is_valid_packet(false), message_length(0), 
      message_header(), message_body()
    {
        // noop
    }
    
    MdpMessage::MdpMessage(MdpMessage &&m)
    : packet_length(m.packet_length), packet_seq_num(m.packet_seq_num), packet_sending_time(m.packet_sending_time), 
      is_valid_packet(m.is_valid_packet), message_length(m.message_length), 
      message_header(m.message_header), message_body(m.message_body)
    {
        // noop
    }
    
    // copy the message out from socket buffer
    std::pair<std::uint32_t, std::string> MdpMessage::Copy_out() const
    {
        // TODO create mdp message object from socket buffer
        std::ostringstream ss;

        ss << std::this_thread::get_id() << " -";
        ss << " packet_len=" << std::setw(4) << packet_length;
        ss << " seq=" << std::setw(6) << packet_seq_num;
        ss << " time=" << std::setw(19) << packet_sending_time;
        
        if(is_valid_packet)
        {
            ss << " - message_len=" << std::setw(6) << message_length << " : ";
        }
        else
        {
            ss << " < discard" << std::endl;
            return std::make_pair(packet_seq_num, ss.str());
        }

        auto header = static_cast<mktdata::MessageHeader*>(message_header.get());
        std::uint16_t templateId = header->templateId();

        void *mdp_message = message_body.get();
        if(templateId == 30)    // SecurityStatus30
        {
            auto ss30 = static_cast<mktdata::SecurityStatus30*>(mdp_message);
            ss << "type=SecurityStatus30";
            ss << ", sbeSemanticType=" << ss30->sbeSemanticType();
            //ss << ", transactTime=" << ss30->transactTime();
            //ss << ", securityTradingEvent=" << ss30->securityTradingEvent();
        }
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            auto md37 = static_cast<mktdata::MDIncrementalRefreshVolume37*>(mdp_message);
            ss << "type=MDIncrementalRefreshVolume37";
            ss << ", sbeSemanticType=" << md37->sbeSemanticType();
            //ss << ", transactTime=" << md37->transactTime();
        }
        
        return std::make_pair(packet_seq_num, ss.str());
    }
    
    MdpMessage::~MdpMessage()
    {
        // noop
    }
}

