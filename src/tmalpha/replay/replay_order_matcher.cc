
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "tmalpha/replay/replay_order_matcher.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{

    ReplayOrderMatcher::ReplayOrderMatcher(int trade_rate)
    : m_result_listener(nullptr), m_exchange_order_id(0), m_fill_id(0),
      m_working_orders(), m_working_order_ids(), m_filled_orders(),  m_canceled_orders(),
      m_mutex(), m_current_states(), m_trade_rate(std::max(std::min(trade_rate, 100), 0))
    {
        // noop
    }

    ReplayOrderMatcher::~ReplayOrderMatcher()
    {
        // noop
    }

    void ReplayOrderMatcher::Add_exchange_listener(fh::core::exchange::ExchangeListenerI *result_listener)
    {
        m_result_listener =  result_listener;
    }

    // L2 情报发生变化时，内部需要记录，同时对尚未成交的订单重新匹配下
    // 接受到的参数为：L2行情数据，最新 bid 成交量，最新 ask 成交量
    void ReplayOrderMatcher::On_state_changed(const pb::dms::L2 &l2, std::uint32_t bid_volumn, std::uint32_t ask_volumn)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_current_states[l2.contract()] = std::make_tuple(l2, bid_volumn, ask_volumn);
        this->Rematching(l2.contract());
    }

    void ReplayOrderMatcher::Add(const ::pb::ems::Order& order)
    {
        LOG_INFO("new order:", fh::core::assist::utility::Format_pb_message(order));

        std::lock_guard<std::mutex> lock(m_mutex);

        pb::ems::Order org_order(order);
        if(this->Has_matching(org_order))
        {
            m_filled_orders[org_order.client_order_id()] = this->Order_filled(org_order, this->Next_exchange_order_id());
        }
        else
        {
            m_working_orders[org_order.client_order_id()] = this->Order_working(org_order, this->Next_exchange_order_id());
            m_working_order_ids.push_back(org_order.client_order_id());
        }
    }

    void ReplayOrderMatcher::Change(const ::pb::ems::Order& order)
    {
        LOG_INFO("update order:", fh::core::assist::utility::Format_pb_message(order));

        std::lock_guard<std::mutex> lock(m_mutex);
        auto fpos = m_filled_orders.find(order.client_order_id());
        if(fpos != m_filled_orders.end())
        {
            // 该订单已经成交过了，不能修改
            this->Order_reject(order, "order is filled");
            return;
        }

        auto cpos = m_canceled_orders.find(order.client_order_id());
        if(cpos != m_canceled_orders.end())
        {
            // 该订单已经删除过了，不能修改
            this->Order_reject(order, "order is canceled");
            return;
        }

        auto opos = m_working_orders.find(order.client_order_id());
        if(opos == m_working_orders.end())
        {
            // 找不到该订单
            this->Order_reject(order, "order not found");
            return;
        }

        // 将指定订单修改成新订单的值 TODO 此时还需要重新匹配下？
        m_working_orders[order.client_order_id()] = this->Order_working(order, std::get<0>(opos->second).exchange_order_id());
    }

    void ReplayOrderMatcher::Delete(const ::pb::ems::Order& order)
    {
        LOG_INFO("delete order:", fh::core::assist::utility::Format_pb_message(order));

        std::lock_guard<std::mutex> lock(m_mutex);
        auto fpos = m_filled_orders.find(order.client_order_id());
        if(fpos != m_filled_orders.end())
        {
            // 该订单已经成交过了，不能删除
            this->Order_reject(order, "order is filled");
            return;
        }

        auto cpos = m_canceled_orders.find(order.client_order_id());
        if(cpos != m_canceled_orders.end())
        {
            // 该订单已经删除过了，不能删除
            this->Order_reject(order, "order is canceled");
            return;
        }

        auto opos = m_working_orders.find(order.client_order_id());
        if(opos == m_working_orders.end())
        {
            // 找不到该订单
            this->Order_reject(order, "order not found");
            return;
        }

        // 发布删除结果
        this->Order_status(std::get<0>(opos->second), pb::ems::OrderStatus::OS_Cancelled);

        // 将找到的订单挪到已删除订单队列
        m_canceled_orders[opos->first] = std::get<0>(opos->second);
        m_working_order_ids.remove(opos->first);
        m_working_orders.erase(opos);
    }

    void ReplayOrderMatcher::Query(const ::pb::ems::Order& order)
    {
        LOG_INFO("query order:", fh::core::assist::utility::Format_pb_message(order));

        std::lock_guard<std::mutex> lock(m_mutex);
        auto fpos = m_filled_orders.find(order.client_order_id());
        if(fpos != m_filled_orders.end())
        {
            // 该订单已经成交过了
            this->Order_status(fpos->second);
            return;
        }

        auto cpos = m_canceled_orders.find(order.client_order_id());
        if(cpos != m_canceled_orders.end())
        {
            // 该订单已经删除过了
            this->Order_status(cpos->second, pb::ems::OrderStatus::OS_Cancelled);
            return;
        }

        auto opos = m_working_orders.find(order.client_order_id());
        if(opos == m_working_orders.end())
        {
            // 找不到该订单
            this->Order_reject(order, "order not found");
            return;
        }

        // 该订单在处理中
        this->Order_status(std::get<0>(opos->second), pb::ems::OrderStatus::OS_Working);
    }

    // 新订单来的时候看看和当前行情能否成交
    bool ReplayOrderMatcher::Has_matching(::pb::ems::Order& org_order) const
    {
        // 订单中的合约在行情状态数据中不存在的话，肯定不能匹配
        auto pos = m_current_states.find(org_order.contract());
        if(pos == m_current_states.end()) return false;

        // L2 行情数据
        const pb::dms::L2 &l2 = std::get<0>(pos->second);
        return ReplayOrderMatcher::Is_price_matching(org_order, l2);
    }

    // 行情变化后看看能够和当前待匹配订单能够成交
    // 不能成交的话，根据需要更新订单信息的同价位已成交数量
    bool ReplayOrderMatcher::Has_rematching(std::tuple<pb::ems::Order, int, std::uint32_t>& order_info) const
    {
        pb::ems::Order &org_order = std::get<0>(order_info);

        // 订单中的合约在行情状态数据中不存在的话，肯定不能匹配
        auto pos = m_current_states.find(org_order.contract());
        if(pos == m_current_states.end()) return false;

        const pb::dms::L2 &l2 = std::get<0>(pos->second); // L2 行情数据
        std::uint32_t bid_volumn = std::get<1>(pos->second);    // 最近成交的 bid 数量
        std::uint32_t ask_volumn = std::get<2>(pos->second);    // 最近成交的 ask 数量

        // 如果可以马上匹配到对手方报价，成交
        if(ReplayOrderMatcher::Is_price_matching(org_order, l2)) return true;

        if(org_order.buy_sell() == pb::ems::BuySell::BS_Buy)
        {
            // 当前行情中最优报价中已成交量满足预设定比率的话，也可以成交
            return this->Is_position_reached(order_info, l2.bid(), bid_volumn, std::greater<double>());
        }
        else if(org_order.buy_sell() == pb::ems::BuySell::BS_Sell)
        {
            // 当前行情中最优报价中已成交量满足预设定比率的话，也可以成交
            return this->Is_position_reached(order_info, l2.offer(), ask_volumn, std::less<double>());
        }

        return false;
    }

    // 根据当前时间点行情数据中最优价上的已成交数量，以及待匹配订单的价格，同价位上的位置，以及同价位上的已成交数量，
    // 来决定该订单是否可以成交
    // order_info: 订单，同价位上的位置，以及同价位上的已成交数量
    // prices: 同侧行情数据（新订单是买，那么这就是当前买单的行情数据）
    // current_turnover_size: 在最优价位上的最近已成交手数
    // compare: 决定一个价格是否优于另外一个价格
    bool ReplayOrderMatcher::Is_position_reached(
            std::tuple<pb::ems::Order, int, std::uint32_t>& order_info,
            const ::google::protobuf::RepeatedPtrField<::pb::dms::DataPoint>& prices,
            std::uint32_t current_turnover_size,
            std::function<bool(double, double)> compare) const
    {
        // 同侧的报价不存在，认为没达到预设定位置
        if(prices.empty()) return false;

        pb::ems::Order &order = std::get<0>(order_info);
        int &position = std::get<1>(order_info);
        std::uint32_t &turnover = std::get<2>(order_info);
        double order_price = std::stod(order.price());
        double best_price = prices.Get(0).price();
        std::uint64_t best_size = prices.Get(0).size();

        LOG_DEBUG("trade position: current_turnover_size=", current_turnover_size, ", current_best_price=", best_price,
                                ", order.price=", order_price, ", order.position=", position, ", order.total_turnover=", turnover);

        // 行情的最优报价优于（买的场合大于，卖的场合小于）订单的报价的话，已成交数量维持不变，订单不能成交
        if(compare(best_price, order_price))
        {
            LOG_DEBUG("current_best_price is better than order_price");
            return false;
        }
        // 订单的报价优于当前行情的最优报价，该订单马上可以成交
        if(compare(order_price, best_price))
        {
            LOG_DEBUG("order_price is better than current_best_price");
            return true;
        }

        // 价格相同的场合
        // 订单的成交位置还没算出来，说明这里是首次遇到同价位行情，这里需要根据总共的未成交数量和预设定比率计算下位置
        if(position == -1)
        {
            position = best_size * m_trade_rate / 100;
            // 此时不能成交
            LOG_DEBUG("calculated position: ", position);
            return false;
        }

        // 累计下订单的已成交数量
        turnover += current_turnover_size;

        // 只有累计已成交数量 > 订单在同价位上的位置的场合才可以成交
        return turnover > (std::uint32_t)position;
    }

    // 订单价格和当前行情中的对手方价格能否成交
    bool ReplayOrderMatcher::Is_price_matching(pb::ems::Order& org_order, const pb::dms::L2 &l2)
    {
        if(org_order.buy_sell() == pb::ems::BuySell::BS_Buy)
        {
            // 没有卖方报价，不能成交
            if(l2.offer().empty()) return false;

            // 成交条件：订单是 market order 的话（没指定价格），或者，报单价格匹配到当前对手方报价
            bool is_matching =
                    (org_order.order_type() == pb::ems::OrderType::OT_Market) || std::stod(org_order.price()) >= l2.offer(0).price();

            // 设置成交价
            if(is_matching) org_order.set_price(std::to_string(l2.offer(0).price()));
            LOG_DEBUG("buy order match result: ", is_matching);

            return is_matching;
        }
        else if(org_order.buy_sell() == pb::ems::BuySell::BS_Sell)
        {
            // 没有买方报价，不能成交
            if(l2.bid().empty()) return false;

            // 成交条件：订单是 market order 的话（没指定价格），或者，报单价格匹配到当前对手方报价
            bool is_matching =
                    (org_order.order_type() == pb::ems::OrderType::OT_Market) || std::stod(org_order.price()) <= l2.bid(0).price();

            // 设置成交价
            if(is_matching) org_order.set_price(std::to_string(l2.bid(0).price()));
            LOG_DEBUG("sell order match result: ", is_matching);

            return is_matching;
        }

        return false;
    }

    // 根据预设定比率，计算一个订单的在同等价位中的位置
    int ReplayOrderMatcher::Calculate_order_position(const ::pb::ems::Order& org_order) const
    {
        // 订单中的合约在行情状态数据中不存在的话，不能计算
        auto pos = m_current_states.find(org_order.contract());
        if(pos == m_current_states.end()) return -1;

        const pb::dms::L2 &l2 = std::get<0>(pos->second); // L2 行情数据
        if(org_order.buy_sell() == pb::ems::BuySell::BS_Buy)
        {
            // 同侧的报价不存在，不能计算
            if(l2.bid().empty()) return -1;
            // 不是同等价位，不能计算
            if(std::stod(org_order.price()) != l2.bid(0).price()) return -1;
            // 按照预设定比率计算出当前订单的位置
            return l2.bid(0).size() * m_trade_rate / 100;
        }
        else if(org_order.buy_sell() == pb::ems::BuySell::BS_Sell)
        {
            // 同侧的报价不存在，不能计算
            if(l2.offer().empty()) return -1;
            // 不是同等价位，不能计算
            if(std::stod(org_order.price()) != l2.offer(0).price()) return -1;
            // 按照预设定比率计算出当前订单的位置
            return l2.offer(0).size() * m_trade_rate / 100;
        }

        return -1;
    }

    void ReplayOrderMatcher::Rematching(const std::string &contract)
    {
        LOG_INFO("rematching order...");

        // 行情变化后看看待处理订单中有没有能匹配价格的
        for(auto iter = m_working_order_ids.begin(); iter != m_working_order_ids.end();)
        {
            auto &order_info = m_working_orders[*iter];
            auto &order = std::get<0>(order_info);
            if(order.contract() == contract && this->Has_rematching(order_info))
            {
                m_filled_orders[order.client_order_id()] = this->Order_filled(order, order.exchange_order_id());
                m_working_order_ids.erase(iter++);
                m_working_orders.erase(order.client_order_id());
            }
            else
            {
                iter++;
            }
        }

        LOG_INFO("rematching end.");
    }

    // 新订单成交时，将成交信息发送出去
    pb::ems::Fill ReplayOrderMatcher::Order_filled(const ::pb::ems::Order& org_order, const std::string &next_exchange_order_id)
    {
        pb::ems::Fill fill;
        fill.set_fill_id(this->Next_fill_id());
        fill.set_fill_price(org_order.price());
        fill.set_fill_quantity(org_order.quantity());
        fill.set_account(org_order.account());
        fill.set_client_order_id(org_order.client_order_id());
        fill.set_exchange_order_id(next_exchange_order_id);
        fill.set_contract(org_order.contract());
        fill.set_buy_sell(org_order.buy_sell());
        this->Set_to_current_time(fill.mutable_fill_time());

        LOG_INFO("fill result:", fh::core::assist::utility::Format_pb_message(fill));
        m_result_listener->OnFill(fill);

        return fill;
    }

    // 新订单未成交时，将状态信息发送出去，并返回需要保存在内存中的订单详情（订单，同价位上的位置，同价位上已成交数量）
    std::tuple<pb::ems::Order, int, int> ReplayOrderMatcher::Order_working(const ::pb::ems::Order& org_order, const std::string &next_exchange_order_id)
    {
        pb::ems::Order order(org_order);
        order.set_exchange_order_id(next_exchange_order_id);
        order.set_status(pb::ems::OrderStatus::OS_Working);
        order.set_working_price(org_order.price());
        order.set_working_quantity(org_order.quantity());
        order.set_filled_quantity(0);
        order.set_message("");
        this->Set_to_current_time(order.mutable_submit_time());

        LOG_INFO("order result:", fh::core::assist::utility::Format_pb_message(order));
        m_result_listener->OnOrder(order);

        // 计算订单在同等报价中的位置
        int position = this->Calculate_order_position(org_order);
        LOG_DEBUG("order's position: price=", org_order.price(), ", position=", position);

        return std::make_tuple(order, position, 0);
    }

    // 订单操作被拒绝时，将拒绝原因发送出去
    void ReplayOrderMatcher::Order_reject(const ::pb::ems::Order& org_order, const std::string &reason)
    {
        pb::ems::Order order(org_order);
        order.set_status(pb::ems::OrderStatus::OS_Rejected);
        order.set_message(reason);
        this->Set_to_current_time(order.mutable_submit_time());

        LOG_INFO("order reject:", fh::core::assist::utility::Format_pb_message(order));
        m_result_listener->OnOrder(order);
    }

    // 将一个对处理中订单状态查询的结果发送出去
    void ReplayOrderMatcher::Order_status(const ::pb::ems::Order& org_order, pb::ems::OrderStatus status)
    {
        pb::ems::Order order(org_order);
        order.set_status(status);
        order.set_message("");
        this->Set_to_current_time(order.mutable_submit_time());

        LOG_INFO("order status:", fh::core::assist::utility::Format_pb_message(order));
        m_result_listener->OnOrder(order);
    }

    // 将一个对已成交订单状态查询的结果发送出去
    void ReplayOrderMatcher::Order_status(const ::pb::ems::Fill& org_fill)
    {
        pb::ems::Order order;
        order.set_client_order_id(org_fill.client_order_id());
        order.set_account(org_fill.account());
        order.set_contract(org_fill.contract());
        order.set_buy_sell(org_fill.buy_sell());
        order.set_price(org_fill.fill_price());
        order.set_quantity(org_fill.fill_quantity());
        //order.set_tif(org_fill.tif());
        //order.set_order_type(org_order.order_type());
        order.set_exchange_order_id(org_fill.exchange_order_id());
        order.set_status(pb::ems::OrderStatus::OS_Filled);
        //order.set_working_price();
        //order.set_working_quantity(0);
        order.set_filled_quantity(org_fill.fill_quantity());
        order.set_message("");
        this->Set_to_current_time(order.mutable_submit_time());

        LOG_INFO("order status:", fh::core::assist::utility::Format_pb_message(order));
        m_result_listener->OnOrder(order);
    }

    std::string ReplayOrderMatcher::Next_exchange_order_id()
    {
        return "Order-" + std::to_string(++m_exchange_order_id);
    }

    std::string ReplayOrderMatcher::Next_fill_id()
    {
        return "Fill-" + std::to_string(++m_fill_id);
    }

    // 将当前时间按照 yyyyMMdd-HH:mi:ss.fff 格式设置到目标对象中
    void ReplayOrderMatcher::Set_to_current_time(::pb::ems::Timestamp* target)
    {
        // yyyy-MM-dd HH:mm:ss.ssssss
        std::string now = fh::core::assist::utility::Current_time_str();
        fh::core::assist::utility::To_pb_time(target, now.substr(0, 4) + now.substr(5, 2) + now.substr(8, 2) + "-" + now.substr(11, 12));
    }

} // namespace replay
} // namespace tmalpha
} // namespace fh
