
#ifndef __TCP_RECEIVER_H__
#define __TCP_RECEIVER_H__

#include "global.h"

namespace rczg
{
    class TCPReceiver
    {
        public:
            TCPReceiver(const boost::asio::ip::address &server_address, const unsigned short server_port);
            virtual ~TCPReceiver();
            
        public:
            // request data([begin, end)) from tcp replay, and process it with processor
            void Start_receive(std::function<void(char *, const size_t)> processor, std::uint32_t begin, std::uint32_t end);
            
        private:
            DISALLOW_COPY_AND_ASSIGN(TCPReceiver);
    };
}
    
#endif // __TCP_RECEIVER_H__