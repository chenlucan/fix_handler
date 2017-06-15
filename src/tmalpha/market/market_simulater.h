
#ifndef __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
#define __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "core/global.h"
#include "core/market/marketi.h"
#include "core/market/marketlisteneri.h"
#include "tmalpha/market/data_provider.h"
#include "tmalpha/market/data_consumer.h"


namespace fh
{
namespace tmalpha
{
namespace market
{
    class MarketSimulater : public fh::core::market::MarketI
    {
        public:
            MarketSimulater(fh::core::market::MarketListenerI *listener, DataProvider *provider, DataConsumer *consumer);
            virtual ~MarketSimulater();

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
            void Join();
            void Speed(float speed);
            bool Is_runing() const;
            fh::core::market::MarketListenerI * Listener();

        private:
            void Read();
            void Replay();
            void Consume_one();
            void Sleep(std::uint64_t last_send_time, std::uint64_t current_send_time, std::uint64_t last_replay_time) const;

        private:
            fh::tmalpha::market::DataProvider *m_provider;
            fh::tmalpha::market::DataConsumer *m_consumer;
            fh::core::market::MarketListenerI *m_listener;
            std::queue<std::string> m_messages;
            std::atomic_bool m_is_fetch_end;
            std::atomic_bool m_is_stopped;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            float m_speed;       // 重放按照几倍速率进行：0.5 -> 慢 2 倍；2 -> 快 2 倍
            std::thread *m_replayer;
            std::thread *m_reader;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketSimulater);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MARKET_SIMULATER_H__
