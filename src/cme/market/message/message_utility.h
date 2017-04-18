
#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_UTILITY_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_UTILITY_H__

#include "core/global.h"
#include "cme/market/message/mdp_message.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
namespace utility
{
    // split packet to messages
    std::uint32_t Pick_messages_from_packet(char *buffer, const size_t data_length, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages);

    // get message's template id from sbe packet
    std::uint16_t Get_sbe_template_id(const char *packet);

    // create fix logon message(35=A)
    std::string Make_fix_logon_message(const std::string &username, const std::string &password);

    // create fix replay request message(35=V)
    std::string Make_fix_recovery_request_message(const std::string &channel_id, const std::string &req_id, std::uint32_t begin, std::uint32_t end);

    // make a mdp message for test
    std::size_t Make_message(char *position, std::uint32_t *ids, size_t len);

    // make a mdp packet for test
    std::size_t Make_packet(char *buffer, std::uint32_t packet_seq_num, std::uint32_t *ids, size_t len);

    // convert price in sbe message to double
    double Get_price(std::pair<std::int64_t, std::int8_t> price_mantissa);
} // namespace utility
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif //  __FH_CME_MARKET_MESSAGE_MESSAGE_UTILITY_H__
