
#ifndef __FH_TMALPHA_TRADE_TRADE_ORDER_MANAGER_H__
#define __FH_TMALPHA_TRADE_TRADE_ORDER_MANAGER_H__

#include <unordered_map>
#include "core/global.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/order_expired_listener.h"
#include "tmalpha/trade/trade_order_box.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 管理所有合约的订单
    class TradeOrderManager
    {
        public:
            explicit TradeOrderManager(OrderExpiredListener *expired_listener);
            virtual ~TradeOrderManager();

        public:
            // 添加一个合约，管理其订单
            void Add_contract(const std::string &contract_name);
            // 返回指定合约的对手方未成交订单价位列表
            const std::map<ComparablePrice, std::list<pb::ems::Order*>> &Opposite_working_orders(
                    const std::string &contract_name, pb::ems::BuySell buy_or_sell) const;
            // 添加一个新的未成交订单
            const pb::ems::Order *Add_order(const pb::ems::Order &order);
            // 添加一个新的删除订单（用于 FOK 或者 FAK 场合的订单删除）
            const pb::ems::Order *Add_deleted_order(const pb::ems::Order &order);
            // 删除一个未成交订单，如果需要放入删除订单列表，则放进去
            pb::ems::Order Delete_order(const pb::ems::Order &order, bool is_add_to_cancelled = true);
            // 查询指定订单状态，返回查询结果
            pb::ems::Order Query_order(const pb::ems::Order &order) const;
            // 一个新订单成交后需要将成交订单信息插入到成交列表
            pb::ems::Fill Fill_order(const pb::ems::Order *order, const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price);
            // 一个未成交订单成交后需要将成交订单信息插入到成交列表，同时修改未成交订单信息
            pb::ems::Fill Fill_working_order(pb::ems::Order *working_order, const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price);
            // 查询指定订单状态，返回： 1：已删除 2：未成交（或者部分成交） 3：全部已成交  4：找不到
            int Order_status(const std::string &contract_name, const std::string &client_order_id) const;
            // 找到未成交订单，不存在的场合返回 null
            const pb::ems::Order *Working_order(const std::string &contract_name, const std::string &client_order_id) const;

        private:
            // 获得指定合约的订单信息（合约不存在的场合抛出异常）
            TradeOrderBox *Order_box(const std::string &contract_name) const;
            // 删除过期订单
            void Delete_expired_order();

        private:
            // 订单过期监听
            OrderExpiredListener *m_expired_listener;
            // 合约对应的订单
            std::unordered_map<std::string, TradeOrderBox*> m_trade_orders;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeOrderManager);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_ORDER_MANAGER_H__
