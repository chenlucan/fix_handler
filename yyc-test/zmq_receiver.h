
#ifndef __ZMQ_RECEIVER_H__
#define __ZMQ_RECEIVER_H__

#include "global.h"
#include <zmq.hpp>

namespace rczg
{
    class ZmqReceiver
    {
        public:
            explicit ZmqReceiver(const char *url);
            virtual ~ZmqReceiver();
            
        public:
            void Start_receive();
            
        private:
            void Process_message(zmq::message_t &message);
            
        private:
            zmq::context_t m_context;
            zmq::socket_t m_receiver;
    };
}

#endif // __ZMQ_RECEIVER_H__