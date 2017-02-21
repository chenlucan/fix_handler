
#include "mdp_message.h"
#include "sbe_decoder.h"
#include "utility.h"

namespace rczg
{
	// message type index with template id
	const char *MdpMessage::MDP_MESSAGE_TYPES = "----X-------0--A5----------d-df-XXXXXXWR-dXXW";

    MdpMessage::MdpMessage(const char *buffer, std::uint16_t message_length, const size_t packet_length, 
                           std::uint32_t packet_seq_num, std::uint64_t packet_sending_time)
    : m_packet_length(packet_length), m_packet_seq_num(packet_seq_num), m_packet_sending_time(packet_sending_time), 
      m_message_length(message_length), m_buffer(&buffer[0], &buffer[message_length]),
      m_received_time(rczg::utility::Current_time_ns())
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
      m_received_time(m.m_received_time)
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
    
    std::uint64_t MdpMessage::received_time() const
    {
        return m_received_time;
    }

    std::uint16_t MdpMessage::template_id() const
    {
    	auto header = static_cast<mktdata::MessageHeader*>(m_message_header.get());
    	return header->templateId();
    }

    char MdpMessage::message_type() const
    {
    	return MDP_MESSAGE_TYPES[this->template_id()];
    }

    // value of tag 369 if is recovery message(35=W)
    std::uint32_t MdpMessage::last_msg_seq_num_processed() const
    {
    	// header is 8 bytes; body's first 4 bytes is tag 369-LastMsgSeqNumProcessed
    	return *(std::uint32_t *)(m_buffer.data() + 8);
    }

    // serialize to send
    std::string MdpMessage::Serialize() const
    {
		//     8 bytes : m_received_time
		//		2 bytes : m_packet_length
		//		4 bytes : m_packet_seq_num
		//		8 bytes : m_packet_sending_time
		//		2 bytes : m_message_length
		//		(m_message_length) bytes : m_buffer
    	std::uint16_t length = 8 + 2 + 4 + 8 + 2 + m_message_length;
    	std::vector<char> message;
    	message.reserve(length);
    	char *data = message.data();
    	memcpy(data, (char *)&m_received_time, 8);
    	memcpy(data + 8, (char *)&m_packet_length, 2);
    	memcpy(data + 8 + 2, (char *)&m_packet_seq_num, 4);
    	memcpy(data + 8 + 2 + 4, (char *)&m_packet_sending_time, 8);
    	memcpy(data + 8 + 2 + 4 + 8, (char *)&m_message_length, 2);
    	memcpy(data + 8 + 2 + 4 + 8 + 2, m_buffer.data(), m_message_length);

        return std::string(data, length);
    }
    
    MdpMessage::~MdpMessage()
    {
        // noop
    }
}

