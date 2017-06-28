
#include "core/zmq/zmq_receiver.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace core
{
namespace zmq
{

    ZmqReceiver::ZmqReceiver(const std::string &url, Mode mode)
    : m_context(1), m_receiver(m_context, mode == Mode::PULL ? ZMQ_PULL : ZMQ_SUB)
    {
        // 接受消息时可能是 bind，也可能是 connect 方式
        // 根据配置文件配置的 url 来判断，如果是 tcp://*:9999 的形式，就是 bind，否则是 connect
        bool is_bind_address = url.find("//*:") != std::string::npos;
        try
        {
            if(is_bind_address) m_receiver.bind(url);
            else m_receiver.connect(url);

            // 如果是订阅模式，一定要设置 filter：这里不过滤消息，所以设置为空
            if(mode == Mode::SUBSCRIBE) m_receiver.setsockopt(ZMQ_SUBSCRIBE, "", 0);

            LOG_DEBUG("(zmq receiver started: ", url, ")");
        }
        catch(std::exception& e)
        {
            LOG_ERROR(is_bind_address ? "bind " : "connect ", url, " error: ", e.what());
            throw std::invalid_argument("zmqreceiver create error, exit");
        }
    }

    ZmqReceiver::~ZmqReceiver()
    {
        m_receiver.close();
        m_context.close();
    }

    void ZmqReceiver::Start_receive()
    {
        try
        {
            while(true)
            {
                ::zmq::message_t message;
                m_receiver.recv(&message);
                this->Process_message(message);
            }
        }
        catch( ::zmq::error_t &e)
        {
            LOG_DEBUG("(zmq receiver stopped)");
        }
    }

    void ZmqReceiver::Process_message(::zmq::message_t &message)
    {
        static fh::core::assist::TimeMeasurer t;

        size_t size = message.size();
        char *data = (char*)message.data();

        if(size == 0)
        {
            LOG_TRACE("received: empty");
        }
        else
        {
            LOG_TRACE("received: id=", *(std::uint64_t *)data);
            this->Save(data + sizeof(std::uint64_t), size - sizeof(std::uint64_t));
        }

        LOG_TRACE("zmq received used: ", t.Elapsed_nanoseconds(), "ns");
    }

} // namespace zmq
} // namespace core
} // namespace fh
