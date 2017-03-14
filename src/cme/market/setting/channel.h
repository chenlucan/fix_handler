
#ifndef __FH_CME_MARKET_SETTING_CHANNEL_H__
#define __FH_CME_MARKET_SETTING_CHANNEL_H__

#include <boost/asio.hpp>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace market
{
    namespace setting
    {
        // Historical Replay, Incremental, Instrument Replay, Snapshot
        enum class FeedType { H, I, N, S };

        // TCP/IP, UDP/IP
        enum class Protocol { TCP, UDP };

        // A, B
        enum class Feed { A, B };

        struct Group
        {
            std::string code;
        };

        struct Product
        {
            std::string code;
            Group group;
        };

        struct Connection
        {
            std::string id;
            FeedType type;
            std::string type_des;
            Protocol protocol;
            boost::asio::ip::address ip;
            boost::asio::ip::address host_ip;
            std::uint16_t port;
            Feed feed;
        };

        struct Channel
        {
            std::string id;
            std::string label;
            std::vector<Product> products;
            std::vector<Connection> connections;
        };   
    } // namespace setting
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_SETTING_CHANNEL_H__
