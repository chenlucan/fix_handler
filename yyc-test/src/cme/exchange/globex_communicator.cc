
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
                fh::cme::exchange::Order order = this->Create_order(data + 1, size - 1);
                return m_order_manager.sendNewOrderSingleRequest(order);
            }
            else if(msg_type == 'F')     // Order Cancel Request
            {
                fh::cme::exchange::Order order = this->Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderCancelRequest(order);
            }
            else if(msg_type == 'G')     // Order Cancel-Replace Request
            {
                fh::cme::exchange::Order order = this->Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderCancelReplaceRequest(order);
            }
            else if(msg_type == 'H')     // Order Status Request
            {
                fh::cme::exchange::Order order = this->Create_order(data + 1, size - 1);
                return m_order_manager.sendOrderStatusRequest(order);
            }
            else if(msg_type == 'X')        // Order Mass Status Request
            {
                fh::cme::exchange::MassOrder mass_order = this->Create_mass_order(data + 1, size - 1);
                return m_order_manager.sendOrderMassStatusRequest(mass_order);
            }
            else if(msg_type == 'Y')        // Order Mass Action Request
            {
                fh::cme::exchange::MassOrder mass_order = this->Create_mass_order(data + 1, size - 1);
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

        fh::cme::exchange::Order order;
        order.cl_order_id = org_order.client_order_id();
        //order. = org_order.account();        // TODO 这个域目前放到了配置文件
        order.symbol = org_order.contract();
        order.security_desc = org_order.contract();      // 这个域和 symbol 使用同一个值
        order.side = this->Convert_buy_sell(org_order.buy_sell());
        order.price = atof(org_order.price().data());
        order.order_qty = org_order.quantity();
        order.time_in_force = this->Convert_tif(org_order.tif());
        order.order_type = this->Convert_order_type(org_order.order_type());
        order.order_id = org_order.exchange_order_id();
        //order. = org_order.status();         // TODO 这个域没有对应 FIX 项目
        //order. = org_order.working_price();        // TODO 这个域没有对应 FIX 项目
        //order. = org_order.working_quantity();         // TODO 这个域没有对应 FIX 项目
        //order. = org_order.filled_quantity();          // TODO 这个域没有对应 FIX 项目
        //order. = org_order.message();          // TODO 这个域没有对应 FIX 项目
        //order. = org_order.submit_time();          // TODO 这个域没有对应 FIX 项目
        //order.stop_px = ;                                  // TODO 这个域没有对应 protobuf 项目
        order.expire_date = "20170310";                          // TODO 这个域没有对应 protobuf 项目
        order.orig_cl_order_id = "ORG-ORDER-1";                 // TODO 这个域没有对应 protobuf 项目

        return order;
    }

    fh::cme::exchange::MassOrder GlobexCommunicator::Create_mass_order(const char *data, size_t size)
    {
        // TODO 目前 protobuf 中还没有定义
        fh::cme::exchange::MassOrder mass_order;
        return mass_order;
    }

    char GlobexCommunicator::Convert_tif(pb::ems::TimeInForce tif) const
    {
        switch(tif)
        {
            case pb::ems::TimeInForce::TIF_None:
                return '0';
            case pb::ems::TimeInForce::TIF_FAK:
                return '3';
            case pb::ems::TimeInForce::TIF_GFD:
                return '6';
            case pb::ems::TimeInForce::TIF_GTC:
                return '1';
            default:
                throw fh::cme::exchange::InvalidOrder("invalid tif");
        }
    }

    char GlobexCommunicator::Convert_order_type(pb::ems::OrderType type) const
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

    char GlobexCommunicator::Convert_buy_sell(pb::ems::BuySell bs) const
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

    pb::ems::BuySell GlobexCommunicator::Convert_buy_sell(char bs) const
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

    void GlobexCommunicator::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        std::string message_str = this->Create_order_result(report);
        if(!message_str.empty())
        {
            m_processor(const_cast<char *>(message_str.data()), message_str.length());
        }
    }

    pb::ems::OrderStatus GlobexCommunicator::Convert_order_status(char status) const
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
            fill.set_buy_sell(this->Convert_buy_sell(report.single_report.side));

            pb::ems::Timestamp ts = fh::core::assist::utility::Posix_time_to_pb(report.single_report.transact_time);
            fill.set_allocated_fill_time(&ts);

            return "F" + fill.SerializeAsString();
        }
        else
        {
            // 9：Order Cancel Reject  8：Execution Report （other）
            pb::ems::Order order;
            order.set_client_order_id(report.single_report.cl_order_id);
            order.set_account(report.single_report.account);
            order.set_contract(report.single_report.symbol);
            order.set_buy_sell(this->Convert_buy_sell(report.single_report.side));
            order.set_price(std::to_string(report.single_report.price));
            order.set_quantity(report.single_report.order_qty);
            //order.set_tif(pb::ems::TimeInForce::TIF_None);        // TODO 这个域暂时不回传了
            //order.set_order_type(pb::ems::OrderType::OT_Limit);        // TODO 这个域暂时不回传了
            order.set_exchange_order_id(report.single_report.order_id);
            order.set_status(this->Convert_order_status(report.single_report.order_status));
            order.set_working_price(std::to_string(report.single_report.price));
            order.set_working_quantity(report.single_report.leaves_qty);
            order.set_filled_quantity(report.single_report.cum_qty);
            order.set_message(report.single_report.text);

            pb::ems::Timestamp ts = fh::core::assist::utility::Posix_time_to_pb(report.single_report.transact_time);
            order.set_allocated_submit_time(&ts);

            return "O" + order.SerializeAsString();
        }
    }

} // namespace exchange
} // namespace cme
} // namespace fh
