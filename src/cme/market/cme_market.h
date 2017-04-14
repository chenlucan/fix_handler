
#ifndef __FH_CME_MARKET_CME_MARKET_H__
#define __FH_CME_MARKET_CME_MARKET_H__

#include <unordered_map>
#include "core/global.h"
#include "cme/market/setting/market_settings.h"
#include "cme/market/setting/channel_settings.h"
#include "core/market/marketi.h"
#include "core/market/marketlisteneri.h"
#include "cme/market/market_manager.h"

namespace fh
{
namespace cme
{
namespace market
{

    class CmeMarket : public fh::core::market::MarketI
    {
        public:
            CmeMarket(
                    fh::core::market::MarketListenerI *listener,
                    const std::vector<fh::cme::market::setting::Channel> &channels,
                    const fh::cme::market::setting::MarketSettings &settings);
            virtual ~CmeMarket();

        public:
            // implement of MarketI
            bool Start() override;
            // implement of MarketI
            void Initialize(std::vector<std::string> insts) override;
            // implement of MarketI
            void Stop() override;
            // implement of MarketI
            void Subscribe(std::vector<std::string> instruments) override;
            // implement of MarketI
            void UnSubscribe(std::vector<std::string> instruments) override;
            // implement of MarketI
            void ReqDefinitions(std::vector<std::string> instruments) override;

        public:
            void Remove_market(const std::string &channel_id);

        private:
            void Initial_market(
                    fh::core::market::MarketListenerI *listener,
                    const std::vector<fh::cme::market::setting::Channel> &channels,
                    const fh::cme::market::setting::MarketSettings &settings);

        private:
            std::unordered_map<std::string, MarketManager *> m_market_managers;

        private:
            DISALLOW_COPY_AND_ASSIGN(CmeMarket);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_CME_MARKET_H__
