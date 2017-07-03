
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

    ZmqSender::ZmqSender(const std::string &url, Mode mode)
    : m_context(1), m_sender(m_context, mode == Mode::PUSH ? ZMQ_PUSH : ZMQ_PUB)
    {
        // url 没有设定的场合就不初期化了
        if(url == "") return;

        // 发送消息时可能是 bind，也可能是 connect 方式
        // 根据配置文件配置的 url 来判断，如果是 tcp://*:9999 的形式，就是 bind，否则是 connect
        bool is_bind_address = url.find("//*:") != std::string::npos;
        try
        {
            if(is_bind_address) m_sender.bind(url);
            else m_sender.connect(url);

            LOG_DEBUG("(zmq sender created: ", url, ")");
        }
        catch(std::exception& e)
        {
            LOG_ERROR(is_bind_address ? "bind " : "connect ", url, " error: ", e.what());
            throw std::invalid_argument("zmq sender create error, exit");
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
