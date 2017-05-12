
#ifndef __FH_TMALPHA_TRADE_TRADE_ORDER_BOX_H__
#define __FH_TMALPHA_TRADE_TRADE_ORDER_BOX_H__

#include <string>
#include <list>
#include <map>
#include "core/global.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/comparable_price.h"
#include "tmalpha/trade/order_expired_listener.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 每个合约的所有订单，按照价位分组，且最优价位靠前
    class TradeOrderBox
    {
        public:
            TradeOrderBox(const std::string &contract_name, OrderExpiredListener *expired_listener);
            virtual ~TradeOrderBox();

        public:
            // 返回对手方未成交订单价位列表
            const std::map<ComparablePrice, std::list<pb::ems::Order*>> &Opposite_working_orders(pb::ems::BuySell buy_or_sell) const;
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
            // 查看当前未成交订单中有没有过期的，有的话就将它删除
            void Delete_expired_order();
            // 查询指定订单状态，返回： 1：已删除 2：未成交（或者部分成交） 3：全部已成交  4：找不到
            int Order_status(const std::string &client_order_id) const;
            // 找到未成交订单，不存在的场合返回 null
            const pb::ems::Order *Working_order(const std::string &client_order_id) const;

        private:
            // 删除指定未成交订单；删除后如果该价位没有订单了，则删除该价位；该订单必须在未成交价位列表中存在
            // 返回被删除的订单
            pb::ems::Order *Remove_working_order(const std::string &client_order_id);
            // 添加一个未成交订单
            void Add_working_order(pb::ems::Order *order);
            // 添加新的成交订单信息到成交列表
            pb::ems::Fill Add_filled_order(const pb::ems::Order *org_order, const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price);
            // 一个未成交订单变成已成交时，修改其信息：
            // 全部成交的话，删除该未成交订单；否则修改其剩余未成交数量
            void Working_order_filled(pb::ems::Order *order, OrderSize fill_quantity);
            // 看看一个交易中订单是否过期了
            static bool Is_expired(const pb::ems::Order* order);

        private:
            // 合约名称
            std::string m_contract_name;
            // 订单过期监听
            OrderExpiredListener *m_expired_listener;
            // 每个价位的 bid 未成交订单列表（价位由高到低）
            std::map<ComparablePrice, std::list<pb::ems::Order*>> m_bid;
            // 每个价位的 ask 未成交订单列表（价位由低到高）
            std::map<ComparablePrice, std::list<pb::ems::Order*>> m_ask;
            // 所有的未成交订单（以 client order id 为 key）
            std::map<std::string, pb::ems::Order*> m_working_orders;
            // 所有的已删除订单（以 client order id 为 key）
            std::map<std::string, pb::ems::Order*> m_canceled_orders;
            // 所有的已成交订单（以 client order id 为 key）
            std::multimap<std::string, pb::ems::Fill> m_filled_orders;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeOrderBox);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_ORDER_BOX_H__
