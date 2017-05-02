
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "tmalpha/exchange/exchange_simulater.h"

namespace fh
{
namespace tmalpha
{
namespace exchange
{

    ExchangeSimulater::ExchangeSimulater(
            core::exchange::ExchangeListenerI *result_listener,
            const std::string &app_setting_file,
            const std::string &persist_setting_file)
    : core::exchange::ExchangeI(result_listener),
      m_result_listener(result_listener), m_market(nullptr), m_init_orders(),
      m_exchange_order_id(0), m_fill_id(0),
      m_working_orders(), m_filled_orders(), m_mutex(), m_current_states()
    {
        m_market = new fh::tmalpha::market::TmalphaMarketApplication(app_setting_file, persist_setting_file);
        m_market->Add_replay_listener(this);
    }

    ExchangeSimulater::~ExchangeSimulater()
    {
        delete m_market;
    }

    // implement of ExchangeI
    bool ExchangeSimulater::Start(const std::vector<::pb::ems::Order> &init_orders)
    {
        m_init_orders = init_orders;
        LOG_INFO("init orders count:", init_orders.size());

        return m_market->Start();
    }

    void ExchangeSimulater::Join()
    {
        m_market->Join();
    }

    // implement of ExchangeI
    void ExchangeSimulater::Stop()
    {
        m_market->Stop();
    }

    // implement of MarketReplayListener
    void ExchangeSimulater::On_state_changed(const std::unordered_map<std::string , pb::dms::L2> &states)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_current_states = states;
        this->Rematching();
    }

    // implement of ExchangeI
    void ExchangeSimulater::Initialize(std::vector<::pb::dms::Contract> contracts)
    {
        // noop
    }

    // implement of ExchangeI
    void ExchangeSimulater::Add(const ::pb::ems::Order& order)
    {
        LOG_INFO("new order:", fh::core::assist::utility::Format_pb_message(order));

        std::lock_guard<std::mutex> lock(m_mutex);
        if(this->Has_matching(order))
        {
            m_filled_orders[order.client_order_id()] = this->Order_filled(order, this->Next_exchange_order_id());
        }
        else
        {
            m_working_orders[order.client_order_id()] = this->Order_working(order);
        }
    }

    // implement of ExchangeI
    void ExchangeSimulater::Change(const ::pb::ems::Order& order)
    {
        // noop
    }

    // implement of ExchangeI
    void ExchangeSimulater::Delete(const ::pb::ems::Order& order)
    {
        // noop
    }

    // implement of ExchangeI
    void ExchangeSimulater::Query(const ::pb::ems::Order& order)
    {
        // noop
    }

    // implement of ExchangeI
    void ExchangeSimulater::Query_mass(const char *data, size_t size)
    {
        // noop
    }

    // implement of ExchangeI
    void ExchangeSimulater::Delete_mass(const char *data, size_t size)
    {
        // noop
    }

    bool ExchangeSimulater::Has_matching(const ::pb::ems::Order& org_order)
    {
        // 订单是 market order 的话（没指定价格），不能做价格匹配
        if(org_order.order_type() != pb::ems::OrderType::OT_Limit) return false;

        // 订单中的合约在行情状态数据中不存在的话，肯定不能匹配
        auto pos = m_current_states.find(org_order.contract());
        if(pos == m_current_states.end()) return false;

        if(org_order.buy_sell() == pb::ems::BuySell::BS_Buy)
        {
            for(const auto &dp : pos->second.offer())
            {
                if(dp.price() == std::stod(org_order.price())) return true;
            }
        }
        else if(org_order.buy_sell() == pb::ems::BuySell::BS_Sell)
        {
            for(const auto &dp : pos->second.bid())
            {
                if(dp.price() == std::stod(org_order.price())) return true;
            }
        }

        return false;
    }

    void ExchangeSimulater::Rematching()
    {
        LOG_INFO("rematching order...");

        // 行情变化后看看待处理订单中有没有能匹配价格的
        for(auto iter = m_working_orders.begin(); iter != m_working_orders.end();)
        {
            if(this->Has_matching(iter->second))
            {
                m_filled_orders[iter->second.client_order_id()] = this->Order_filled(iter->second, iter->second.exchange_order_id());
                m_working_orders.erase(iter++);
            }
            else
            {
                iter++;
            }
        }

        LOG_INFO("rematching end.");
    }

    pb::ems::Fill ExchangeSimulater::Order_filled(const ::pb::ems::Order& org_order, std::string next_exchange_order_id)
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
        fh::core::assist::utility::To_pb_time(fill.mutable_fill_time(), fh::core::assist::utility::Current_time_str().substr(0, 23));

        LOG_INFO("fill result:", fh::core::assist::utility::Format_pb_message(fill));
        m_result_listener->OnFill(fill);

        return fill;
    }

    pb::ems::Order ExchangeSimulater::Order_working(const ::pb::ems::Order& org_order)
    {
        pb::ems::Order order;
        order.set_client_order_id(org_order.client_order_id());
        order.set_account(org_order.account());
        order.set_contract(org_order.contract());
        order.set_buy_sell(org_order.buy_sell());
        order.set_price(org_order.price());
        order.set_quantity(org_order.quantity());
        order.set_tif(org_order.tif());
        order.set_order_type(org_order.order_type());
        order.set_exchange_order_id(this->Next_exchange_order_id());
        order.set_status(pb::ems::OrderStatus::OS_Working);
        order.set_working_price(org_order.price());
        order.set_working_quantity(org_order.quantity());
        order.set_filled_quantity(0);
        order.set_message("");
        fh::core::assist::utility::To_pb_time(order.mutable_submit_time(), fh::core::assist::utility::Current_time_str().substr(0, 23));

        LOG_INFO("order result:", fh::core::assist::utility::Format_pb_message(order));
        m_result_listener->OnOrder(order);

        return order;
    }

    std::string ExchangeSimulater::Next_exchange_order_id()
    {
        return "Order-" + std::to_string(++m_exchange_order_id);
    }

    std::string ExchangeSimulater::Next_fill_id()
    {
        return "Fill-" + std::to_string(++m_fill_id);
    }

} // namespace exchange
} // namespace tmalpha
} // namespace fh
