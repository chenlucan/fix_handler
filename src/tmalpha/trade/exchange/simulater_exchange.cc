
#include "tmalpha/trade/exchange/simulater_exchange.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
namespace exchange
{

    SimulaterExchange::SimulaterExchange(core::exchange::ExchangeListenerI *exchange_listener)
    : core::exchange::ExchangeI(exchange_listener), m_exchange_listener(exchange_listener),
      m_init_orders(), m_simulater(nullptr)
    {
        // noop
    }

    SimulaterExchange::~SimulaterExchange()
    {
        // noop
    }

    void SimulaterExchange::Set_simulater(fh::tmalpha::trade::TradeSimulater *simulater)
    {
        m_simulater = simulater;
    }

    // implement of ExchangeI
    bool SimulaterExchange::Start(const std::vector<pb::ems::Order> &init_orders)
    {
        m_simulater->Set_exchange_listener(m_exchange_listener);
        return true;
    }

    // implement of ExchangeI
    void SimulaterExchange::Stop()
    {
        // noop
    }

    // implement of ExchangeI
    void SimulaterExchange::Initialize(std::vector<::pb::dms::Contract> contracts)
    {
        // noop
    }

    // implement of ExchangeI
    void SimulaterExchange::Add(const pb::ems::Order& order)
    {
        m_simulater->Add(order);
    }

    // implement of ExchangeI
    void SimulaterExchange::Change(const pb::ems::Order& order)
    {
        m_simulater->Change(order);
    }

    // implement of ExchangeI
    void SimulaterExchange::Delete(const pb::ems::Order& order)
    {
        m_simulater->Delete(order);
    }

    // implement of ExchangeI
    void SimulaterExchange::Query(const pb::ems::Order& order)
    {
        m_simulater->Query(order);
    }

    // implement of ExchangeI
    void SimulaterExchange::Query_mass(const char *data, size_t size)
    {
        // noop
    }

    // implement of ExchangeI
    void SimulaterExchange::Delete_mass(const char *data, size_t size)
    {
        // noop
    }

} // namespace exchange
} // namespace trade
} // namespace tmalpha
} // namespace fh
