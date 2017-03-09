
#include "cme/market/recovery_saver.h"
#include "cme/market/message/message_utility.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    RecoverySaver::RecoverySaver(bool is_definition)
    : m_is_definition(is_definition), m_current_sequence(0), m_total_number(0), m_recovery_datas(), m_mutex()
    {
        // noop
    }

    RecoverySaver::~RecoverySaver()
    {
        // noop
    }

    // process received recovery packet, call callback if last packet received
    void RecoverySaver::Process_recovery_packet(std::function<void()> callback, char *buffer, const size_t data_length)
    {
        std::uint32_t packet_seq_num = *((std::uint32_t *)buffer);
        std::lock_guard<std::mutex> lock(m_mutex);

        if(packet_seq_num != m_current_sequence + 1 && packet_seq_num != 1)
        {
            // discard
            LOG_DEBUG("discard ", m_is_definition ? "definition" : "recovery", " packet: seq=", packet_seq_num, " current=", m_current_sequence);
            return;
        }

        bool is_all_saved = this->Save_packet(buffer, data_length, packet_seq_num);
        LOG_INFO("saved ", m_is_definition ? "definition" : "recovery", " packet: seq=", packet_seq_num, " total=", m_total_number);

        if(is_all_saved)
        {
            callback();
        }
    }

    std::vector<fh::cme::market::message::MdpMessage> *RecoverySaver::Get_data()
    {
        return &m_recovery_datas;
    }

    // return true if all messages are saved
    bool RecoverySaver::Save_packet(char *buffer, const size_t data_length, const std::uint32_t packet_seq_num)
    {
        if(packet_seq_num == 1)
        {
            m_recovery_datas.clear();

            // total packet number(field 911:TotNumReports)
            // packet seq num(4 bytes), packet sending time(8 bytes), message size(2 bytes), sbe header(8 bytes)
            // field 911:TotNumReports offset : 1 byte(definition message), 4 bytes(revocery message)
            char *field911 = buffer + 4 + 8 + 2 + 8 + (m_is_definition ? 1 : 4);
            m_total_number = *((std::uint32_t *)field911);
        }

        // get messages from packet
        m_current_sequence = fh::cme::market::message::utility::Pick_messages_from_packet(buffer, data_length, m_recovery_datas);

        return m_total_number == m_current_sequence;
    }

} // namespace market
} // namespace cme
} // namespace fh
