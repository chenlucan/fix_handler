
#ifndef __FH_TMALPHA_MARKET_TMALPHA_MARKET_APPLICATION_H__
#define __FH_TMALPHA_MARKET_TMALPHA_MARKET_APPLICATION_H__

#include <thread>
#include "core/global.h"
#include "core/assist/settings.h"
#include "tmalpha/market/market_simulater.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class TmalphaMarketApplication
    {
        public:
            explicit TmalphaMarketApplication(
                    const std::string &app_setting_file = "trade_matching_settings.ini",
                    const std::string &persist_setting_file = "persist_settings.ini");
            virtual ~TmalphaMarketApplication();

        public:
            bool Start();
            void Join();
            void Stop();

        private:
            void Init(const fh::core::assist::Settings &app_settings, const fh::core::assist::Settings &persist_setting_file);

        private:
            DataProvider *m_provider;
            DataConsumer *m_consumer;
            fh::core::market::MarketListenerI *m_listener;
            MarketSimulater *m_simulater;

        private:
            DISALLOW_COPY_AND_ASSIGN(TmalphaMarketApplication);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_TMALPHA_MARKET_APPLICATION_H__
