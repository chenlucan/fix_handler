
#include <stdexcept>
#include "core/zmq/zmq_sender.h"
#include "core/assist/utility.h"
#include "core/assist/logger.h"

namespace fh
{
namespace core
{
namespace zmq
{

    ZmqSender::ZmqSender(const std::string &url) : m_context(1), m_sender(m_context, ZMQ_PUSH)
    {
        try
        {
            m_sender.bind(url);
        }
        catch(std::exception& e)
        {
            LOG_ERROR("bind to ", url, " error: ", e.what());
            throw std::invalid_argument("bind error, exit");
        }
    }

    ZmqSender::~ZmqSender()
    {
        m_sender.close();
        m_context.close();
    }

    void ZmqSender::Send(const char *message, size_t length)
    {
        // 使用当前时间当作 message id
        std::uint64_t now = fh::core::assist::utility::Current_time_ns();

        ::zmq::message_t zmsg(length + sizeof(now));    // 附加 bytes 信息来保存 message id
        memcpy((char *)zmsg.data(), &now, sizeof(now));
        memcpy((char *)zmsg.data() + sizeof(now), message, length);

        bool isSuccess = m_sender.send(zmsg, ZMQ_NOBLOCK);

        LOG_DEBUG("message sent ", (isSuccess ? "success" : "failure"), ", id=", now);
    }

    void ZmqSender::Send(const std::string &message)
    {
        this->Send(message.data(), message.size());
    }
} // namespace zmq
} // namespace core
} // namespace fh
