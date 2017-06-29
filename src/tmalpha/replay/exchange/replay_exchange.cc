
#include "tmalpha/replay/exchange/replay_exchange.h"


namespace fh
{
namespace tmalpha
{
namespace replay
{
namespace exchange
{

    ReplayExchange::ReplayExchange(core::exchange::ExchangeListenerI *exchange_listener)
    : core::exchange::ExchangeI(exchange_listener), m_exchange_listener(exchange_listener),
      m_init_orders(), m_simulater(nullptr)
    {
        // noop
    }

    ReplayExchange::~ReplayExchange()
    {
        // noop
    }

    void ReplayExchange::Set_simulater(fh::tmalpha::replay::ReplaySimulater *simulater)
    {
        m_simulater = simulater;
    }

    // implement of ExchangeI
    bool ReplayExchange::Start(const std::vector<pb::ems::Order> &init_orders)
    {
        m_simulater->Set_exchange_listener(m_exchange_listener);
        return true;
    }

    // implement of ExchangeI
    void ReplayExchange::Stop()
    {
        // noop
    }

    // implement of ExchangeI
    void ReplayExchange::Initialize(std::vector<::pb::dms::Contract> contracts)
    {
        // noop
    }

    // implement of ExchangeI
    void ReplayExchange::Add(const pb::ems::Order& order)
    {
        m_simulater->Add(order);
    }

    // implement of ExchangeI
    void ReplayExchange::Change(const pb::ems::Order& order)
    {
        m_simulater->Change(order);
    }

    // implement of ExchangeI
    void ReplayExchange::Delete(const pb::ems::Order& order)
    {
        m_simulater->Delete(order);
    }

    // implement of ExchangeI
    void ReplayExchange::Query(const pb::ems::Order& order)
    {
        m_simulater->Query(order);
    }

    // implement of ExchangeI
    void ReplayExchange::Query_mass(const char *data, size_t size)
    {
        // noop
    }

    // implement of ExchangeI
    void ReplayExchange::Delete_mass(const char *data, size_t size)
    {
        // noop
    }

} // namespace exchange
} // namespace replay
} // namespace tmalpha
} // namespace fh
