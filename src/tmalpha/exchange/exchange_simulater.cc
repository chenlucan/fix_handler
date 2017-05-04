
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "tmalpha/exchange/exchange_simulater.h"
#include "tmalpha/market/market_data_provider.h"
#include "tmalpha/market/cme_data_consumer.h"
#include "tmalpha/exchange/market_replay_listener.h"

namespace fh
{
namespace tmalpha
{
namespace exchange
{

    ExchangeSimulater::ExchangeSimulater(
            fh::tmalpha::market::MarketSimulater *market,
            fh::core::exchange::ExchangeListenerI *result_listener)
    : fh::core::exchange::ExchangeI(result_listener),
      m_market(market), m_result_listener(result_listener), m_init_orders(),
      m_exchange_order_id(0), m_fill_id(0),
      m_working_orders(), m_working_order_ids(), m_filled_orders(), m_mutex(), m_current_states()
    {
        if(m_market && m_market->Listener())
        {
            // L2 行情发生变化时需要通知本模块
            auto market_listener = static_cast<fh::tmalpha::exchange::MarketReplayListener *>(m_market->Listener());
            market_listener->Add_l2_changed_callback(std::bind(&ExchangeSimulater::On_state_changed, this, std::placeholders::_1));
        }
    }

    ExchangeSimulater::~ExchangeSimulater()
    {
        // noop
    }

    // implement of ExchangeI
    bool ExchangeSimulater::Start(const std::vector<::pb::ems::Order> &init_orders)
    {
        m_init_orders = init_orders;
        LOG_INFO("init orders count:", init_orders.size());

        if(m_market == nullptr)
        {
            LOG_WARN("market is invalid");
            return false;
        }

        if(m_market->Is_runing())
        {
            LOG_INFO("market is already runing");
            return false;
        }

        LOG_INFO("start market");
        return m_market->Start();
    }

    void ExchangeSimulater::Join()
    {
        m_market->Join();
    }

    // implement of ExchangeI
    void ExchangeSimulater::Stop()
    {
        if(m_market == nullptr)
        {
            LOG_WARN("market is invalid");
            return;
        }

        if(!m_market->Is_runing())
        {
            LOG_INFO("market is already stopped");
            return;
        }

        LOG_INFO("stop market");
        m_market->Stop();
    }

    void ExchangeSimulater::On_state_changed(const pb::dms::L2 &l2)
    {
        // L2 情报发生变化时，内部需要记录，同时对尚未成交的订单重新匹配下
        std::lock_guard<std::mutex> lock(m_mutex);
        m_current_states[l2.contract()] = l2;
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
            m_working_order_ids.push_back(order.client_order_id());
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
        for(auto iter = m_working_order_ids.begin(); iter != m_working_order_ids.end();)
        {
            auto order = m_working_orders[*iter];
            if(this->Has_matching(order))
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
