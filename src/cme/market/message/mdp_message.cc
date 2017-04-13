
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/sbe_decoder.h"
#include "core/assist/utility.h"
#include "cme/market/message/mktdata.h"
#include "cme/market/message/sbe_to_json.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    // message type index with template id
    const char *MdpMessage::MDP_MESSAGE_TYPES = "----X-------0--A5----------d-df-XXXXXXWR-dXXW";

    MdpMessage::MdpMessage(const char *buffer, std::uint16_t message_length, const size_t packet_length, 
                           std::uint32_t packet_seq_num, std::uint64_t packet_sending_time)
    : m_packet_length(packet_length), m_packet_seq_num(packet_seq_num), m_packet_sending_time(packet_sending_time), 
      m_message_length(message_length), m_buffer(&buffer[0], &buffer[message_length]),
      m_received_time(fh::core::assist::utility::Current_time_ns())
    {
        this->Reset();
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

    // 将消息转换成 json 格式然后发送出去，以便保存到数据库
    std::string MdpMessage::Serialize() const
    {
        this->Reset();  // 因为在这个方法之前， mdp message 可能被解析过，会导致内部 position 指针已经变化了，所以这里要重置下
        SBEtoJSON s2j(this);
        return s2j.To_json();
    }

    MdpMessage::~MdpMessage()
    {
        // noop
    }

    void MdpMessage::Reset() const
    {
        fh::cme::market::message::SBEDecoder decoder(m_buffer.data(), m_message_length);
        auto sbe_message = decoder.Start_decode();
        m_message_header = sbe_message.first;
        m_message_body = sbe_message.second;
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
