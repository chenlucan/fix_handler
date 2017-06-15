
#ifndef __FH_TMALPHA_TRADE_TMALPHA_TRADE_APPLICATION_H__
#define __FH_TMALPHA_TRADE_TMALPHA_TRADE_APPLICATION_H__

#include "core/global.h"
#include "core/strategy/strategy_communicator.h"
#include "core/market/marketlisteneri.h"
#include "tmalpha/trade/market/simulater_market.h"
#include "tmalpha/trade/exchange/simulater_exchange.h"
#include "tmalpha/trade/trade_simulater.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
    class TmalphaTradeApplication
    {
        public:
            explicit TmalphaTradeApplication(const std::string &app_setting_file = "trade_matching_settings.ini");
            virtual ~TmalphaTradeApplication();

        public:
            bool Start();
            void Stop();

        private:
            void Init(const std::string &app_setting_file);
            std::vector<::pb::ems::Order> Get_init_orders();

        private:
            fh::core::market::MarketListenerI *m_market_listener;
            fh::core::strategy::StrategyCommunicator *m_strategy;
            std::thread *m_strategy_thread;
            TradeAlgorithm *m_algorithm;
            TradeSimulater *m_trade_simulater;
            fh::tmalpha::trade::market::SimulaterMarket *m_market;
            fh::tmalpha::trade::exchange::SimulaterExchange *m_exchange;

        private:
            DISALLOW_COPY_AND_ASSIGN(TmalphaTradeApplication);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TMALPHA_TRADE_APPLICATION_H__
