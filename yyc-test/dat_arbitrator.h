
#ifndef __DAT_ARBITRATOR_H__
#define __DAT_ARBITRATOR_H__

#include "global.h"

namespace rczg
{
    class DatArbitrator
    {
        public:
            // start_sequence is received max sequence
            explicit DatArbitrator(std::uint32_t start_sequence);
            virtual ~DatArbitrator();
            
        public:
            // check whether a packet from udp feed is valid
            std::uint32_t Check_feed_packet(std::uint32_t packet_seq_num);
            // check whether a packet from tcp replayer is valid
            std::uint32_t Check_replay_packet(std::uint32_t packet_seq_num);
            
        private:
            
        private:
            std::uint32_t m_current_sequence;
            std::set<uint32_t> m_lost_sequences;
            std::mutex m_mutex;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatArbitrator);
    };   
}

#endif // __DAT_ARBITRATOR_H__