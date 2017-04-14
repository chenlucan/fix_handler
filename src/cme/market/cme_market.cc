
#include "cme/market/cme_market.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    CmeMarket::CmeMarket(
            fh::core::market::MarketListenerI *listener,
            const std::vector<fh::cme::market::setting::Channel> &channels,
            const fh::cme::market::setting::MarketSettings &settings)
    : fh::core::market::MarketI(listener), m_market_managers()
    {
        Initial_market(listener, channels, settings);
    }

    CmeMarket::~CmeMarket()
    {
        for(const auto &m : m_market_managers) { delete m.second; }
    }

    void CmeMarket::Initial_market(
            fh::core::market::MarketListenerI *listener,
            const std::vector<fh::cme::market::setting::Channel> &channels,
            const fh::cme::market::setting::MarketSettings &settings)
    {
        std::for_each(channels.cbegin(), channels.cend(), [this, &listener, &settings](const fh::cme::market::setting::Channel &c){
            LOG_DEBUG("create market ", c.id);
            MarketManager *market = new MarketManager(this, listener, c, settings);
            m_market_managers[c.id] = market;
        });
    }

    // implement of MarketI
    bool CmeMarket::Start()
    {
        for(const auto &m : m_market_managers) { m.second->Start(); }
        return true;
    }

    // implement of MarketI
    void CmeMarket::Initialize(std::vector<std::string> insts)
    {
        // noop
    }

    // implement of MarketI
    void CmeMarket::Stop()
    {
        for(const auto &m : m_market_managers) { m.second->Stop(); }
    }

    // implement of MarketI
    void CmeMarket::Subscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void CmeMarket::UnSubscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void CmeMarket::ReqDefinitions(std::vector<std::string> instruments)
    {
        // noop
    }

    void CmeMarket::Remove_market(const std::string &channel_id)
    {
        m_market_managers.erase(channel_id);

        if(m_market_managers.empty())
        {
            LOG_INFO("all market stopped. exit.");
            exit(0);
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
