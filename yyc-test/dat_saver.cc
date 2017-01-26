
#include "dat_saver.h"
#include "logger.h"

namespace rczg
{
    DatSaver::DatSaver(rczg::ZmqSender &sender) 
    : m_last_seq(0), m_unreceived_seqs(), m_mutex(), m_datas(), m_sender(&sender)
    {
        // noop
    }

    void DatSaver::Set_later_join(bool is_lj)
    {
        if(is_lj)
        {
            // use max value as initial value
            m_last_seq = std::numeric_limits<std::uint32_t>::max();
        }
        else
        {
            m_last_seq = 0;
        }
    }
    
    // save mdp messages to memory
    void DatSaver::Save_data(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](rczg::MdpMessage &m){ m_datas.insert(std::move(m)); });
        
        if(m_last_seq == std::numeric_limits<std::uint32_t>::max())
        {
            // later joiner's first packet is treated as first
            m_last_seq = packet_seq_num;
        }
        else if(packet_seq_num < m_last_seq)
        {
            m_unreceived_seqs.erase(packet_seq_num);
        }
        else
        {
            m_unreceived_seqs.insert(
                boost::counting_iterator<std::uint32_t>(m_last_seq + 1), 
                boost::counting_iterator<std::uint32_t>(packet_seq_num));
            m_last_seq = packet_seq_num;
        }
    }
    
    // process the messages in memory and serialize to zeroqueue
    void DatSaver::Start_serialize()
    {
       while(true)
       {
           auto message = Pick_next_message_to_serialize();
           if(message != m_datas.end())
           {
               bool should_send_now = this->Convert_message(message);
               if(should_send_now)
               {
                   this->Send(message);
               }
               
               Remove_past_message(message);
           }
           else
           {
               std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
           }
       }
    }
    
    // pick first message to serialize and remove it from memory
    std::multiset<rczg::MdpMessage>::iterator DatSaver::Pick_next_message_to_serialize()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if(m_datas.empty())
        {
            // no message
            return m_datas.end();
        }
        
        auto message = m_datas.begin();
        std::uint32_t first_unreceived = m_unreceived_seqs.empty() ? 0 : *m_unreceived_seqs.begin();
        
        if(first_unreceived != 0 && message->packet_seq_num() >= first_unreceived)
        {
            // wait for next seq
            return m_datas.end();
        }
        
        return message;
    }
    
    bool DatSaver::Convert_message(std::multiset<rczg::MdpMessage>::iterator message)
    {
        // TODO convert the message
        return true;
    }
    
    void DatSaver::Send(std::multiset<rczg::MdpMessage>::iterator message)
    {
        static auto start = std::chrono::high_resolution_clock::now();
        
        // send to zeroqueue
        auto s = message->Serialize();
        m_sender->Send(s.data(), s.size());

        auto finish = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        rczg::Logger::Info("send to zmq: ", ns, "ns, ", s);
        start = finish;
    }
    
    void DatSaver::Remove_past_message(std::multiset<rczg::MdpMessage>::iterator message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_datas.erase(message);
    }
    
    DatSaver::~DatSaver()
    {
        // noop
    }
}