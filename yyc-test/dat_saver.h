
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
            
        private:
            // pick first message to serialize and remove it from memory
            bool Pick_next_message_to_serialize(std::string &target_message);
            bool Convert_message(std::string &message);
            void Send(std::string &message);
            
        private:
            std::uint32_t m_last_seq;
            std::set<std::uint32_t> m_unreceived_seqs;
            std::mutex m_mutex;
            std::multiset<std::pair<std::uint32_t, std::string>> m_datas;
            rczg::ZmqSender *m_sender;
            
        private:
            DISALLOW_COPY_AND_ASSIGN(DatSaver);
    };   
}

#endif // __DAT_SAVER_H__