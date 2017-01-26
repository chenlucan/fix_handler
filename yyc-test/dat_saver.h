
#ifndef __DAT_SAVER_H__
#define __DAT_SAVER_H__

#include "global.h"
#include "mdp_message.h"
#include "zmq_sender.h"

namespace rczg
{
    class DatSaver
    {
        public:
            explicit DatSaver(rczg::ZmqSender &sender);
            virtual ~DatSaver();
            
        public:
            // save mdp messages to memory
            virtual void Save_data(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages);
            // process the messages in memory and serialize to zeroqueue
            virtual void Start_serialize();
            // set whether start join in middle week
            void Set_later_join(bool is_lj);
            
        private:
            // pick first message to serialize and remove it from memory
            std::multiset<rczg::MdpMessage>::iterator Pick_next_message_to_serialize();
            bool Convert_message(std::multiset<rczg::MdpMessage>::iterator message);
            void Send(std::multiset<rczg::MdpMessage>::iterator message);
            void Remove_past_message(std::multiset<rczg::MdpMessage>::iterator message);
            struct Message_Compare
            {
                bool operator() (const rczg::MdpMessage &a, const rczg::MdpMessage &b)
                {
                    return a.packet_seq_num() < b.packet_seq_num();
                }
            };
            
        private:
            std::uint32_t m_last_seq;
            std::set<std::uint32_t> m_unreceived_seqs;
            std::mutex m_mutex;
            std::multiset<rczg::MdpMessage, Message_Compare> m_datas;
            rczg::ZmqSender *m_sender;
            
        private:
            DISALLOW_COPY_AND_ASSIGN(DatSaver);
    };   
}

#endif // __DAT_SAVER_H__