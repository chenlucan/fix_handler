#ifndef __FH_CME_MARKET_MESSAGE_MDP_MESSAGE_H__
#define __FH_CME_MARKET_MESSAGE_MDP_MESSAGE_H__

#include <memory>
#include <vector>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
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
            std::uint64_t received_time() const;
            std::uint16_t template_id() const;
            char message_type() const;
            std::uint32_t last_msg_seq_num_processed() const;

        private:
            // message type index with template id
            static const char *MDP_MESSAGE_TYPES;

        private:
            size_t m_packet_length;
            std::uint32_t m_packet_seq_num;
            std::uint64_t m_packet_sending_time;
            std::uint16_t m_message_length;
            std::vector<char> m_buffer;
            std::shared_ptr<void> m_message_header;
            std::shared_ptr<void> m_message_body;
            std::uint64_t m_received_time;

        private:
            DISALLOW_COPY_AND_ASSIGN(MdpMessage);
    };    
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MDP_MESSAGE_H__
