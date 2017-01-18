
#include "dat_processor.h"
#include "sbe_decoder.h"

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
            //std::cout << "****** discard tcp: seq=" << packet_seq_num << std::endl;
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
            //std::cout << "****** discard udp: seq=" << packet_seq_num << std::endl;
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
            rczg::MdpMessage mdp;
            std::uint16_t message_length = this->Make_mdp_message(
                    mdp, data_length, packet_seq_num, packet_sending_time, current_position);
            mdp_messages.push_back(std::move(mdp));
            current_position = current_position + message_length;
        }
        
        this->Save_message(packet_seq_num, mdp_messages);
        
        auto finish = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        std::cout << "received: " << ns << "ns: " << std::this_thread::get_id();
        std::cout << " - " << packet_seq_num << ", len=" << data_length << std::endl;
    }

    void DatProcessor::Start_tcp_replay(std::uint32_t begin, std::uint32_t end)
    {
        std::thread t([this, begin, end]{
            m_replayer->Start_receive(
                std::bind(&rczg::DatProcessor::Process_replay_data, std::ref(*this), std::placeholders::_1, std::placeholders::_2),
                begin, end);
        });
        //std::cout << "start thread: " << begin << " - " << end << std::endl;
        t.detach();     // must make it unjoinable
    }
    
    std::uint16_t DatProcessor::Make_mdp_message(
        rczg::MdpMessage &mdp, 
        const size_t data_length, 
        const std::uint32_t packet_seq_num,
        const std::uint64_t packet_sending_time,
        char *current_position)
    {
        std::uint16_t message_length = *((std::uint16_t *)current_position);
        char *message = current_position + 2;
        rczg::SBEDecoder decoder(message, message_length);
        auto sbe_message = decoder.Start_decode();
        
        mdp.packet_length = data_length;
        mdp.packet_seq_num = packet_seq_num;
        mdp.packet_sending_time = packet_sending_time;
        mdp.is_valid_packet = true;
        mdp.message_length = message_length;
        mdp.message_header = sbe_message.first;
        mdp.message_body = sbe_message.second;
        
        return message_length + 2;
    }
    
    void DatProcessor::Make_mdp_invalid_message(
        rczg::MdpMessage &mdp, 
        const size_t data_length, 
        const std::uint32_t packet_seq_num,
        const std::uint64_t packet_sending_time)
    {
        mdp.packet_length = data_length;
        mdp.packet_seq_num = packet_seq_num;
        mdp.packet_sending_time = packet_sending_time;
        mdp.is_valid_packet = false;
    }
    
    void DatProcessor::Save_message(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages)
    {
        m_saver->Save_data(packet_seq_num, mdp_messages);
    }
    
}

