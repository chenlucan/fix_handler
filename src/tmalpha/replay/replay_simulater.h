
#ifndef __FH_TMALPHA_REPLAY_REPLAY_SIMULATER_H__
#define __FH_TMALPHA_REPLAY_REPLAY_SIMULATER_H__

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "core/global.h"
#include "core/market/marketlisteneri.h"
#include "core/exchange/exchangelisteneri.h"
#include "tmalpha/replay/replay_data_consumer.h"
#include "tmalpha/replay/replay_data_provider.h"
#include "tmalpha/replay/replay_listener.h"
#include "tmalpha/replay/replay_order_matcher.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{
    class ReplaySimulater
    {
        public:
            explicit ReplaySimulater(fh::tmalpha::market::DataProvider *provider, int trade_rate = 100);
            virtual ~ReplaySimulater();

        public:
            // 设置交易监听器
            void Set_exchange_listener(core::exchange::ExchangeListenerI *exchange_listener);
            // 设置行情监听器
            void Set_market_listener(fh::core::market::MarketListenerI *market_listener);
            // L2 行情变化时
            void On_state_changed(const pb::dms::L2 &l2, std::uint32_t bid_volumn = 0, std::uint32_t ask_volumn = 0);

        public:
            bool Start();
            void Stop();
            void Join();
            void Speed(float speed);
            bool Is_runing() const;

        public:
            void Add(const ::pb::ems::Order& order) ;
            void Change(const ::pb::ems::Order& order) ;
            void Delete(const ::pb::ems::Order& order) ;
            void Query(const ::pb::ems::Order& order) ;

        private:
            void Read();
            void Replay();
            void Consume_one();
            void Sleep(std::uint64_t last_send_time, std::uint64_t current_send_time, std::uint64_t last_replay_time) const;

        private:
            ReplayOrderMatcher *m_matcher;
            ReplayDataConsumer *m_consumer;
            fh::tmalpha::market::DataProvider *m_provider;
            std::queue<std::string> m_messages;
            std::atomic_bool m_is_fetch_end;
            std::atomic_bool m_is_stopped;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            float m_speed;       // 重放按照几倍速率进行：0.5 -> 慢 2 倍；2 -> 快 2 倍
            std::thread *m_replayer;
            std::thread *m_reader;

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplaySimulater);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_REPLAY_SIMULATER_H__
