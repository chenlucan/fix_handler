#include "tmalpha/trade/market/simulater_market.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
namespace market
{

    SimulaterMarket::SimulaterMarket(fh::core::market::MarketListenerI *listener)
    : fh::core::market::MarketI(listener), m_listener(listener), m_simulater(nullptr)
    {
        // noop
    }

    SimulaterMarket::~SimulaterMarket()
    {
        // noop
    }

    void SimulaterMarket::Set_simulater(fh::tmalpha::trade::TradeSimulater *simulater)
    {
        m_simulater = simulater;
    }

    // implement of MarketI
    bool SimulaterMarket::Start()
    {
        m_simulater->Set_market_listener(m_listener);
        return true;
    }

    // implement of MarketI
    void SimulaterMarket::Initialize(std::vector<std::string> insts)
    {
        // noop
    }
    // implement of MarketI
    void SimulaterMarket::Stop()
    {
        m_simulater->Set_market_listener(nullptr);
    }

    // implement of MarketI
    void SimulaterMarket::Subscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void SimulaterMarket::UnSubscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void SimulaterMarket::ReqDefinitions(std::vector<std::string> instruments)
    {
        // noop
    }

}   // namespace market
}   // namespace trade
}   // namespace tmalpha
}   // namespace fh

