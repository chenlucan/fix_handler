
#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "global.h"

namespace rczg
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
    }
}

#endif // __CHANNEL_H__