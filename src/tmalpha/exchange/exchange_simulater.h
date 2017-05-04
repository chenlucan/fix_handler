
#ifndef __FH_TMALPHA_EXCHANGE_EXCHANGE_SIMULATER_H__
#define __FH_TMALPHA_EXCHANGE_EXCHANGE_SIMULATER_H__

#include <string>
#include <atomic>
#include <map>
#include <list>
#include <mutex>
#include "core/global.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/market/market_simulater.h"


namespace fh
{
namespace tmalpha
{
namespace exchange
{
    class ExchangeSimulater : public core::exchange::ExchangeI
    {
        public:
            ExchangeSimulater(
                    fh::tmalpha::market::MarketSimulater *market,
                    fh::core::exchange::ExchangeListenerI *result_listener);
            virtual ~ExchangeSimulater();

        public:
            // implement of ExchangeI
            bool Start(const std::vector<::pb::ems::Order> &init_orders) override;
            // implement of ExchangeI
            void Stop() override;

        public:
            void Join();

        public:
            // implement of ExchangeI
            void Initialize(std::vector<::pb::dms::Contract> contracts) override;
            // implement of ExchangeI
            void Add(const ::pb::ems::Order& order) override;
            // implement of ExchangeI
            void Change(const ::pb::ems::Order& order) override;
            // implement of ExchangeI
            void Delete(const ::pb::ems::Order& order) override;
            // implement of ExchangeI
            void Query(const ::pb::ems::Order& order) override;
            // implement of ExchangeI
            void Query_mass(const char *data, size_t size) override;
            // implement of ExchangeI
            void Delete_mass(const char *data, size_t size) override;

        private:
            void Init(const fh::core::assist::Settings &app_settings, const fh::core::assist::Settings &persist_settings);
            pb::ems::Fill Order_filled(const ::pb::ems::Order& org_order, std::string next_exchange_order_id);
            pb::ems::Order Order_working(const ::pb::ems::Order& org_order);
            std::string Next_exchange_order_id();
            std::string Next_fill_id();
            bool Has_matching(const ::pb::ems::Order& org_order);
            void Rematching();
            void On_state_changed(const pb::dms::L2 &l2);

        private:
            fh::tmalpha::market::MarketSimulater *m_market;
            core::exchange::ExchangeListenerI *m_result_listener;
            std::vector<::pb::ems::Order> m_init_orders;
            std::atomic<std::uint32_t> m_exchange_order_id;
            std::atomic<std::uint32_t> m_fill_id;
            std::map<std::string, pb::ems::Order> m_working_orders;
            std::list<std::string> m_working_order_ids;   // 为了保证 working order 按照时间顺序匹配
            std::map<std::string, pb::ems::Fill> m_filled_orders;
            std::mutex m_mutex;
            std::unordered_map<std::string , pb::dms::L2> m_current_states;

        private:
            DISALLOW_COPY_AND_ASSIGN(ExchangeSimulater);
    };
} // namespace exchange
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_EXCHANGE_EXCHANGE_SIMULATER_H__
