
#ifndef __FH_TMALPHA_REPLAY_TMALPHA_REPLAY_APPLICATION_H__
#define __FH_TMALPHA_REPLAY_TMALPHA_REPLAY_APPLICATION_H__

#include <thread>
#include "core/global.h"
#include "core/assist/settings.h"
#include "core/strategy/strategy_communicator.h"
#include "tmalpha/replay/replay_simulater.h"
#include "tmalpha/replay/exchange/replay_exchange.h"
#include "tmalpha/replay/market/replay_market.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{
    class TmalphaReplayApplication
    {
        public:
            explicit TmalphaReplayApplication(
                    const std::string &app_setting_file = TRADE_MATCHING_SETTINGS_INI,
                    const std::string &persist_setting_file = PERSIST_SETTINGS_INI);
            virtual ~TmalphaReplayApplication();

        public:
            bool Start();
            void Join();
            void Stop();

        private:
            void Init(const fh::core::assist::Settings &app_settings, const fh::core::assist::Settings &persist_setting_file);
            std::vector<::pb::ems::Order> Get_init_orders();

        private:
            fh::core::market::MarketListenerI *m_listener;
            fh::core::persist::DataProvider *m_provider;
            ReplaySimulater *m_simulater;
            fh::core::strategy::StrategyCommunicator *m_strategy;
            std::thread *m_strategy_thread;
            fh::tmalpha::replay::market::ReplayMarket *m_market;
            fh::tmalpha::replay::exchange::ReplayExchange *m_exchange;

        private:
            DISALLOW_COPY_AND_ASSIGN(TmalphaReplayApplication);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_TMALPHA_REPLAY_APPLICATION_H__
