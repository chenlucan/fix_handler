
#include "zmq_receiver.h"
#include "sbe_decoder.h"
#include "time_measurer.h"
#include "logger.h"
#include "utility.h"

namespace rczg
{
    
    ZmqReceiver::ZmqReceiver(const char *url) : m_context(1), m_receiver(m_context, ZMQ_PULL)
    {
        m_receiver.connect(url);
        LOG_DEBUG("(zmq receiver started)");
    }
    
    ZmqReceiver::~ZmqReceiver()
    {
        // noop
    }
    
    void ZmqReceiver::Start_receive()
    {
        while(true)
        {
            zmq::message_t message;
            m_receiver.recv(&message);
            this->Process_message(message);
        }
    }
    
    void ZmqReceiver::Process_message(zmq::message_t &message)
    {
        static TimeMeasurer t;

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

}







