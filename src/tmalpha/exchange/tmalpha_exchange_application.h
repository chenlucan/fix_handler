
#ifndef __FH_TMALPHA_EXCHANGE_TMALPHA_EXCHANGE_APPLICATION_H__
#define __FH_TMALPHA_EXCHANGE_TMALPHA_EXCHANGE_APPLICATION_H__

#include <string>
#include <thread>
#include "core/global.h"
#include "core/strategy/strategy_communicator.h"
#include "tmalpha/exchange/exchange_simulater.h"
#include "tmalpha/exchange/market_replay_listener.h"


namespace fh
{
namespace tmalpha
{
namespace exchange
{
    class TmalphaExchangeApplication
    {
        public:
            explicit TmalphaExchangeApplication(
                    const std::string &app_setting_file = TRADE_MATCHING_SETTINGS_INI,
                    const std::string &persist_setting_file = PERSIST_SETTINGS_INI);
            virtual ~TmalphaExchangeApplication();

        public:
            bool Start();
            void Join();
            void Stop();

        private:
            void Init(const std::string &app_setting_file, const std::string &persist_setting_file);
            std::vector<::pb::ems::Order> Get_init_orders();

        private:
            fh::tmalpha::market::MarketSimulater *m_market;
            fh::tmalpha::exchange::MarketReplayListener *m_market_listener;
            fh::core::persist::DataProvider *m_provider;
            fh::tmalpha::market::DataConsumer *m_consumer;
            fh::core::strategy::StrategyCommunicator *m_strategy;
            ExchangeSimulater *m_exchange_simulater;
            std::thread *m_strategy_thread;

        private:
            DISALLOW_COPY_AND_ASSIGN(TmalphaExchangeApplication);
    };
} // namespace exchange
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_EXCHANGE_TMALPHA_EXCHANGE_APPLICATION_H__
