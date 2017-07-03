#include "tmalpha/replay/market/replay_market.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{
namespace market
{

    ReplayMarket::ReplayMarket(fh::core::market::MarketListenerI *listener)
    : fh::core::market::MarketI(listener), m_listener(listener), m_simulater(nullptr)
    {
        // noop
    }

    ReplayMarket::~ReplayMarket()
    {
        // noop
    }

    void ReplayMarket::Set_simulater(fh::tmalpha::replay::ReplaySimulater *simulater)
    {
        m_simulater = simulater;
    }

    // implement of MarketI
    bool ReplayMarket::Start()
    {
        m_simulater->Set_market_listener(m_listener);
        return true;
    }

    // implement of MarketI
    void ReplayMarket::Initialize(std::vector<std::string> insts)
    {
        // noop
    }
    // implement of MarketI
    void ReplayMarket::Stop()
    {
        m_simulater->Set_market_listener(nullptr);
    }

    // implement of MarketI
    void ReplayMarket::Subscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void ReplayMarket::UnSubscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void ReplayMarket::ReqDefinitions(std::vector<std::string> instruments)
    {
        // noop
    }

}   // namespace market
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

