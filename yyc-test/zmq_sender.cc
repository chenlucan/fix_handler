
#include "zmq_sender.h"
#include "utility.h"
#include "logger.h"

namespace rczg
{
    
    ZmqSender::ZmqSender(const char *url) : m_context(1), m_sender(m_context, ZMQ_PUSH)
    {
        m_sender.bind(url);
    }
    
    ZmqSender::ZmqSender(const std::string &url) : ZmqSender(url.data())
    {
    	// noop
    }

    ZmqSender::~ZmqSender()
    {
        // noop
    }
    
    void ZmqSender::Send(const char *message, size_t length)
    {
    	// 使用当前时间当作 message id
    	std::uint64_t now = rczg::utility::Current_time_ns();

        zmq::message_t zmsg(length + sizeof(now));	// 附加 bytes 信息来保存 message id
        memcpy((char *)zmsg.data(), &now, sizeof(now));
        memcpy((char *)zmsg.data() + sizeof(now), message, length);

        bool isSuccess = m_sender.send(zmsg);

        LOG_DEBUG("message sent ", (isSuccess ? "success" : "failure"), ", id=", now);
    }
    
    void ZmqSender::Send(const std::string &message)
    {
    	this->Send(message.data(), message.size());
    }
}
