
#ifndef __FH_CME_MARKET_DAT_ARBITRATOR_H__
#define __FH_CME_MARKET_DAT_ARBITRATOR_H__

#include <mutex>
#include <set>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DatArbitrator
    {
        public:
            DatArbitrator();
            virtual ~DatArbitrator();

        public:
            // check whether a packet from udp feed is valid
            std::uint32_t Check_feed_packet(std::uint32_t packet_seq_num);
            // check whether a packet from tcp replayer is valid
            std::uint32_t Check_replay_packet(std::uint32_t packet_seq_num);

        private:
            std::uint32_t m_current_sequence;
            std::set<uint32_t> m_lost_sequences;
            std::mutex m_mutex;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatArbitrator);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_ARBITRATOR_H__
