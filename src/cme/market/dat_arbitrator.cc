
#include <boost/iterator/counting_iterator.hpp>
#include "cme/market/dat_arbitrator.h"

namespace fh
{
namespace cme
{
namespace market
{

    DatArbitrator::DatArbitrator()
    : m_current_sequence(std::numeric_limits<std::uint32_t>::max()), m_lost_sequences(), m_mutex()
    {
        // noop
    }

    DatArbitrator::~DatArbitrator()
    {
        // noop
    }

    // check whether a packet from udp feed is valid
    // if the packet is already processed, return uint32_t max value
    // if there is no gap, return 0
    // or return first sequence in gap
    std::uint32_t DatArbitrator::Check_feed_packet(std::uint32_t packet_seq_num)
    {
        //std::lock_guard<std::mutex> lock(m_mutex);
        if(packet_seq_num == m_current_sequence + 1)
        {
            // normal packet
            m_current_sequence = packet_seq_num;
            return 0;
        }

        if(packet_seq_num == m_current_sequence)
        {
            // discard the packet
            return std::numeric_limits<std::uint32_t>::max();
        }

        if(m_current_sequence == std::numeric_limits<std::uint32_t>::max())
        {
            // first packet is treated as normal packet
            m_current_sequence = packet_seq_num;
            return 0;
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(packet_seq_num < m_current_sequence)
            {
                auto index = m_lost_sequences.find(packet_seq_num);
                if(index == m_lost_sequences.end())
                {
                    // discard the packet if it's sequence number is already processed
                    return std::numeric_limits<std::uint32_t>::max();
                }
                else
                {
                    // if it is a lost sequence, process it and remove from lost list
                    m_lost_sequences.erase(index);
                    return 0;
                }
            }
            else // packet_seq_num > m_current_sequence + 1
            {
                // gap detected
                std::uint32_t old_seq = m_current_sequence + 1;
                // save lost sequence number
                m_lost_sequences.insert(
                    boost::counting_iterator<std::uint32_t>(old_seq), 
                    boost::counting_iterator<std::uint32_t>(packet_seq_num));

                m_current_sequence = packet_seq_num;
                return old_seq;
            }
        }
    }

    // check whether a packet from tcp replayer is valid
    // if the packet is already processed, return uint32_t max value
    // or return 0 
    std::uint32_t DatArbitrator::Check_replay_packet(std::uint32_t packet_seq_num)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto index = m_lost_sequences.find(packet_seq_num);
        if(index == m_lost_sequences.end())
        {
            // discard the packet if it's sequence number is already processed
            return std::numeric_limits<std::uint32_t>::max();
        }
        else
        {
            // if it is a lost sequence, process it and remove from lost list
            m_lost_sequences.erase(index);
            return 0;
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
