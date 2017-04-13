
#include "cme/market/dat_processor.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"
#include "cme/market/message/message_utility.h"

namespace fh
{
namespace cme
{
namespace market
{

    DatProcessor::DatProcessor(fh::cme::market::DatSaver *saver, fh::cme::market::DatReplayer *replayer, std::function<void(void)> on_error)
    : m_arbitrator(), m_saver(saver), m_replayer(replayer), m_on_error(on_error)
    {
        // noop
    }

    DatProcessor::~DatProcessor()
    {
        // noop
    }

    // process tcp replay data to mdp messages and save it
    void DatProcessor::Process_replay_data(char *buffer, const size_t data_length)
    {
        // check if packet is valid by first 4 bytes(packet sqquence number)
        std::uint32_t packet_seq_num = *((std::uint32_t *)buffer);
        std::uint32_t status = m_arbitrator.Check_replay_packet(packet_seq_num);

        if(status == std::numeric_limits<std::uint32_t>::max())
        {
            // should be discarded
            LOG_DEBUG("{IN}****** discard tcp increment packet: seq=", packet_seq_num);
            return;
        }

        Process_data(buffer, data_length);
    }

    // process udp feed data to mdp messages and save it
    void DatProcessor::Process_feed_data(char *buffer, const size_t data_length)
    {
        // check if packet is valid by first 4 bytes(packet sqquence number)
        std::uint32_t packet_seq_num = *((std::uint32_t *)buffer);
        std::uint32_t status = m_arbitrator.Check_feed_packet(packet_seq_num);

        if(status == std::numeric_limits<std::uint32_t>::max())
        {
            // should be discarded
            LOG_DEBUG("{IN}****** discard udp increment packet: seq=", packet_seq_num);
            return;
        }

        if(status > 0)
        {
            // gap detected, should start tcp replay
            Start_tcp_replay(status, packet_seq_num);
        }

        Process_data(buffer, data_length);
    }

    // process packet data to mdp messages and save it
    // data layout:
    //  MsgSeqNum : 4 bytes
    //  SendingTime : 8 bytes
    //  Message :
    //   MsgSize : 2 bytes
    //   SBE data
    void DatProcessor::Process_data(char *buffer, const size_t data_length)
    {
        fh::core::assist::TimeMeasurer t;

        // get messages from packet
        std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
        std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buffer, data_length, mdp_messages);
        this->Save_message(packet_seq_num, mdp_messages);

        std::vector<char> message_types;
        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                {
                    message_types.push_back(m.message_type());
                });
        LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", data_length, ", message=", std::string(message_types.begin(), message_types.end()));
    }

    void DatProcessor::Start_tcp_replay(std::uint32_t begin, std::uint32_t end)
    {
        // TCP replayer 没有设置的话，直接调用错误处理 handle，退出
        if(m_replayer == nullptr)
        {
            this->m_on_error();
            return;
        }

        std::thread t([this, begin, end]{
            m_replayer->Start_receive(
                std::bind(&fh::cme::market::DatProcessor::Process_replay_data, std::ref(*this), std::placeholders::_1, std::placeholders::_2),
                begin, end);
        });
        t.detach();     // must make it unjoinable
    }

    void DatProcessor::Save_message(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages)
    {
        m_saver->Insert_data(packet_seq_num, mdp_messages);
    }

} // namespace market
} // namespace cme
} // namespace fh
