
#ifndef __ZMQ_SENDER_H__
#define __ZMQ_SENDER_H__

#include "global.h"
#include <zmq.hpp>

namespace rczg
{
    class ZmqSender
    {
        public:
            explicit ZmqSender(const char *url);
            ~ZmqSender();
            
        public:
            void Send(const char *message, size_t length);
            
        private:
            
        private:
            zmq::context_t m_context;
            zmq::socket_t m_sender;
    };
}

#endif // __ZMQ_SENDER_H__