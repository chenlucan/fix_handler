
#include "zmq_sender.h"

namespace rczg
{
    
    ZmqSender::ZmqSender(const char *url) : m_context(1), m_sender(m_context, ZMQ_PUSH)
    {
        m_sender.bind(url);
    }
    
    ZmqSender::~ZmqSender()
    {
        // noop
    }
    
    void ZmqSender::Send(const char *message, size_t length)
    {
        zmq::message_t zmsg(length + 1);
        snprintf((char *)zmsg.data(), length + 1, message);
        m_sender.send(zmsg);
    }
    
}
