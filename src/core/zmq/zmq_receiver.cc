
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

    ZmqReceiver::ZmqReceiver(const std::string &url) : m_context(1), m_receiver(m_context, ZMQ_PULL)
    {
        m_receiver.connect(url);
        LOG_DEBUG("(zmq receiver started: ", url, ")");
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
