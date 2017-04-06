
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
        std::for_each(m_market_managers.cbegin(), m_market_managers.cend(), [](const MarketManager *m){ delete m;});
    }

    void CmeMarket::Initial_market(
            fh::core::market::MarketListenerI *listener,
            const std::vector<fh::cme::market::setting::Channel> &channels,
            const fh::cme::market::setting::MarketSettings &settings)
    {
        std::for_each(channels.cbegin(), channels.cend(), [this, &listener, &settings](const fh::cme::market::setting::Channel &c){
            LOG_DEBUG("create market ", c.id);
            MarketManager *market = new MarketManager(listener, c, settings);
            m_market_managers.push_back(market);
        });
    }

    // implement of MarketI
    bool CmeMarket::Start()
    {
        std::for_each(m_market_managers.begin(), m_market_managers.end(), [](MarketManager *m){
            m->Start();
        });
        return true;
    }

    // implement of MarketI
    bool CmeMarket::Join()
    {
        std::for_each(m_market_managers.begin(), m_market_managers.end(), [](MarketManager *m){
            m->Join();
        });
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
        std::for_each(m_market_managers.begin(), m_market_managers.end(), [](MarketManager *m){
            m->Stop();
        });
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

} // namespace market
} // namespace cme
} // namespace fh
