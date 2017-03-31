
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

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnOrder(const ::pb::ems::Order &order)
    {
        LOG_INFO("send order result:  (O)", fh::core::assist::utility::Format_pb_message(order));
        m_sender.Send("O" + order.SerializeAsString());
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnFill(const ::pb::ems::Fill &fill)
    {
        LOG_INFO("send order result:  (F)", fh::core::assist::utility::Format_pb_message(fill));
        m_sender.Send("F" + fill.SerializeAsString());
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnPosition(const core::exchange::PositionVec& position)
    {

    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnExchangeReady(boost::container::flat_map<std::string, std::string>)
    {
        LOG_INFO("trade server is ready");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractAuctioning(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Auction]");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractNoTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [NoTrading]");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Trading]");
    }

} // namespace exchange
} // namespace cme
} // namespace fh
