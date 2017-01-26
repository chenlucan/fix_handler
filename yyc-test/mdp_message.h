#ifndef __MDP_MESSAGE_H__
#define __MDP_MESSAGE_H__

#include "global.h"
#include "mktdata.h"

namespace rczg
{
    class MdpMessage
    {
        public:
            MdpMessage(const char *buffer, std::uint16_t message_length, const size_t packet_length, 
                       std::uint32_t packet_seq_num, std::uint64_t packet_sending_time); 
            MdpMessage(MdpMessage &&m);
            virtual ~MdpMessage(); 
            
        public:
            // serialize to send
            std::string Serialize() const;
            
        public:
            size_t packet_length() const;
            std::uint32_t packet_seq_num() const;
            std::uint64_t packet_sending_time() const;
            std::uint16_t message_length() const;
            std::shared_ptr<void> message_header() const;
            std::shared_ptr<void> message_body() const;
            std::string received_time() const;
            
        private:
            size_t m_packet_length;
            std::uint32_t m_packet_seq_num;
            std::uint64_t m_packet_sending_time;
            std::uint16_t m_message_length;
            std::vector<char> m_buffer;
            std::shared_ptr<void> m_message_header;
            std::shared_ptr<void> m_message_body;
            std::string m_received_time;
            
        private:
            DISALLOW_COPY_AND_ASSIGN(MdpMessage);
    };    
}


#endif // __MDP_MESSAGE_H__