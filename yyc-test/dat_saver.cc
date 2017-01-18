
#include "dat_saver.h"

namespace rczg
{
    DatSaver::DatSaver(rczg::ZmqSender &sender) 
    : m_last_seq(0), m_unreceived_seqs(), m_mutex(), m_datas(), m_sender(&sender)
    {
        // noop
    }

    // save mdp messages to memory
    void DatSaver::Save_data(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), 
                      [this](const rczg::MdpMessage &m){ m_datas.insert(m.Copy_out()); });
        
        if(packet_seq_num < m_last_seq)
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
           std::string message;
           bool has_message = Pick_next_message_to_serialize(message);
           if(has_message)
           {
               bool should_send_now = this->Convert_message(message);
               if(should_send_now)
               {
                   this->Send(message);
               }
           }
           else
           {
               std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
           }
       }
    }
    
    // pick first message to serialize and remove it from memory
    bool DatSaver::Pick_next_message_to_serialize(std::string &target_message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if(m_datas.empty())
        {
            // no message
            //std::cout << "no message." << std::endl;
            return false;
        }
        
        std::pair<std::uint32_t, std::string> message = *m_datas.begin();
        std::uint32_t first_unreceived = m_unreceived_seqs.empty() ? 0 : *m_unreceived_seqs.begin();
        
        if(first_unreceived != 0 && message.first >= first_unreceived)
        {
            // wait for next seq
            //std::cout << "wait for " << first_unreceived << std::endl;
            return false;
        }
        
        target_message = message.second;
        m_datas.erase(m_datas.begin());
        
        return true;
    }
    
    bool DatSaver::Convert_message(std::string &message)
    {
        // TODO convert the message
        return true;
    }
    
    void DatSaver::Send(std::string &message)
    {
        static auto start = std::chrono::high_resolution_clock::now();
        
        // send to zeroqueue
        m_sender->Send(message.data(), message.size());

        auto finish = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        std::cout << "send to zmq: " << ns << "ns : " << message << std::endl;
        start = finish;
    }
    
    DatSaver::~DatSaver()
    {
        // noop
    }
}