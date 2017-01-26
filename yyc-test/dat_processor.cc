
#include "dat_processor.h"
#include "logger.h"

namespace rczg
{
    
    DatProcessor::DatProcessor(rczg::DatArbitrator &arbitrator, rczg::DatSaver &saver, rczg::TCPReceiver &replayer)
    : m_arbitrator(&arbitrator), m_saver(&saver), m_replayer(&replayer)
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
        std::uint32_t status = m_arbitrator->Check_replay_packet(packet_seq_num);

        if(status == std::numeric_limits<std::uint32_t>::max())
        {
            // should be discarded
            rczg::Logger::Debug("****** discard tcp: seq=", packet_seq_num);
            return;
        }
        
        Process_data(buffer, data_length);
    }

    // process udp feed data to mdp messages and save it
    void DatProcessor::Process_feed_data(char *buffer, const size_t data_length)
    {
        // check if packet is valid by first 4 bytes(packet sqquence number)
        std::uint32_t packet_seq_num = *((std::uint32_t *)buffer);
        std::uint32_t status = m_arbitrator->Check_feed_packet(packet_seq_num);

        if(status == std::numeric_limits<std::uint32_t>::max())
        {
            // should be discarded
            rczg::Logger::Debug("****** discard udp: seq=", packet_seq_num);
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
        auto start = std::chrono::high_resolution_clock::now();
        
        char *data_begin = buffer;
        char *data_end = data_begin + data_length;
        std::uint32_t packet_seq_num = *((std::uint32_t *)data_begin);
        std::uint64_t packet_sending_time = *((std::uint64_t *)(data_begin + 4));
        std::vector<rczg::MdpMessage> mdp_messages;
        
        char *current_position = data_begin + 4 + 8;
        while(current_position < data_end)  // TODO != or <
        {
            std::uint16_t message_length = *((std::uint16_t *)current_position);
            char *message = current_position + 2;
            rczg::MdpMessage mdp(message, message_length, data_length, packet_seq_num, packet_sending_time);
            mdp_messages.push_back(std::move(mdp));
            current_position = current_position + message_length + 2;
        }
        
        this->Save_message(packet_seq_num, mdp_messages);
        
        auto finish = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        rczg::Logger::Info("received: ", ns, "ns, seq=", packet_seq_num, ", len=", data_length);
    }

    void DatProcessor::Start_tcp_replay(std::uint32_t begin, std::uint32_t end)
    {
        std::thread t([this, begin, end]{
            m_replayer->Start_receive(
                std::bind(&rczg::DatProcessor::Process_replay_data, std::ref(*this), std::placeholders::_1, std::placeholders::_2),
                begin, end);
        });
        t.detach();     // must make it unjoinable
    }

    void DatProcessor::Save_message(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages)
    {
        m_saver->Save_data(packet_seq_num, mdp_messages);
    }
    
}

