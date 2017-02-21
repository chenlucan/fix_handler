
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "global.h"
#include "mdp_message.h"

namespace rczg
{
namespace utility
{
    // get current time in nanoseconds since epoch
    std::uint64_t Current_time_ns();
    
    // get current time in "yyyy-MM-dd HH:mm:ss.ssssss" format
    std::string Current_time_str();
    
    // translate each char in buffer to hex format for display
    // ex: "abc" -> "61 62 63 "
    std::string Hex_str(const char *buffer, std::size_t length);
    
    // generate random number between min and max
    std::uint32_t Random_number(std::uint32_t min, std::uint32_t max);
    
    // pad string on right with special character or delete character from right if size is longer than special
    std::string Pad_Right(const std::string &str, const std::size_t total_length, const char padding_char = ' ');

    // split packet to messages
    std::uint32_t Pick_messages_from_packet(char *buffer, const size_t data_length, std::vector<rczg::MdpMessage> &mdp_messages);

    // get message's template id from sbe packet
    std::uint16_t Get_sbe_template_id(const char *packet);

    // calculate fix message checksum
    std::string Calculate_fix_checksum(const std::string &message);

    // create fix logon message(35=A)
    std::string Make_fix_logon_message(const std::string &username, const std::string &password);

    // create fix replay request message(35=V)
    std::string Make_fix_recovery_request_message(const std::string &channel_id, const std::string &req_id, std::uint32_t begin, std::uint32_t end);

    // make a mdp message for test
    std::size_t Make_message(char *position, std::uint32_t *ids, size_t len);
    
    // make a mdp packet for test
    std::size_t Make_packet(char *buffer, std::uint32_t packet_seq_num, std::uint32_t *ids, size_t len);
}   
}

#endif //  __UTILITY_H__
