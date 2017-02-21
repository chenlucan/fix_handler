
#include "mdp_receiver.h"
#include "sbe_decoder.h"
#include "logger.h"
#include "utility.h"

namespace rczg
{
    
    MdpReceiver::MdpReceiver(const char *url) : ZmqReceiver(url)
    {
    	// noop
    }
    
    MdpReceiver::~MdpReceiver()
    {
        // noop
    }
    
    // TODO save it
    void MdpReceiver::Save(char *data, size_t size)
    {
        //     8 bytes : m_received_time
		//		2 bytes : m_packet_length
		//		4 bytes : m_packet_seq_num
		//		8 bytes : m_packet_sending_time
		//		2 bytes : m_message_length
		//		(m_message_length) bytes : m_buffer

    	std::ostringstream ss;
    	ss << "received_time=" << *(std::uint64_t *)data << " ";
    	ss << "packet_length=" << *(std::uint16_t *)(data + 8) << " ";
    	ss << "packet_seq_num=" << *(std::uint32_t *)(data + 8 + 2) << " ";
    	ss << "packet_sending_time=" << *(std::uint64_t *)(data + 8 + 2 + 4) << " ";
    	ss << "message_length=" << *(std::uint16_t *)(data + 8 + 2 + 4 + 8) << " ";
    	ss << "message=[";

        rczg::SBEDecoder decoder(data + 8 + 2 + 4 + 8 + 2, *(std::uint16_t *)(data + 8 + 2 + 4 + 8));
        auto sbe_message = decoder.Start_decode();
        auto message_header = sbe_message.first;
        auto message_body = sbe_message.second;

    	auto header = static_cast<mktdata::MessageHeader*>(message_header.get());
    	std::uint16_t templateId = header->templateId();

    	void *mdp_message = message_body.get();
    	if(templateId == 30)    // SecurityStatus30
    	{
    	    auto ss30 = static_cast<mktdata::SecurityStatus30*>(mdp_message);
    	    ss << "type=SecurityStatus30";
    	    ss << ", sbeSemanticType=" << ss30->sbeSemanticType();
    	    ss << ", transactTime=" << ss30->transactTime();
    	    //ss << ", securityTradingEvent=" << ss30->securityTradingEvent();
    	}
    	else if(templateId == 32)    // MDIncrementalRefreshBook32
    	{
    	    auto md32 = static_cast<mktdata::MDIncrementalRefreshBook32*>(mdp_message);
    	    ss << "type=MDIncrementalRefreshBook32";
    	    ss << ", sbeSemanticType=" << md32->sbeSemanticType();
    	    ss << ", transactTime=" << md32->transactTime();
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
    	else if(templateId == 15)    // AdminLogin15
    	{
    	    auto md15 = static_cast<mktdata::AdminLogin15*>(mdp_message);
    	    ss << "type=AdminLogin15";
    	    ss << ", sbeSemanticType=" << md15->sbeSemanticType();
    	    ss << ", heartBtInt=" << (int)md15->heartBtInt();
    	}
    	else if(templateId == 16)    // AdminLogout16
    	{
    	    auto md16 = static_cast<mktdata::AdminLogout16*>(mdp_message);
    	    ss << "type=AdminLogout16";
    	    ss << ", sbeSemanticType=" << md16->sbeSemanticType();
    	    ss << ", text=" << md16->text();
    	}

    	ss << "]";

        LOG_TRACE("messages: ", ss.str());
    }
    
}







