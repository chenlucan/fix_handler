
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "global.h"

namespace rczg
{
namespace utility
{
    // get current time in nanoseconds since epoch
    std::uint64_t Current_time_ns();
    
    // translate each char in buffer to hex format for display
    // ex: "abc" -> "61 62 63 "
    std::string Hex_str(const char *buffer, std::size_t length);
    
    // generate random number between min and max
    std::uint32_t Random_number(std::uint32_t min, std::uint32_t max);
    
    // make a mdp message for test
    std::size_t make_message(char *position);
    
    // make a mdp packet for test
    std::size_t make_packet(char *buffer, std::uint32_t packet_seq_num);
}   
}

#endif //  __UTILITY_H__