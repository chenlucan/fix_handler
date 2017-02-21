
#ifndef __MDP_RECEIVER_H__
#define __MDP_RECEIVER_H__

#include "global.h"
#include "zmq_receiver.h"

namespace rczg
{
    class MdpReceiver : public ZmqReceiver
    {
        public:
            explicit MdpReceiver(const char *url);
            virtual ~MdpReceiver();
            
        public:
            virtual void Save(char *data, size_t size);
            
        private:
        	DISALLOW_COPY_AND_ASSIGN(MdpReceiver);
    };
}

#endif // __MDP_RECEIVER_H__
