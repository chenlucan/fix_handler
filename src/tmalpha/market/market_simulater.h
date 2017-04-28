
#ifndef __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
#define __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__

#include <string>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "core/global.h"
#include "tmalpha/market/data_provider.h"
#include "tmalpha/market/data_consumer.h"
#include "tmalpha/market/market_replay_listener.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class MarketSimulater
    {
        public:
            MarketSimulater(DataProvider *provider, DataConsumer *replayer);
            virtual ~MarketSimulater();

        public:
            void Add_replay_listener(fh::tmalpha::market::MarketReplayListener *);
            void Speed(float speed);
            void Start();
            void Stop();
            bool Is_runing() const;

        private:
            void Replay();
            void Show_states();
            void Consume_one();
            void Sleep(std::uint64_t last_send_time, std::uint64_t current_send_time, std::uint64_t last_replay_time) const;

        private:
            fh::tmalpha::market::DataProvider *m_provider;
            fh::tmalpha::market::DataConsumer *m_replayer;
            std::vector<fh::tmalpha::market::MarketReplayListener *> m_replay_listeners;
            std::queue<std::string> m_messages;
            std::atomic_bool m_is_fetch_end;
            std::atomic_bool m_is_stopped;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            float m_speed;       // 重放按照几倍速率进行：0.5 -> 慢 2 倍；2 -> 快 2 倍

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketSimulater);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
