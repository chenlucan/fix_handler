
#ifndef __FH_TMALPHA_REPLAY_MARKET_REPLAY_MARKET_H__
#define __FH_TMALPHA_REPLAY_MARKET_REPLAY_MARKET_H__

#include <vector>
#include "core/global.h"
#include "core/market/marketi.h"
#include "core/market/marketlisteneri.h"
#include "tmalpha/replay/replay_simulater.h"


namespace fh
{
namespace tmalpha
{
namespace replay
{
namespace market
{
    class ReplayMarket : public fh::core::market::MarketI
    {
        public:
            explicit ReplayMarket(fh::core::market::MarketListenerI *listener);
            virtual ~ReplayMarket();

        public:
            void Set_simulater(fh::tmalpha::replay::ReplaySimulater *simulater);

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

        private:
            fh::core::market::MarketListenerI *m_listener;
            fh::tmalpha::replay::ReplaySimulater *m_simulater;

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayMarket);
    };
}   // namespace market
}   // namespace trade
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_MARKET_REPLAY_MARKET_H__
