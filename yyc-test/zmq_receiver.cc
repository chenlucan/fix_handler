
#include "zmq_receiver.h"
#include "logger.h"

namespace rczg
{
    
    ZmqReceiver::ZmqReceiver(const char *url) : m_context(1), m_receiver(m_context, ZMQ_PULL)
    {
        m_receiver.connect(url);
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
        static auto start = std::chrono::high_resolution_clock::now();

        size_t size = message.size();
        char *data = static_cast<char*>(message.data());

        auto finish = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        start = finish;

        rczg::Logger::Trace("received:", ns, "ns, len=", size, ", data=", data);
    }
    
}