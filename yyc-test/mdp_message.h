#ifndef __MDP_MESSAGE_H__
#define __MDP_MESSAGE_H__

#include "global.h"
#include "mktdata.h"

namespace rczg
{
    class MdpMessage
    {
        public:
            MdpMessage(); 
            MdpMessage(MdpMessage &&m);
            virtual ~MdpMessage(); 
            
        public:
            // copy the message out from socket buffer
            std::pair<std::uint32_t, std::string> Copy_out() const;
            
        public:
            size_t packet_length;
            std::uint32_t packet_seq_num;
            std::uint64_t packet_sending_time;
            bool is_valid_packet;
            std::uint16_t message_length;
            std::shared_ptr<void> message_header;
            std::shared_ptr<void> message_body;
        
        private:
            DISALLOW_COPY_AND_ASSIGN(MdpMessage);
    };    
}


#endif // __MDP_MESSAGE_H__