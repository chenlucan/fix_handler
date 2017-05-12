
#include "core/assist/logger.h"
#include "tmalpha/trade/trade_order_manager.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    TradeOrderManager::TradeOrderManager(OrderExpiredListener *expired_listener)
    : m_expired_listener(expired_listener), m_trade_orders()
    {
        // noop
    }

    TradeOrderManager::~TradeOrderManager()
    {
        for(auto &t : m_trade_orders) delete t.second;
    }

    // 添加一个合约，管理其订单
    void TradeOrderManager::Add_contract(const std::string &contract_name)
    {
        m_trade_orders[contract_name] = new TradeOrderBox(contract_name);
    }

    // 返回对手方未成交订单价位列表（合约必须存在）
    const std::map<ComparablePrice, std::list<pb::ems::Order*>>
        &TradeOrderManager::Opposite_working_orders(const std::string &contract_name, pb::ems::BuySell buy_or_sell) const
    {
        TradeOrderBox *box = this->Order_box(contract_name);
        return box->Opposite_working_orders(buy_or_sell);
    }

    // 添加一个新的未成交订单
    const pb::ems::Order *TradeOrderManager::Add_order(const pb::ems::Order &order)
    {
        LOG_INFO("add order: ", fh::core::assist::utility::Format_pb_message(order));

        TradeOrderBox *box = this->Order_box(order.contract());
        return box->Add_order(order);
    }

    // 添加一个新的删除订单（用于 FOK 或者 FAK 场合的订单删除）
    const pb::ems::Order *TradeOrderManager::Add_deleted_order(const pb::ems::Order &order)
    {
        LOG_INFO("add deleted order: ", fh::core::assist::utility::Format_pb_message(order));

        TradeOrderBox *box = this->Order_box(order.contract());
        return box->Add_deleted_order(order);
    }

    // 删除一个未成交订单，如果需要放入删除订单列表，则放进去
    pb::ems::Order TradeOrderManager::Delete_order(const pb::ems::Order &order, bool is_add_to_cancelled)
    {
        LOG_INFO("delete order: ", fh::core::assist::utility::Format_pb_message(order));

        TradeOrderBox *box = this->Order_box(order.contract());
        return box->Delete_order(order, is_add_to_cancelled);
    }

    // 查询指定订单状态，返回查询结果
    pb::ems::Order TradeOrderManager::Query_order(const pb::ems::Order &order) const
    {
        LOG_INFO("query order: ", fh::core::assist::utility::Format_pb_message(order));

        TradeOrderBox *box = this->Order_box(order.contract());
        pb::ems::Order result = box->Query_order(order);

        LOG_INFO("query canceled:", fh::core::assist::utility::Format_pb_message(result));
        return result;
    }

    // 一个新订单成交后需要将成交订单信息插入到成交列表
    pb::ems::Fill TradeOrderManager::Fill_order(const pb::ems::Order *order,
            const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price)
    {
        TradeOrderBox *box = this->Order_box(order->contract());
        return box->Fill_order(order, fill_id, filled_quantity, fill_price);
    }

    // 一个未成交订单成交后需要将成交订单信息插入到成交列表，同时修改未成交订单信息
    pb::ems::Fill TradeOrderManager::Fill_working_order(pb::ems::Order *working_order,
            const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price)
    {
        TradeOrderBox *box = this->Order_box(working_order->contract());
        return box->Fill_working_order(working_order, fill_id, filled_quantity, fill_price);
    }

    // 查询指定订单状态，返回： 1：已删除 2：未成交（或者部分成交） 3：全部已成交  4：找不到
    int TradeOrderManager::Order_status(const std::string &contract_name, const std::string &client_order_id) const
    {
        TradeOrderBox *box = this->Order_box(contract_name);
        return box->Order_status(client_order_id);
    }

    // 找到未成交订单，不存在的场合返回 null
    const pb::ems::Order *TradeOrderManager::Working_order(const std::string &contract_name, const std::string &client_order_id) const
    {
        TradeOrderBox *box = this->Order_box(contract_name);
        return box->Working_order(client_order_id);
    }

    // 获得指定合约的订单信息（合约不存在的场合抛出异常）
    TradeOrderBox *TradeOrderManager::Order_box(const std::string &contract_name) const
    {
        auto pos = m_trade_orders.find(contract_name);
        if(pos == m_trade_orders.end()) throw std::invalid_argument("contract " + contract_name + " not exist");
        return pos->second;
    }

    // 删除过期订单
    void TradeOrderManager::Delete_expired_order()
    {
        for(auto &c : m_trade_orders)
        {
            std::list<pb::ems::Order*> deleted = c.second->Delete_expired_order();
            for(pb::ems::Order* order : deleted)
            {
                m_expired_listener->On_order_expired(order);
            }
        }
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh

