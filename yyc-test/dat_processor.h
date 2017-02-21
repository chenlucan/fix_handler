
#ifndef __DAT_PROCESSOR_H__
#define __DAT_PROCESSOR_H__

#include "global.h"
#include "mdp_message.h"
#include "dat_arbitrator.h"
#include "dat_saver.h"
#include "dat_replayer.h"

namespace rczg
{
    class DatProcessor
    {
        public:
            DatProcessor(rczg::DatSaver &, rczg::DatReplayer &);
            virtual ~DatProcessor();
            
        public:
            // process udp feed data to mdp messages and save it
            virtual void Process_feed_data(char *buffer, const size_t data_length);
            // process tcp replay data to mdp messages and save it
            virtual void Process_replay_data(char *buffer, const size_t data_length);
            // set whether start join in middle week
            void Set_later_join(bool is_lj);
            
        private:
            void Process_data(char *buffer, const size_t data_length);
            void Start_tcp_replay(std::uint32_t begin, std::uint32_t end);
            void Save_message(std::uint32_t packet_seq_num, std::vector<rczg::MdpMessage> &mdp_messages);
            
        private:
            rczg::DatArbitrator m_arbitrator;
            rczg::DatSaver *m_saver;
            rczg::DatReplayer *m_replayer;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatProcessor);
    };   
}

#endif // __DAT_PROCESSOR_H__
