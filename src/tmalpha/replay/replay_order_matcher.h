
#ifndef __FH_TMALPHA_REPLAY_REPLAY_ORDER_MATCHER_H__
#define __FH_TMALPHA_REPLAY_REPLAY_ORDER_MATCHER_H__

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


namespace fh
{
namespace tmalpha
{
namespace replay
{
    class ReplayOrderMatcher
    {
        public:
            explicit ReplayOrderMatcher(int trade_rate);
            virtual ~ReplayOrderMatcher();

        public:
            void Add_exchange_listener(fh::core::exchange::ExchangeListenerI *result_listener);
            void On_state_changed(const pb::dms::L2 &l2, std::uint32_t bid_volumn = 0, std::uint32_t ask_volumn = 0);
            void Add(const ::pb::ems::Order& order) ;
            void Change(const ::pb::ems::Order& order) ;
            void Delete(const ::pb::ems::Order& order) ;
            void Query(const ::pb::ems::Order& order) ;

        private:
            void Init(const fh::core::assist::Settings &app_settings, const fh::core::assist::Settings &persist_settings);
            pb::ems::Fill Order_filled(const ::pb::ems::Order& org_order, const std::string &next_exchange_order_id);
            std::tuple<pb::ems::Order, int, int> Order_working(const ::pb::ems::Order& org_order, const std::string &next_exchange_order_id);
            void Order_reject(const ::pb::ems::Order& org_order, const std::string &reason);
            void Order_status(const ::pb::ems::Order& org_order, pb::ems::OrderStatus status);
            void Order_status(const ::pb::ems::Fill& org_fill);
            std::string Next_exchange_order_id();
            std::string Next_fill_id();
            bool Has_matching(pb::ems::Order& org_order) const;
            bool Has_rematching(std::tuple<pb::ems::Order, int, std::uint32_t>& order_info) const;
            void Rematching(const std::string &contract);
            void Set_to_current_time(::pb::ems::Timestamp* target);
            bool Is_position_reached(
                    std::tuple<pb::ems::Order, int, std::uint32_t>& order_info,
                    const ::google::protobuf::RepeatedPtrField<::pb::dms::DataPoint>& prices,
                    std::uint32_t current_turnover_size,
                    std::function<bool(double, double)> compare) const;
            static bool Is_price_matching(pb::ems::Order& org_order, const pb::dms::L2 &l2);
            int Calculate_order_position(const ::pb::ems::Order& org_order) const;

        private:
            core::exchange::ExchangeListenerI *m_result_listener;
            std::atomic<std::uint32_t> m_exchange_order_id;
            std::atomic<std::uint32_t> m_fill_id;
            // 当前未成交订单信息：订单，在同价位订单中的位置，同价位已成交手数
            std::map<std::string, std::tuple<pb::ems::Order, int, std::uint32_t>> m_working_orders;
            std::list<std::string> m_working_order_ids;   // 为了保证 working order 按照时间顺序匹配
            std::map<std::string, pb::ems::Fill> m_filled_orders;
            std::map<std::string, pb::ems::Order> m_canceled_orders;
            std::mutex m_mutex;
            // 每个合约的信息：L2 行情，最新 bid 成交数量，最新 ask 成交数量
            std::unordered_map<std::string , std::tuple<pb::dms::L2, std::uint32_t, std::uint32_t>> m_current_states;
            // 利用这个比率（[0,100]）和行情数据中实际成交数量来决定新来的订单是否成交
            uint32_t m_trade_rate;

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayOrderMatcher);
    };
} // namespace replay
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_REPLAY_REPLAY_ORDER_MATCHER_H__
