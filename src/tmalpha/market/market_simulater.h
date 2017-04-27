
#ifndef __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
#define __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__

#include <string>
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
            MarketSimulater(DataProvider *provider, DataConsumer *replayer, MarketReplayListener *replay_listener);
            virtual ~MarketSimulater();

        public:
            void Rate(float rate);
            void Start();
            void Stop();
            bool Is_runing();

        private:
            void Replay();
            void Show_states();
            void Consume_one();
            void Sleep(std::uint64_t last_send_time, std::uint64_t current_send_time, std::uint64_t last_replay_time) const;
            static std::uint64_t Get_message_insert_time(const std::string &last_message);
            static std::uint64_t Get_message_send_time(const std::string &next_message);

        private:
            fh::tmalpha::market::DataProvider *m_provider;
            fh::tmalpha::market::DataConsumer *m_replayer;
            fh::tmalpha::market::MarketReplayListener *m_replay_listener;
            std::queue<std::string> m_messages;
            std::atomic_bool m_is_fetch_end;
            std::atomic_bool m_is_stopped;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            float m_rate;       // 重放按照几倍速率进行：0.5 -> 慢 2 倍；2 -> 快 2 倍

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketSimulater);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
