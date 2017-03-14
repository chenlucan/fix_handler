
#include "core/assist/logger.h"
#include "cme/exchange/strategy_communicator.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    StrategyReceiver::StrategyReceiver(const std::string &url) : fh::core::zmq::ZmqReceiver(url)
    {
        // noop
    }

    StrategyReceiver::~StrategyReceiver()
    {
        // noop
    }

    void StrategyReceiver::Set_processor(std::function<void(char *, const size_t)> processor)
    {
        m_processor = processor;
    }

    void StrategyReceiver::Save(char *data, size_t size)
    {
        LOG_DEBUG("received: ", std::string(data, size));
        m_processor(data, size);
    }

    StrategyCommunicator::StrategyCommunicator(const std::string &send_url, const std::string &receive_url)
    : m_sender(send_url), m_receiver(receive_url)
    {
        // noop
    }

    StrategyCommunicator::~StrategyCommunicator()
    {
        // noop
    }

    void StrategyCommunicator::Start_receive(std::function<void(char *, const size_t)> processor)
    {
        m_receiver.Set_processor(processor);
        m_receiver.Start_receive();
    }

    void StrategyCommunicator::Send(const char *message, size_t length)
    {
        LOG_DEBUG("send: ", std::string(message, length));
        m_sender.Send(message, length);
    }

    void StrategyCommunicator::Send(const std::string &message)
    {
        LOG_DEBUG("send: ", message);
        m_sender.Send(message);
    }

} // namespace exchange
} // namespace cme
} // namespace fh
