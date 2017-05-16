
#include <boost/algorithm/string.hpp>
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "tmalpha/trade/trade_simulater.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{

    TradeSimulater::TradeSimulater(
            fh::core::market::MarketListenerI *market_listener, fh::core::exchange::ExchangeListenerI *exchange_listener)
    : fh::core::exchange::ExchangeI(exchange_listener),
      m_exchange_listener(exchange_listener), m_init_orders(), m_match_algorithm(nullptr),
      m_contract_assist(), m_market_manager(market_listener), m_order_manager(this),
      m_exchange_order_id(0), m_fill_id(0)
    {
        // noop
    }

    TradeSimulater::~TradeSimulater()
    {
        // noop
    }

    // 加载初期合约定义信息
    void TradeSimulater::Load_contracts(const std::unordered_map<std::string, std::string> &contracts)
    {
        // contract name=depth,tick price,min price,max price(price exponent is -2)
        for(const auto &c : contracts)
        {
            const std::string &name = c.first;
            std::vector<std::string> cs;
            boost::split(cs, c.second, boost::is_any_of(","));
            TradeContract tc{name, (std::uint32_t)std::stoul(cs[0]), (OrderPrice)std::stoul(cs[1]), (OrderPrice)std::stoul(cs[2]), (OrderPrice)std::stoul(cs[3])};

            // 初期化合约管理模块
            m_contract_assist.Add(tc);
            // 初期化行情管理模块
            m_market_manager.Add_contract(tc.name, tc.depth);
            // 初期化订单管理模块
            m_order_manager.Add_contract(tc.name);
        }
    }

    // 加载订单匹配算法
    void TradeSimulater::Load_match_algorithm(TradeAlgorithm *ta)
    {
        m_match_algorithm = ta;
    }

    // implement of ExchangeI
    bool TradeSimulater::Start(const std::vector<pb::ems::Order> &init_orders)
    {
        m_init_orders = init_orders;
        LOG_INFO("init orders count:", init_orders.size());

        // 将合约信息对外通知
        for(const auto &c : m_contract_assist.Contracts())
        {
            m_market_manager.Send_contract(c.second.To_dms());
        }

        LOG_INFO("trade simulater started.");
        return true;
    }

    // implement of ExchangeI
    void TradeSimulater::Stop()
    {
        LOG_INFO("trade simulater stopped.");
    }

    // implement of ExchangeI
    void TradeSimulater::Initialize(std::vector<::pb::dms::Contract> contracts)
    {
        // noop
    }

    // implement of ExchangeI
    void TradeSimulater::Add(const pb::ems::Order& order)
    {
        LOG_INFO("add order: ", fh::core::assist::utility::Format_pb_message(order));

        // 对订单的合约，数量，价格进行检查
        std::string err = this->Check_order(order);
        if(err != "")
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, err));
            return;
        }

        // 新订单的 client order id 不能已存在
        // 1：已删除；2：未成交（或者部分成交）；3：全部已成交；4：找不到
        int status = m_order_manager.Order_status(order.contract(), order.client_order_id());
        if(status != 4)
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, Message::CLIENT_ORDER_ID_EXIST));
            return;
        }

        // 接受该订单
        pb::ems::Order pending(order);
        pending.set_exchange_order_id(this->Next_exchange_order_id());
        pending.set_status(pb::ems::OrderStatus::OS_Pending);
        m_exchange_listener->OnOrder(pending);

        // 看看能否成交
        this->Match_order(pending);
    }

    // implement of ExchangeI
    void TradeSimulater::Change(const pb::ems::Order& order)
    {
        LOG_INFO("change order: ", fh::core::assist::utility::Format_pb_message(order));

        // 对变更后订单的合约，数量，价格进行检查
        std::string err = this->Check_order(order);
        if(err != "")
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, err));
            return;
        }

        // 必须存在指定的未成交订单
        // 1：已删除；2：未成交（或者部分成交）；3：全部已成交；4：找不到
        int status = m_order_manager.Order_status(order.contract(), order.client_order_id());
        if(status != 2)
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, Message::WORKING_ORDER_NOT_FOUND));
            return;
        }

        // 接受该订单修改请求
        pb::ems::Order pending(order);
        pending.set_status(pb::ems::OrderStatus::OS_Pending);
        m_exchange_listener->OnOrder(pending);

        // 删除旧订单（不需要放入删除订单列表），并将结果发送出去
        pb::ems::Order deleted_order = m_order_manager.Delete_order(order, false);
        m_exchange_listener->OnOrder(TradeSimulater::Make_cancel_response(deleted_order, Message::ORDER_CANCELLED));
        // 修改并发送行情数据
        m_market_manager.Change_market_on_order_deleted(&deleted_order);

        // 看看能否成交
        this->Match_order(pending);
    }

    // implement of ExchangeI
    void TradeSimulater::Delete(const pb::ems::Order& order)
    {
        LOG_INFO("delete order: ", fh::core::assist::utility::Format_pb_message(order));

        // 订单对应的合约不存在
        if(!m_contract_assist.Is_contract_exist(order.contract()))
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, Message::CONTRACT_NOT_FOUND));
            return;
        }

        // 1：已删除；2：未成交（或者部分成交）；3：全部已成交；4：找不到
        int status = m_order_manager.Order_status(order.contract(), order.client_order_id());
        if(status != 2)
        {
            static std::string reject_reasons[]{
                    "", Message::ORDER_ALREADY_CANCELLED, "", Message::ORDER_ALREADY_FILLED, Message::ORDER_NOT_FOUND};
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, reject_reasons[status]));
            return;
        }

        // 删除指定订单，并将结果发送出去
        pb::ems::Order deleted_order = m_order_manager.Delete_order(order);
        m_exchange_listener->OnOrder(TradeSimulater::Make_cancel_response(deleted_order, Message::ORDER_CANCELLED));

        // 修改并发送行情数据
        m_market_manager.Change_market_on_order_deleted(&deleted_order);
    }

    // implement of ExchangeI
    void TradeSimulater::Query(const pb::ems::Order& order)
    {
        LOG_INFO("query order: ", fh::core::assist::utility::Format_pb_message(order));

        // 订单对应的合约不存在
        if(!m_contract_assist.Is_contract_exist(order.contract()))
        {
            m_exchange_listener->OnOrder(TradeSimulater::Make_reject_response(order, Message::CONTRACT_NOT_FOUND));
            return;
        }

        // 查询指定订单的状态
        pb::ems::Order result = m_order_manager.Query_order(order);
        // Reject 说明指定订单没找到
        if(result.status() == pb::ems::OrderStatus::OS_Rejected)
        {
            result.set_message(Message::ORDER_NOT_FOUND);
        }

        m_exchange_listener->OnOrder(result);
    }

    // implement of ExchangeI
    void TradeSimulater::Query_mass(const char *data, size_t size)
    {
        // noop
    }

    // implement of ExchangeI
    void TradeSimulater::Delete_mass(const char *data, size_t size)
    {
        // noop
    }

    // implement of OrderExpiredListener
    void TradeSimulater::On_order_expired(const pb::ems::Order *order)
    {
        // 通知订单监听者
        m_exchange_listener->OnOrder(TradeSimulater::Make_cancel_response(*order, Message::ORDER_EXPIRED));
        // 修改并发送行情数据
        m_market_manager.Change_market_on_order_deleted(order);
    }

    std::string TradeSimulater::Next_exchange_order_id()
    {
        return "Order-" + std::to_string(++m_exchange_order_id);
    }

    std::string TradeSimulater::Next_fill_id()
    {
        return "Fill-" + std::to_string(++m_fill_id);
    }

    // 检查一个订单的合约，数量，价格是否合法
    // 不合法的场合返回错误信息，否则返回 ""
    std::string TradeSimulater::Check_order(const pb::ems::Order& order) const
    {
        // 订单的报价是否符合对应的合约的要求
        TradeOrderPreCheckStatus ocs = m_contract_assist.Check_order(order);
        if(ocs != TradeOrderPreCheckStatus::NORMAL)
        {
            static std::string reject_reasons[]{
                    "", Message::CONTRACT_NOT_FOUND, Message::ORDER_PRICE_TOO_LOW, Message::ORDER_PRICE_TOO_HIGH, Message::ORDER_PRICE_INVALID};
            return reject_reasons[(int)ocs];
        }

        // 新订单类型要是 Limit 或者 Market
        if(order.order_type() == pb::ems::OrderType::OT_None) return Message::ORDER_TYPE_INVALID;
        // 新订单处理只能是买或者卖
        if(order.buy_sell() == pb::ems::BuySell::BS_None) return Message::ORDER_BS_INVALID;
        // limit 订单价格要大于 0
        if(order.order_type() == pb::ems::OrderType::OT_Limit && std::stod(order.price()) == 0) return Message::LIMIT_ORDER_PRICE_INVALID;
        // market 订单价格要为 0
        if(order.order_type() == pb::ems::OrderType::OT_Market && std::stod(order.price()) != 0) return Message::MARKET_ORDER_PRICE_INVALID;
        // 订单数量要大于 0
        if(order.quantity() == 0) return Message::ORDER_QUANTITY_INVALID;
        
        return "";
    }

    void TradeSimulater::Match_order(pb::ems::Order &order)
    {
        // 看看能否成交
        const std::map<ComparablePrice, std::list<pb::ems::Order*>> &opposite_working_orders
                        = m_order_manager.Opposite_working_orders(order.contract(), order.buy_sell());
        std::vector<MatchedOrder> matched_orders;
        // 返回：剩余未成交数量，是否需要马上删除该数量
        std::pair<OrderSize, bool> remainder = m_match_algorithm->Matches(matched_orders, order, opposite_working_orders);

        // 对每一个成交订单处理
        for(const auto &oppo : matched_orders)
        {
            pb::ems::Order matched = *oppo.match_order;
            LOG_INFO("order matched: ", fh::core::assist::utility::Format_pb_message(matched));
            LOG_INFO("match price: ", TO_REAL_PRICE(oppo.match_price), ", match size: ", oppo.match_quantity);

            // 记录订单成交信息并发送出去
            std::string fill_id = this->Next_fill_id();
            pb::ems::Fill fill = m_order_manager.Fill_order(&order, fill_id, oppo.match_quantity, oppo.match_price);
            m_exchange_listener->OnFill(fill);
            pb::ems::Fill wfill = m_order_manager.Fill_working_order(oppo.match_order, fill_id, oppo.match_quantity, oppo.match_price);
            m_exchange_listener->OnFill(wfill);

            // 发送交易信息
            m_market_manager.Send_trade(order.contract(), oppo.match_price, oppo.match_quantity);
            // 发送行情信息
            m_market_manager.Change_market_on_order_filled(&matched, oppo.match_quantity);
        }

        LOG_INFO("remainder unmatch size: ", remainder.first, ", cancel?:", remainder.second);

        // 还有剩余数量的话
        if(remainder.first > 0)
        {
            // 记录剩余数量和已成交数量
            order.set_filled_quantity(order.quantity() - remainder.first);
            order.set_quantity(remainder.first);

            if(remainder.second)
            {
                // 订单的剩余数量需要马上删除掉
                const pb::ems::Order *deleted_order = m_order_manager.Add_deleted_order(order);
                m_exchange_listener->OnOrder(TradeSimulater::Make_cancel_response(*deleted_order, Message::ORDER_CANCELLED));
            }
            else
            {
                // 订单的剩余数量需要加入到未成交订单列表
                const pb::ems::Order *working_order = m_order_manager.Add_order(order);
                m_exchange_listener->OnOrder(*working_order);
                m_market_manager.Change_market_on_order_created(working_order);
            }
        }
    }

    // 一个订单处理请求被拒绝时，发送 reject 应答
    pb::ems::Order TradeSimulater::Make_reject_response(const pb::ems::Order& org_order, const std::string &reject_reason)
    {
        pb::ems::Order result(org_order);
        result.set_status(pb::ems::OrderStatus::OS_Rejected);
        result.set_message(reject_reason);
        LOG_DEBUG("reject response:", fh::core::assist::utility::Format_pb_message(result));
        return result;
    }

    // 一个订单被删除时，发送 cancel 应答
    pb::ems::Order TradeSimulater::Make_cancel_response(const pb::ems::Order& org_order, const std::string &cancel_reason)
    {
        pb::ems::Order result(org_order);
        result.set_status(pb::ems::OrderStatus::OS_Cancelled);
        result.set_message(cancel_reason);
        LOG_DEBUG("cancel response:", fh::core::assist::utility::Format_pb_message(result));
        return result;
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh
