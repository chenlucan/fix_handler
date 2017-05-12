
#include <ctime>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include "core/assist/logger.h"
#include "tmalpha/trade/trade_order_box.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    TradeOrderBox::TradeOrderBox(const std::string &contract_name, OrderExpiredListener *expired_listener)
    : m_contract_name(contract_name), m_expired_listener(expired_listener), m_bid(), m_ask(),
      m_working_orders(), m_canceled_orders(), m_filled_orders()
    {
        // noop
    }

    TradeOrderBox::~TradeOrderBox()
    {
        for(auto &w : m_working_orders) delete w.second;
        for(auto &c : m_canceled_orders) delete c.second;
    }

    // 返回对手方未成交订单价位列表
    const std::map<ComparablePrice, std::list<pb::ems::Order*>>
        &TradeOrderBox::Opposite_working_orders(pb::ems::BuySell buy_or_sell) const
    {
        if(buy_or_sell == pb::ems::BuySell::BS_Buy) return m_ask;
        else return m_bid;
    }

    // 添加一个新的未成交订单
    const pb::ems::Order *TradeOrderBox::Add_order(const pb::ems::Order &order)
    {
        LOG_INFO("add order: ", fh::core::assist::utility::Format_pb_message(order));

        // 在价位列表和未成交订单列表中添加新未成交订单
        pb::ems::Order *new_order = new pb::ems::Order(order);
        new_order->set_status(pb::ems::OrderStatus::OS_Working);
        this->Add_working_order(new_order);

        return new_order;
    }

    // 添加一个新的删除订单（用于 FOK 或者 FAK 场合的订单删除）
    const pb::ems::Order *TradeOrderBox::Add_deleted_order(const pb::ems::Order &order)
    {
        LOG_INFO("add deleted order: ", fh::core::assist::utility::Format_pb_message(order));

        // 将该订单加入到已删除列表
        pb::ems::Order *new_order = new pb::ems::Order(order);
        new_order->set_status(pb::ems::OrderStatus::OS_Cancelled);
        m_canceled_orders.insert({order.client_order_id(), new_order});

        return new_order;
    }

    // 删除一个未成交订单，如果需要放入删除订单列表，则放进去
    pb::ems::Order TradeOrderBox::Delete_order(const pb::ems::Order &order, bool is_add_to_cancelled)
    {
        LOG_INFO("delete order: ", fh::core::assist::utility::Format_pb_message(order));

        // 从价位列表和未成交订单列表中删除指定订单
        pb::ems::Order *target = this->Remove_working_order(order.client_order_id());
        target->set_status(pb::ems::OrderStatus::OS_Cancelled);

        // 有需要的话就将该订单挪到已删除列表
        pb::ems::Order result(*target);
        if(is_add_to_cancelled) m_canceled_orders.insert({order.client_order_id(), target});
        else  delete target;

        return result;
    }

    // 查询指定订单状态，返回查询结果
    pb::ems::Order TradeOrderBox::Query_order(const pb::ems::Order &order) const
    {
        LOG_INFO("query order: ", fh::core::assist::utility::Format_pb_message(order));

        // 看看是否已删除
        auto pos = m_canceled_orders.find(order.client_order_id());
        if(pos != m_canceled_orders.end())
        {
            pb::ems::Order result(*pos->second);
            result.set_status(pb::ems::OrderStatus::OS_Cancelled);

            LOG_INFO("order canceled:", fh::core::assist::utility::Format_pb_message(result));
            return result;
        }

        // 看看是否有未成交
        auto wpos = m_working_orders.find(order.client_order_id());
        if(wpos != m_working_orders.end())
        {
            pb::ems::Order result(*wpos->second);
            result.set_working_price(result.price());
            result.set_working_quantity(result.quantity());
            result.set_status(pb::ems::OrderStatus::OS_Working);

            LOG_INFO("order working:", fh::core::assist::utility::Format_pb_message(result));
            return result;
        }

        // 看看是否已全部成交
        OrderSize filled = 0;
        auto range = m_filled_orders.equal_range(order.client_order_id());
        for(auto p = range.first; p != range.second; ++p) filled += p->second.fill_quantity();
        if(filled != 0)
        {
            pb::ems::Fill fill = range.first->second;
            pb::ems::Order result;
            result.set_client_order_id(fill.client_order_id());
            result.set_account(fill.account());
            result.set_contract(fill.contract());
            result.set_buy_sell(fill.buy_sell());
            result.set_exchange_order_id(fill.exchange_order_id());
            result.set_filled_quantity(filled);
            result.set_status(pb::ems::OrderStatus::OS_Filled);

            LOG_INFO("order filled:", fh::core::assist::utility::Format_pb_message(result));
            return result;
        }

        // 找不到该订单情报
        pb::ems::Order result(order);
        result.set_status(pb::ems::OrderStatus::OS_Rejected);
        result.set_message("order not found");

        LOG_INFO("order rejected:", fh::core::assist::utility::Format_pb_message(result));
        return result;
    }

    // 一个新订单成交后需要将成交订单信息插入到成交列表
    pb::ems::Fill TradeOrderBox::Fill_order(const pb::ems::Order *order,
            const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price)
    {
        // 插入到成交列表
        return this->Add_filled_order(order, fill_id, filled_quantity, fill_price);
    }

    // 一个未成交订单成交后需要将成交订单信息插入到成交列表，同时修改未成交订单信息
    pb::ems::Fill TradeOrderBox::Fill_working_order(pb::ems::Order *working_order,
            const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price)
    {
        // 插入到成交列表
        pb::ems::Fill fill = this->Add_filled_order(working_order, fill_id, filled_quantity, fill_price);

        // 修改原始未成交订单数量（减去已成交数量）
        this->Working_order_filled(working_order, filled_quantity);

        return fill;
    }

    // 查看当前未成交订单中有没有过期的，有的话就将它删除
    void TradeOrderBox::Delete_expired_order()
    {
        // 找到过期订单
        std::vector<pb::ems::Order*> expired;
        boost::push_back(expired, m_working_orders | boost::adaptors::map_values | boost::adaptors::filtered(&TradeOrderBox::Is_expired));

        // 逐一删除
        for(const pb::ems::Order* e : expired)
        {
            // 从价位列表和未成交订单列表中删除指定订单
            pb::ems::Order *target = this->Remove_working_order(e->client_order_id());
            target->set_status(pb::ems::OrderStatus::OS_Cancelled);
            // 将该订单挪到已删除列表
            m_canceled_orders.insert({e->client_order_id(), target});
            // 通知监听器
            if(m_expired_listener) m_expired_listener->On_order_expired(e);

            LOG_INFO("order is expired: ", fh::core::assist::utility::Format_pb_message(*target));
        }
    }

    // 查询指定订单状态，返回： 1：已删除 2：未成交（或者部分成交） 3：全部已成交  4：找不到
    int TradeOrderBox::Order_status(const std::string &client_order_id) const
    {
        // 是否已删除
        auto cpos = m_canceled_orders.find(client_order_id);
        if(cpos != m_canceled_orders.end()) return 1;

        // 是否有未成交
        auto wpos = m_working_orders.find(client_order_id);
        if(wpos != m_working_orders.end()) return 2;

        // 是否有已成交
        auto fpos = m_filled_orders.find(client_order_id);
        if(fpos != m_filled_orders.end()) return 3;

        // 找不到
        return 4;
    }

    // 找到未成交订单，不存在的场合返回 null
    const pb::ems::Order *TradeOrderBox::Working_order(const std::string &client_order_id) const
    {
        auto wpos = m_working_orders.find(client_order_id);
        if(wpos == m_working_orders.end()) return nullptr;
        return wpos->second;
    }

    // 删除指定未成交订单；删除后如果该价位没有订单了，则删除该价位；该订单必须在未成交价位列表中存在
    // 返回被删除的订单
    pb::ems::Order *TradeOrderBox::Remove_working_order(const std::string &client_order_id)
    {
        // 从价位列表中删除该订单
        pb::ems::Order *order = m_working_orders[client_order_id];
        ComparablePrice cp(*order);
        std::map<ComparablePrice, std::list<pb::ems::Order*>> &current =
                order->buy_sell() == pb::ems::BuySell::BS_Buy ? m_bid : m_ask;
        current[cp].remove(order);
        if(current[cp].empty()) current.erase(cp);

        // 从未成交订单列表中删除该订单
        m_working_orders.erase(client_order_id);

        return order;
    }

    // 添加一个未成交订单
    void TradeOrderBox::Add_working_order(pb::ems::Order *order)
    {
        // 加入到未成交价位列表中
        ComparablePrice cp(*order);
        std::map<ComparablePrice, std::list<pb::ems::Order*>> &current =
                order->buy_sell() == pb::ems::BuySell::BS_Buy ? m_bid : m_ask;
        // 如果该价位不存在，创建该价位；然后在该价位上插入该订单
        current[cp].push_back(order);

        // 记录到未成交订单列表
        m_working_orders.insert({order->client_order_id(), order});
    }

    // 添加新的成交订单信息到成交列表
    pb::ems::Fill TradeOrderBox::Add_filled_order(const pb::ems::Order *org_order,
            const std::string &fill_id, OrderSize filled_quantity, OrderPrice fill_price)
    {
        // 创建成交订单信息
        pb::ems::Fill fill;
        fill.set_fill_id(fill_id);
        fill.set_fill_price(std::to_string(TO_REAL_PRICE(fill_price)));
        fill.set_fill_quantity(filled_quantity);
        fill.set_account(org_order->account());
        fill.set_client_order_id(org_order->client_order_id());
        fill.set_exchange_order_id(org_order->exchange_order_id());
        fill.set_contract(org_order->contract());
        fill.set_buy_sell(org_order->buy_sell());
        fh::core::assist::utility::To_pb_time(fill.mutable_fill_time(), fh::core::assist::utility::Current_time_str("yyyyMMdd- HH:mm:ss.sss"));

        // 登记到已成交订单列表
        m_filled_orders.insert({fill.client_order_id(), fill});

        LOG_INFO("add filled order:", fh::core::assist::utility::Format_pb_message(fill));
        return fill;
    }

    // 一个未成交订单变成已成交时，修改其信息：
    // 全部成交的话，删除该未成交订单；否则修改其剩余未成交数量
    void TradeOrderBox::Working_order_filled(pb::ems::Order *order, OrderSize fill_quantity)
    {
        OrderSize working_quantity = order->quantity();
        OrderSize current_filled_quantity = order->filled_quantity();
        if(working_quantity == fill_quantity)
        {
            LOG_INFO("order full filled: ", fill_quantity);

            // 全部成交了，要从价位中删除
            pb::ems::Order *target = this->Remove_working_order(order->client_order_id());
            // 释放该未成交订单的内存
            delete target;
        }
        else
        {
            LOG_INFO("order partial filled: ", working_quantity, " - ", fill_quantity);

            // 部分成交，修改未成交数量和已成交数量
            order->set_quantity(working_quantity - fill_quantity);
            order->set_filled_quantity(current_filled_quantity + fill_quantity);
        }
    }

    // 看看一个交易中订单是否过期了
    bool TradeOrderBox::Is_expired(const pb::ems::Order* order)
    {
        // 不是 GFD 订单（GDF：当日有效）的话，不会自动过期
        if(order->tif() != pb::ems::TimeInForce::TIF_GFD) return false;

        // 看看订单日期是否过去了
        const pb::ems::Date &order_date = order->submit_time().date();
        time_t  now;
        std::tm utc = *std::gmtime(&now);

        if(utc.tm_year + 1900 > (int)order_date.year()) return true;
        if(utc.tm_year + 1900 < (int)order_date.year()) return false;

        if(utc.tm_mon + 1 > (int)order_date.month()) return true;
        if(utc.tm_mon + 1 < (int)order_date.month()) return false;

        return utc.tm_mday > (int)order_date.day();
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh

