
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "cme/exchange/globex_communicator.h"
#include "cme/exchange/order.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    GlobexCommunicator::GlobexCommunicator(
            core::exchange::ExchangeListenerI *strategy,
            const std::string &config_file,
            const fh::cme::exchange::ExchangeSettings &app_settings)
    : core::exchange::ExchangeI(strategy), m_strategy(strategy), m_order_manager(app_settings),
      m_settings(config_file), m_store(m_settings), m_logger(m_settings),
      m_initiator(m_order_manager, m_store, m_settings, m_logger), m_init_orders()
    {
        m_order_manager.setCallback(std::bind(&GlobexCommunicator::Order_response, this, std::placeholders::_1));
    }

    GlobexCommunicator::~GlobexCommunicator()
    {
        // noop
    }

    // implement of ExchangeI
    bool GlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
    {
        m_init_orders = init_orders;
        m_initiator.start();

        LOG_INFO("init orders count:", init_orders.size());

        // 一直要到所有的指定订单的状态都发回去了，才能返回
        while(!m_init_orders.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        LOG_INFO("all init orders status sent");

        return true;
    }

    // implement of ExchangeI
    void GlobexCommunicator::Stop()
    {
        // TODO 在下面这个方法中，quickfix 中做了 logout 处理
        // 但是该处理没有等待服务器端的回应，只是最多等待 10s 后就中断了连接
        // 如果此时服务器端有 resend request 请求，那么能不能在 10s 内完成？
        // 如果不能保证，则需要重写这个 stop 方法，以达到在确认收到 logout 回应时才断开连接
        m_initiator.stop();
    }

    // implement of ExchangeI
    void GlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
    {
        // noop
    }

    // implement of ExchangeI
    void GlobexCommunicator::Add(const ::pb::ems::Order& order)
    {
        // D: New Order
        auto order_d = GlobexCommunicator::Create_order(order);
        bool result = m_order_manager.sendNewOrderSingleRequest(order_d);
        LOG_INFO("add order :", result);
    }

    // implement of ExchangeI
    void GlobexCommunicator::Change(const ::pb::ems::Order& order)
    {
        // G: Order Cancel-Replace Request
        auto order_g = GlobexCommunicator::Create_order(order);
        bool result = m_order_manager.sendOrderCancelReplaceRequest(order_g);
        LOG_INFO("change order :", result);
    }

    // implement of ExchangeI
    void GlobexCommunicator::Delete(const ::pb::ems::Order& order)
    {
        // F: Order Cancel Request
        auto order_f = GlobexCommunicator::Create_order(order);
        bool result = m_order_manager.sendOrderCancelRequest(order_f);
        LOG_INFO("delete order :", result);
    }

    void GlobexCommunicator::Query(const ::pb::ems::Order& order)
    {
        // H: Order Status Request
        auto order_h = GlobexCommunicator::Create_order(order);
        bool result = m_order_manager.sendOrderStatusRequest(order_h);
        LOG_INFO("query order :", result);
    }

    void GlobexCommunicator::Query_mass(const char *data, size_t size)
    {
        // AF: Order Mass Status Request
        auto mass_order_af = GlobexCommunicator::Create_mass_order(data, size);
        bool result = m_order_manager.sendOrderMassStatusRequest(mass_order_af);
        LOG_INFO("query mass order :", result);
    }

    void GlobexCommunicator::Delete_mass(const char *data, size_t size)
    {
        // CA: Order Mass Action Request
        auto mass_order_ca = GlobexCommunicator::Create_mass_order(data, size);
        bool result = m_order_manager.sendOrderMassActionRequest(mass_order_ca);
        LOG_INFO("delete mass order :", result);
    }

    fh::cme::exchange::Order GlobexCommunicator::Create_order(const ::pb::ems::Order& strategy_order)
    {
        LOG_INFO("received order:  ", fh::core::assist::utility::Format_pb_message(strategy_order));

        fh::cme::exchange::Order order;
        order.cl_order_id = strategy_order.client_order_id();
        //order. = strategy_order.account();        // 这个域从配置文件读取
        order.symbol = strategy_order.contract();
        order.security_desc = strategy_order.contract();      // 这个域和 symbol 使用同一个值
        order.side = strategy_order.has_buy_sell() ? GlobexCommunicator::Convert_buy_sell(strategy_order.buy_sell()) : 0;
        order.price = std::stod(strategy_order.price());
        order.order_qty = strategy_order.quantity();
        order.time_in_force = strategy_order.has_tif() ? GlobexCommunicator::Convert_tif(strategy_order.tif()) : 0;
        order.order_type = strategy_order.has_order_type() ? GlobexCommunicator::Convert_order_type(strategy_order.order_type()) : 0;
        order.order_id = strategy_order.exchange_order_id();
        //order. = strategy_order.status();         // 这个域是回传订单结果的
        //order. = strategy_order.working_price();        // 这个域是回传订单结果的
        //order. = strategy_order.working_quantity();         // 这个域是回传订单结果的
        //order. = strategy_order.filled_quantity();          // 这个域是回传订单结果的
        //order. = strategy_order.message();          // 这个域是回传订单结果的
        //order. = strategy_order.submit_time();          // 这个域是回传订单结果的
        //order.stop_px = ;                                  // 这个域只在 OrdType = stop and stop-limit orders 时有用，目前不需要
        //order.expire_date = ;                          // 这个域只在 TimeInForce = Good Till Date (GTD) 时有用，目前不需要
        order.orig_cl_order_id = order.cl_order_id;                 // 这个域设置成和 cl_order_id 一样

        return order;
    }

    fh::cme::exchange::MassOrder GlobexCommunicator::Create_mass_order(const char *data, size_t size)
    {
        // TODO 目前还没有 protobuf 定义，暂时格式假设为：
        // id（20 byte）：CA 消息的场合是 cl_order_id，AF 消息的场合是 mass_status_req_id
        // req_type（1 byte）：CA 消息的场合不使用，AF 消息的场合是 mass_status_req_type
        // name（20 byte）：CA 消息的场合 security_desc，AF 消息的场合 security_desc 或者 symbol
        fh::cme::exchange::MassOrder mass_order{};
        mass_order.cl_order_id =  std::string(data, 20);                              // （CA）
        mass_order.mass_action_type = 3;      // （CA）
        mass_order.mass_action_scope = 1;               // （CA）     1: Instrument  9: Market Segment ID   10: Instrument Group
        mass_order.mass_status_req_id = mass_order.cl_order_id;                // （AF）
        mass_order.mass_status_req_type = data[20] - '0';          // （AF）         1: Instrument  3: Instrument Group  7: All Orders  100: Market Segment（不对应）
        mass_order.security_desc = size > 21 ? std::string(data + 21, size - 21) : "";                           // （CA/AF）
        mass_order.symbol = mass_order.security_desc;  // （AF）
        // 其他项目无需设置

        return mass_order;
    }

    void GlobexCommunicator::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        if(report.message_type == "")
        {
            // 登录成功，说明可以开始交易了
            m_strategy->OnExchangeReady(boost::container::flat_map<std::string, std::string>());
        }
        else if(report.message_type == "BZ")
        {
            // Order Mass Action Report
            return;      // TODO 目前 protobuf 中还没有定义
        }
        else if(report.message_type == "8" && report.single_report.order_status == 'H')
        {
            // Execution Report - Trade Cancel
            // 这个不用发送回去
            return;
        }
        else if(report.message_type == "8" && (report.single_report.order_status == '1' || report.single_report.order_status == '2'))
        {
            // Fill Notice
            pb::ems::Fill fill;
            fill.set_fill_id(report.single_report.exec_id);
            fill.set_fill_price(std::to_string(report.single_report.last_px));
            fill.set_fill_quantity(report.single_report.last_qty);
            fill.set_account(report.single_report.account);
            fill.set_client_order_id(report.single_report.cl_order_id);
            fill.set_exchange_order_id(report.single_report.order_id);
            fill.set_contract(report.single_report.security_desc);
            fill.set_buy_sell(GlobexCommunicator::Convert_buy_sell(report.single_report.side));
            fh::core::assist::utility::To_pb_time(fill.mutable_fill_time(), report.single_report.transact_time);

            m_strategy->OnFill(fill);
        }
        else
        {
            // 9：Order Cancel Reject  8：Execution Report （other）
            pb::ems::Order order;
            order.set_client_order_id(report.single_report.cl_order_id);
            order.set_account(report.single_report.account);
            order.set_contract(report.single_report.security_desc);
            order.set_buy_sell(GlobexCommunicator::Convert_buy_sell(report.single_report.side));
            order.set_price(std::to_string(report.single_report.price));
            order.set_quantity(report.single_report.order_qty);
            order.set_tif(GlobexCommunicator::Convert_tif(report.single_report.time_in_force));
            order.set_order_type(GlobexCommunicator::Convert_order_type(report.single_report.order_type));
            order.set_exchange_order_id(report.single_report.order_id);
            order.set_status(GlobexCommunicator::Convert_order_status(report.single_report.order_status));
            order.set_working_price(std::to_string(report.single_report.price));
            order.set_working_quantity(report.single_report.leaves_qty);
            order.set_filled_quantity(report.single_report.cum_qty);
            order.set_message(report.single_report.text);
            fh::core::assist::utility::To_pb_time(order.mutable_submit_time(), report.single_report.transact_time);

            m_strategy->OnOrder(order);

            if(report.message_type == "8" && report.single_report.exec_type == 'I')
            {
                // 是查询订单状态的应答消息的话，就消去该订单
                this->On_order_status_sent(report.single_report.cl_order_id);
            }
        }
    }

    // 当一个订单的状态送出去后，就把它从初期订单列表中去掉
    void GlobexCommunicator::On_order_status_sent(const std::string &cl_order_id)
    {
        LOG_DEBUG("order[", cl_order_id, "] status is got.");
        auto index = std::find_if(m_init_orders.begin(), m_init_orders.end(), [&cl_order_id](::pb::ems::Order &order){
            return order.client_order_id() == cl_order_id;
        });
        if(index != m_init_orders.end())
        {
            m_init_orders.erase(index);
            LOG_DEBUG("order[", cl_order_id, "] is removed from init list.");
        }
    }

    char GlobexCommunicator::Convert_tif(pb::ems::TimeInForce tif)
    {
        // TODO FAK 和 FOK 需要区分，FOK 的场合需要设置  tag 110（MinQty）
        switch(tif)
        {
            case pb::ems::TimeInForce::TIF_FAK:
            case pb::ems::TimeInForce::TIF_FOK:
                return '3';
            case pb::ems::TimeInForce::TIF_GFD:
                return '0';
            case pb::ems::TimeInForce::TIF_GTC:
                return '1';
            default:
                throw fh::core::strategy::InvalidOrder("invalid tif");
        }
    }

    pb::ems::TimeInForce GlobexCommunicator::Convert_tif(char tif)
    {
        switch(tif)
        {
            // TODO 要根据 tag 110（MinQty） 区分 FAK 和 FOK
            case '0':
                return pb::ems::TimeInForce::TIF_GFD;
            case '3':
                return pb::ems::TimeInForce::TIF_FAK;
            case '1':
                return pb::ems::TimeInForce::TIF_GTC;
            default:
                return pb::ems::TimeInForce::TIF_None;
        }
    }

    char GlobexCommunicator::Convert_order_type(pb::ems::OrderType type)
    {
        switch(type)
        {
            case pb::ems::OrderType::OT_Limit:
                return '2';
            case pb::ems::OrderType::OT_Market:
                return '1';
            default:
                throw fh::core::strategy::InvalidOrder("invalid order type");
        }
    }

    pb::ems::OrderType GlobexCommunicator::Convert_order_type(char type)
    {
        switch(type)
        {
            case '2':
                return pb::ems::OrderType::OT_Limit;
            case '1':
                return pb::ems::OrderType::OT_Market;
            default:
                return pb::ems::OrderType::OT_None;
        }
    }

    char GlobexCommunicator::Convert_buy_sell(pb::ems::BuySell bs)
    {
        switch(bs)
        {
            case pb::ems::BuySell::BS_Buy:
                return '1';
            case pb::ems::BuySell::BS_Sell:
                return '2';
            default:
                throw fh::core::strategy::InvalidOrder("invalid buy sell type");
        }
    }

    pb::ems::BuySell GlobexCommunicator::Convert_buy_sell(char bs)
    {
        switch(bs)
        {
            case '1':
                return pb::ems::BuySell::BS_Buy;
            case '2':
                return pb::ems::BuySell::BS_Sell;
            default:
                return pb::ems::BuySell::BS_None;
        }
    }

    pb::ems::OrderStatus GlobexCommunicator::Convert_order_status(char status)
    {
        switch(status)
        {
            case '0':
                return pb::ems::OrderStatus::OS_Pending;
            case '1':
                return pb::ems::OrderStatus::OS_Working;
            case '2':
                return pb::ems::OrderStatus::OS_Filled;
            case '4':
                return pb::ems::OrderStatus::OS_Pending;
            case '5':
                return pb::ems::OrderStatus::OS_Pending;
            case '8':
                return pb::ems::OrderStatus::OS_Rejected;
            case 'C':
                return pb::ems::OrderStatus::OS_Cancelled;
            case 'H':
                return pb::ems::OrderStatus::OS_Cancelled;
            case 'U':
                return pb::ems::OrderStatus::OS_None;
            default:
                return pb::ems::OrderStatus::OS_None;
        }
    }

} // namespace exchange
} // namespace cme
} // namespace fh
