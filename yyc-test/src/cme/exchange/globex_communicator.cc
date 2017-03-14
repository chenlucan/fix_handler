
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "cme/exchange/globex_communicator.h"
#include "cme/exchange/order.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    GlobexCommunicator::GlobexCommunicator(
            const std::string &config_file,
            const fh::cme::exchange::ExchangeSettings &app_settings,
            bool is_week_begin)
    : m_order_manager(app_settings, is_week_begin), m_settings(config_file), m_store(m_settings),
      m_logger(m_settings), m_initiator(m_order_manager, m_store, m_settings, m_logger)
    {
        m_order_manager.setCallback(std::bind(&GlobexCommunicator::Order_response, this, std::placeholders::_1));
    }

    GlobexCommunicator::~GlobexCommunicator()
    {
        // noop
    }

    void GlobexCommunicator::Start(std::function<void(char *data, size_t size)> processor)
    {
        m_processor = processor;
        m_initiator.start();
    }

    void GlobexCommunicator::Stop()
    {
        // TODO 在下面这个方法中，quickfix 中做了 logout 处理
        // 但是该处理没有等待服务器端的回应，只是最多等待 10s 后就中断了连接
        // 如果此时服务器端有 resend request 请求，那么能不能在 10s 内完成？
        // 如果不能保证，则需要重写这个 stop 方法，以达到在确认收到 logout 回应时才断开连接
        m_initiator.stop();
    }

    bool GlobexCommunicator::Order_request(const char *data, size_t size)
    {
        try
        {
            // 第一个字节当作 MsgType：D/F/G/H/X(AF)/Y(CA)
            char msg_type = data[0];

            if(msg_type == 'D')     // New Order
            {
                fh::cme::exchange::Order order = GlobexCommunicator::Create_order(data + 1, size - 1);
                return m_order_manager.sendNewOrderSingleRequest(order);
            }
            else if(msg_type == 'F')     // Order Cancel Request
            {
                fh::cme::exchange::Order order = GlobexCommunicator::Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderCancelRequest(order);
            }
            else if(msg_type == 'G')     // Order Cancel-Replace Request
            {
                fh::cme::exchange::Order order = GlobexCommunicator::Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderCancelReplaceRequest(order);
            }
            else if(msg_type == 'H')     // Order Status Request
            {
                fh::cme::exchange::Order order = GlobexCommunicator::Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderStatusRequest(order);
            }
            else if(msg_type == 'X')        // Order Mass Status Request
            {
                fh::cme::exchange::MassOrder mass_order = GlobexCommunicator::Create_mass_order(data + 1, size - 1);
                return m_order_manager.sendOrderMassStatusRequest(mass_order);
            }
            else if(msg_type == 'Y')        // Order Mass Action Request
            {
                fh::cme::exchange::MassOrder mass_order = GlobexCommunicator::Create_mass_order(data + 1, size - 1);
                return m_order_manager.sendOrderMassActionRequest(mass_order);
            }

            LOG_WARN("unknow order message type: ", msg_type);
            return false;
        }
        catch(fh::cme::exchange::InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            return false;
        }
    }

    fh::cme::exchange::Order GlobexCommunicator::Create_order(const char *data, size_t size)
    {
        pb::ems::Order org_order;
        if(!org_order.ParseFromArray(data, size))
        {
            throw fh::cme::exchange::InvalidOrder("order parse error");
        }

        LOG_INFO("received order:  ", fh::core::assist::utility::Format_pb_message(org_order));

        fh::cme::exchange::Order order;
        order.cl_order_id = org_order.client_order_id();
        //order. = org_order.account();        // 这个域从配置文件读取
        order.symbol = org_order.contract();
        order.security_desc = org_order.contract();      // 这个域和 symbol 使用同一个值
        order.side = org_order.has_buy_sell() ? GlobexCommunicator::Convert_buy_sell(org_order.buy_sell()) : 0;
        order.price = atof(org_order.price().data());
        order.order_qty = org_order.quantity();
        order.time_in_force = org_order.has_tif() ? GlobexCommunicator::Convert_tif(org_order.tif()) : 0;
        order.order_type = org_order.has_order_type() ? GlobexCommunicator::Convert_order_type(org_order.order_type()) : 0;
        order.order_id = org_order.exchange_order_id();
        //order. = org_order.status();         // 这个域是回传订单结果的
        //order. = org_order.working_price();        // 这个域是回传订单结果的
        //order. = org_order.working_quantity();         // 这个域是回传订单结果的
        //order. = org_order.filled_quantity();          // 这个域是回传订单结果的
        //order. = org_order.message();          // 这个域是回传订单结果的
        //order. = org_order.submit_time();          // 这个域是回传订单结果的
        //order.stop_px = ;                                  // 这个域只在 OrdType = stop and stop-limit orders 时有用，目前不需要
        //order.expire_date = ;                          // 这个域只在 TimeInForce = Good Till Date (GTD) 时有用，目前不需要
        order.orig_cl_order_id = order.cl_order_id;                 // 这个域设置成和 cl_order_id 一样

        return order;
    }

    fh::cme::exchange::MassOrder GlobexCommunicator::Create_mass_order(const char *data, size_t size)
    {
        // TODO 目前还没有 protobuf 定义，暂时格式假设为：
        // id（20 byte）：CA 消息的场合是 cl_order_id，AF 消息的场合是 mass_status_req_id
        // name（20 byte）：CA 消息的场合 security_desc，AF 消息的场合没有这个数据
        fh::cme::exchange::MassOrder mass_order;
        mass_order.cl_order_id =  std::string(data, 20);                              // （CA）
        mass_order.mass_action_type = 3;      // （CA）
        mass_order.mass_action_scope = 1;               // （CA）     1: Instrument  9: Market Segment ID   10: Instrument Group
        mass_order.security_desc = size > 20 ? std::string(data + 20, size - 20) : "";                           // （CA）
        mass_order.mass_status_req_id = mass_order.cl_order_id;                // （AF）
        mass_order.mass_status_req_type = 7;          // （AF）         1: Instrument  3: Instrument Group  7: All Orders  100: Market Segment
        // 其他项目无需设置

        return mass_order;
    }

    void GlobexCommunicator::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        std::string message_str = GlobexCommunicator::Create_order_result(report);
        if(!message_str.empty())
        {
            m_processor(const_cast<char *>(message_str.data()), message_str.length());
        }
    }

    // 将接受到的 fix 消息变成 protobuf 的消息（第一个字节放消息类型）
    std::string GlobexCommunicator::Create_order_result(const fh::cme::exchange::OrderReport& report)
    {
        if(report.message_type == "BZ")
        {
            // Order Mass Action Report
            return "";      // TODO 目前 protobuf 中还没有定义
        }
        else if(report.message_type == "8" && report.single_report.order_status == 'H')
        {
            // Execution Report - Trade Cancel
            // 这个不用发送回去
            return "";
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
            fill.set_contract(report.single_report.symbol);
            fill.set_buy_sell(GlobexCommunicator::Convert_buy_sell(report.single_report.side));
            fh::core::assist::utility::To_pb_time(fill.mutable_fill_time(), report.single_report.transact_time);

            LOG_INFO("send order result:  (F)", fh::core::assist::utility::Format_pb_message(fill));

            return "F" + fill.SerializeAsString();
        }
        else
        {
            // 9：Order Cancel Reject  8：Execution Report （other）
            pb::ems::Order order;
            order.set_client_order_id(report.single_report.cl_order_id);
            order.set_account(report.single_report.account);
            order.set_contract(report.single_report.symbol);
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

            LOG_INFO("send order result:  (O)", fh::core::assist::utility::Format_pb_message(order));

            return "O" + order.SerializeAsString();
        }
    }

    char GlobexCommunicator::Convert_tif(pb::ems::TimeInForce tif)
    {
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
                throw fh::cme::exchange::InvalidOrder("invalid tif");
        }
    }

    pb::ems::TimeInForce GlobexCommunicator::Convert_tif(char tif)
    {
        switch(tif)
        {
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
                throw fh::cme::exchange::InvalidOrder("invalid order type");
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
                throw fh::cme::exchange::InvalidOrder("invalid buy sell type");
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
