
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "cme/exchange/order_manager.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "core/assist/time_measurer.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    OrderManager::OrderManager(const fh::cme::exchange::ExchangeSettings &app_settings)
    : m_current_received_seq()
    {
        auto target_id = app_settings.Get_target_id();
        auto sender_id = app_settings.Get_sender_id();
        auto password = app_settings.Get_password();
        auto app = app_settings.Get_app_info();
        m_target_comp_id = target_id.first;
        m_target_sub_id = target_id.second;
        m_sender_comp_id = std::get<0>(sender_id);
        m_sender_sub_id = std::get<1>(sender_id);
        m_sender_location_id = std::get<2>(sender_id);
        m_raw_data = password.first;
        m_raw_data_length = password.second;
        m_app_name = std::get<0>(app);
        m_trading_ver = std::get<1>(app);
        m_app_vendor = std::get<2>(app);
        m_account = app_settings.Get_account();
        m_manual_flag = (app_settings.Get_manual_flag() == "Y");
        m_security_type = app_settings.Get_security_type();
        m_customer_flag = (app_settings.Get_customer_flag() == "0" ? 0 : 1);
        m_cti_code = app_settings.Get_cti_code();
    }

    OrderManager::~OrderManager()
    {
        // noop
    }

    void OrderManager::setCallback(std::function<void(const fh::cme::exchange::OrderReport&)> processor)
    {
        m_processor = processor;
    }

    void OrderManager::onCreate(const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on create: ", sessionID);
    }

    void OrderManager::onLogon(const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on logon: ", sessionID);

        // 登录成功后先发送状态
        fh::cme::exchange::OrderReport result;
        result.message_type = "";   // 用这个空的 type 标识登录成功了
        m_processor(result);

        // 然后发送请求，查询所有订单状态
        this->Mass_order_status();
    }

    void OrderManager::onLogout(const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on Logout: ", sessionID);
    }

    void OrderManager::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // session 还没有创建的时候，直接返回
        // 对应服务器端还没有能够接受连接请求的时间段的启动
        if(!fh::core::assist::utility::Is_fix_session_exist(sessionID)) return;

        auto type = fh::core::assist::utility::Fix_message_type(message);

        // Logon message 的场合需要添加一些字段
        if( type == "A" )
        {
            this->appendLogonField(message, sessionID);
        }
        // Logout message 的场合需要添加一些字段
        else if( type == "5" )
        {
            this->appendLogoutField(message, sessionID);
        }
        // Resend request message 的场合需要添加一些字段
        else if( type == "2" )
        {
            this->appendResendRequestField(message, sessionID);
        }

        // 由于 quickfix 里面对 header 的设置比 CME 要求的少，这里把缺少的几个字段设置上
        this->appendHeaderFieldForCME(message, sessionID);

        LOG_DEBUG("to admin: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::DoNotSend)
    {
        // 由于 quickfix 里面对 header 的设置比 CME 要求的少，这里把缺少的几个字段设置上
        this->appendHeaderFieldForCME(message, sessionID);

        LOG_DEBUG("to app: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID)
        throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
    {
        crack(message, sessionID);
        this->setCurrentReceivedSeq(message, sessionID);
        LOG_DEBUG("from admin: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
        throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
    {
        crack(message, sessionID);
        this->setCurrentReceivedSeq(message, sessionID);
        LOG_DEBUG("from app: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::onMessage(const FIX42::ExecutionReport& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on ExecutionReport message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        fh::core::assist::TimeMeasurer t;

        fh::cme::exchange::OrderReport result;
        result.message_type = "8";
        result.single_report = this->processOrderResult(message);
        m_processor(result);

        LOG_INFO("order result processed used: ", t.Elapsed_nanoseconds(), "ns");
    }

    void OrderManager::onMessage(const FIX42::OrderCancelReject& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on OrderCancelReject message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        fh::core::assist::TimeMeasurer t;

        fh::cme::exchange::OrderReport result;
        result.message_type = "9";
        result.single_report = this->processOrderResult(message);
        m_processor(result);

        LOG_INFO("order result processed used: ", t.Elapsed_nanoseconds(), "ns");
    }

    void OrderManager::onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on Heartbeat message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on TestRequest message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
        // 收到这个消息后，quickfix 会自动回应一个 heartbeat 消息
    }

    void OrderManager::onMessage(const FIX42::SequenceReset& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on SequenceReset message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
        // 收到这个消息后，quickfix 会自动修改文件中当前 session 的 next sequence number
    }

    void OrderManager::onMessage(const FIX42::Reject& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on Reject message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
        // 收到这个消息说明发生了某些错误，比如送过去的 message 格式不对什么的
    }

    void OrderManager::onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on Logout message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::onMessage(const FIX42::BusinessMessageReject& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on business Reject message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
        // 收到这个消息说明发生了某些业务字段错误
    }

    void OrderManager::onMessage(const FIX42::Message & message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on other message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        if( fh::core::assist::utility::Fix_message_type(message)  == "BZ")
        {
            fh::core::assist::TimeMeasurer t;

            // 是 Order Mass Action Report 的场合需要返回到策略模块
            fh::cme::exchange::OrderReport result;
            result.message_type = "BZ";
            result.mass_report = this->processMassOrderResult(message);
            m_processor(result);

            LOG_INFO("process Order Mass Action Report used: ", t.Elapsed_nanoseconds(), "ns");
        }
    }

    void OrderManager::onMessage(const FIX42::ResendRequest& message, const FIX::SessionID& sessionID)
    {
        // 收到 resend request 后，quickfix 会自动根据配置文件中 PersistMessages 的值来决定发送什么信息回去：
        // PersistMessages =  N : 发送 sequence reset 消息回去
        // PersistMessages =  Y : 重新发送指定区间的消息回去（会设定上 PossDupFlag 和 OrigSendingTime）
        // 参考：Session.cpp 的 nextResendRequest 方法
        LOG_DEBUG("on ResendRequest message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    bool OrderManager::sendLogout(const std::string &text)
    {
        LOG_INFO("send Logout");
        FIX::Message logout = createLogout42(text);

        LOG_DEBUG("message(xml): ", logout.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(logout.toString()));

        return FIX::Session::sendToTarget(logout);
    }

    bool OrderManager::sendNewOrderSingleRequest(const fh::cme::exchange::Order &order)
    {
        LOG_INFO("send NewOrderSingle");
        FIX::Message fix_order = createNewOrderSingle42(
                order.order_type,
                order.cl_order_id,
                order.side,
                order.symbol,
                order.time_in_force,
                order.order_qty,
                order.price,
                order.stop_px,
                order.security_desc,
                order.expire_date);

        LOG_DEBUG("message(xml): ", fix_order.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(fix_order.toString()));

        return FIX::Session::sendToTarget(fix_order);
    }

    bool OrderManager::sendOrderCancelRequest(const fh::cme::exchange::Order &order)
    {
        LOG_INFO("send OrderCancelRequest");
        FIX::Message cancel = createOrderCancelRequest42(
                order.cl_order_id,
                order.orig_cl_order_id,
                order.side,
                order.symbol,
                order.order_id,
                order.security_desc);

        LOG_DEBUG("message(xml): ", cancel.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(cancel.toString()));

        return FIX::Session::sendToTarget(cancel);
    }

    bool OrderManager::sendOrderCancelReplaceRequest(const fh::cme::exchange::Order &order)
    {
        LOG_INFO("send CancelReplaceRequest");
        FIX::Message replace = createOrderCancelReplaceRequest42(
                order.order_type,
                order.cl_order_id,
                order.order_id,
                order.side,
                order.symbol,
                order.time_in_force,
                order.orig_cl_order_id,
                order.order_qty,
                order.price,
                order.stop_px,
                order.security_desc,
                order.expire_date);

        LOG_DEBUG("message(xml): ", replace.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(replace.toString()));

        return FIX::Session::sendToTarget(replace);
    }

    bool OrderManager::sendOrderStatusRequest(const fh::cme::exchange::Order &order)
    {
        LOG_INFO("send OrderStatusRequest");
        FIX::Message status = createOrderStatusRequest42(
                order.cl_order_id,
                order.symbol,
                order.side,
                order.order_id,
                order.security_desc);

        LOG_DEBUG("message(xml): ", status.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(status.toString()));

        return FIX::Session::sendToTarget(status);
    }

    bool OrderManager::sendOrderMassStatusRequest(const fh::cme::exchange::MassOrder &mass_order)
    {
        LOG_INFO("send OrderMassStatusRequest");
        FIX::Message status = createOrderMassStatusRequest42(
                mass_order.mass_status_req_id,
                mass_order.mass_status_req_type,
                mass_order.market_segment_id,
                mass_order.ord_status_req_type,
                mass_order.account,
                mass_order.symbol,
                mass_order.security_desc,
                mass_order.time_in_force);

        LOG_DEBUG("message(xml): ", status.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(status.toString()));

        return FIX::Session::sendToTarget(status);
    }

    bool OrderManager::sendOrderMassActionRequest(const fh::cme::exchange::MassOrder &mass_order)
    {
        LOG_INFO("send sendOrderMassActionRequest");
        FIX::Message action = createOrderMassActionRequest42(
                mass_order.cl_order_id,
                mass_order.mass_action_type,
                mass_order.mass_action_scope,
                mass_order.market_segment_id,
                mass_order.symbol,
                mass_order.security_desc,
                mass_order.mass_cancel_req_type,
                mass_order.account,
                mass_order.side,
                mass_order.order_type,
                mass_order.time_in_force);

        LOG_DEBUG("message(xml): ", action.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(action.toString()));

        return FIX::Session::sendToTarget(action);
    }

    void OrderManager::Mass_order_status()
    {
        fh::cme::exchange::MassOrder mass_order;
        mass_order.mass_status_req_id = "S" + std::to_string(fh::core::assist::utility::Current_time_ns());   // 使用 "S + 当前时间" 作为 ID
        mass_order.mass_status_req_type = 7;    //  1: Instrument  3: Instrument Group  7: All Orders
        bool isSuccess = this->sendOrderMassStatusRequest(mass_order);
        LOG_INFO("mass order status processed:", isSuccess);

//        // TEST FOR Order Mass Action Request
//        mass_order.cl_order_id =  mass_order.mass_status_req_id;
//        mass_order.mass_action_type = 3;
//        mass_order.mass_action_scope = 1;
//        mass_order.security_desc = "TEST";
//        bool isSuccess = this->sendOrderMassActionRequest(mass_order);
//        LOG_INFO("mass order action processed:", isSuccess);
    }

    FIX42::Logout OrderManager::createLogout42(const std::string &text)
    {
        FIX42::Logout logout;
        logout.set(FIX::Text(text));
        setHeader(logout.getHeader());
        return logout;
    }

    FIX42::NewOrderSingle OrderManager::createNewOrderSingle42(
            char order_type,
            const std::string &cl_order_id,
            char side,
            const std::string &symbol,
            char time_in_force,
            std::uint64_t order_qty,
            double price,
            double stop_px,
            const std::string &security_desc,
            const std::string &expire_date)
    {
        FIX42::NewOrderSingle newOrderSingle;
        newOrderSingle.set(FIX::Account(m_account));
        newOrderSingle.set(FIX::ClOrdID(cl_order_id));
        newOrderSingle.set(FIX::HandlInst('1'));     // 1: Automated execution
        newOrderSingle.set(FIX::OrderQty(order_qty));
        newOrderSingle.set(FIX::OrdType(order_type));
        newOrderSingle.set(FIX::Side(side));
        newOrderSingle.set(FIX::Symbol(symbol));
        if(time_in_force != 0) newOrderSingle.set(FIX::TimeInForce(time_in_force));
        newOrderSingle.set(FIX::TransactTime(true));
        newOrderSingle.set(FIX::SecurityDesc(security_desc));
        newOrderSingle.set(FIX::SecurityType(m_security_type));
        newOrderSingle.set(FIX::CustomerOrFirm(m_customer_flag));
        //newOrderSingle.set(FIX::ExpireDate(expire_date)); // 目前用不到  tag 59-TimeInForce=Good Till Date (GTD)

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        newOrderSingle.setField(fh::cme::exchange::CmeFixField::CtiCode, m_cti_code);
        newOrderSingle.setField(fh::cme::exchange::CmeFixField::CorrelationClOrdID, cl_order_id);

        // 下面的 tag，目前 quickfix 的 NewOrderSingle 不支持，所以用下面这种方式设置
        newOrderSingle.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");

        if (order_type == FIX::OrdType_LIMIT || order_type == FIX::OrdType_STOP_LIMIT)
        {
            newOrderSingle.set(FIX::Price(price));
        }

        // 目前用不到 OrdType = stop ， stop-limit
        //if (order_type == FIX::OrdType_STOP || order_type == FIX::OrdType_STOP_LIMIT)
        //{
        //    newOrderSingle.set(FIX::StopPx(stop_px));
        //}

        setHeader(newOrderSingle.getHeader());

        return newOrderSingle;
    }

    FIX42::OrderCancelRequest OrderManager::createOrderCancelRequest42(
            const std::string &cl_order_id,
            const std::string &orig_cl_order_id,
            char side,
            const std::string &symbol,
            const std::string &order_id,
            const std::string &security_desc)
    {
        FIX42::OrderCancelRequest orderCancelRequest;
        orderCancelRequest.set(FIX::Account(m_account));
        orderCancelRequest.set(FIX::ClOrdID(cl_order_id));
        orderCancelRequest.set(FIX::OrderID(order_id));
        orderCancelRequest.set(FIX::OrigClOrdID(orig_cl_order_id));
        orderCancelRequest.set(FIX::Side(side));
        //orderCancelRequest.set(FIX::Symbol(symbol));		// 目前用不到
        orderCancelRequest.set(FIX::TransactTime(true));
        orderCancelRequest.set(FIX::SecurityDesc(security_desc));
        orderCancelRequest.set(FIX::SecurityType(m_security_type));

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        orderCancelRequest.setField(fh::cme::exchange::CmeFixField::CorrelationClOrdID, cl_order_id);

        // 下面的 tag，目前 quickfix 的 OrderCancelRequest 不支持，所以用下面这种方式设置
        orderCancelRequest.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");

        setHeader(orderCancelRequest.getHeader());

        return orderCancelRequest;
    }

    FIX42::OrderCancelReplaceRequest OrderManager::createOrderCancelReplaceRequest42(
            char order_type,
            const std::string &cl_order_id,
            const std::string &order_id,
            char side,
            const std::string &symbol,
            char time_in_force,
            const std::string &orig_cl_order_id,
            std::uint64_t order_qty,
            double price,
            double stop_px,
            const std::string &security_desc,
            const std::string &expire_date)
    {
        FIX42::OrderCancelReplaceRequest cancelReplaceRequest;
        cancelReplaceRequest.set(FIX::Account(m_account));
        cancelReplaceRequest.set(FIX::ClOrdID(cl_order_id));
        cancelReplaceRequest.set(FIX::OrderID(order_id));
        cancelReplaceRequest.set(FIX::HandlInst('1'));     // 1: Automated execution
        cancelReplaceRequest.set(FIX::OrderQty(order_qty));
        cancelReplaceRequest.set(FIX::OrdType(order_type));
        cancelReplaceRequest.set(FIX::OrigClOrdID(orig_cl_order_id));
        cancelReplaceRequest.set(FIX::Side(side));
        //cancelReplaceRequest.set(FIX::Symbol(symbol));		// 目前用不到
        if(time_in_force != 0) cancelReplaceRequest.set(FIX::TimeInForce(time_in_force));
        cancelReplaceRequest.set(FIX::TransactTime(true));
        cancelReplaceRequest.set(FIX::SecurityDesc(security_desc));
        cancelReplaceRequest.set(FIX::SecurityType(m_security_type));
        cancelReplaceRequest.set(FIX::CustomerOrFirm(m_customer_flag));
        //cancelReplaceRequest.set(FIX::ExpireDate(expire_date)); // 目前用不到  tag 59-TimeInForce=Good Till Date (GTD)

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        cancelReplaceRequest.setField(fh::cme::exchange::CmeFixField::CtiCode, m_cti_code);
        cancelReplaceRequest.setField(fh::cme::exchange::CmeFixField::CorrelationClOrdID, cl_order_id);

        // 下面的 tag，目前 quickfix 的 OrderCancelReplaceRequest 不支持，所以用下面这种方式设置
        cancelReplaceRequest.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");

        if (order_type == FIX::OrdType_LIMIT || order_type == FIX::OrdType_STOP_LIMIT)
        {
            cancelReplaceRequest.set(FIX::Price(price));
        }

        // 目前用不到 OrdType = stop ， stop-limit
        //if (order_type == FIX::OrdType_STOP || order_type == FIX::OrdType_STOP_LIMIT)
        //{
        //    cancelReplaceRequest.set(FIX::StopPx(stop_px));
        //}

        setHeader(cancelReplaceRequest.getHeader());

        return cancelReplaceRequest;
    }

    FIX42::OrderStatusRequest OrderManager::createOrderStatusRequest42(
            const std::string &cl_order_id,
            const std::string &symbol,
            char side,
            const std::string &order_id,
            const std::string &security_desc)
    {
        FIX42::OrderStatusRequest orderStatusRequest;
        orderStatusRequest.set(FIX::ClOrdID(cl_order_id));
        orderStatusRequest.set(FIX::OrderID(order_id));
        orderStatusRequest.set(FIX::Side(side));
        orderStatusRequest.set(FIX::Symbol(symbol));
        orderStatusRequest.set(FIX::SecurityDesc(security_desc));
        orderStatusRequest.set(FIX::SecurityType(m_security_type));

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        orderStatusRequest.setField(fh::cme::exchange::CmeFixField::CorrelationClOrdID, cl_order_id);

        // 下面的 tag，目前 quickfix 的 OrderStatusRequest 不支持，所以用下面这种方式设置
        orderStatusRequest.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");
        orderStatusRequest.setField(FIX::FIELD::TransactTime, FIX::TransactTime(true).getString());

        setHeader(orderStatusRequest.getHeader());

        return orderStatusRequest;
    }

    cme::exchange::message::OrderMassStatusRequest OrderManager::createOrderMassStatusRequest42(
            const std::string &mass_status_req_id,
            std::uint8_t mass_status_req_type,
            const std::string &market_segment_id,
            std::uint8_t ord_status_req_type,
            const std::string &account,
            const std::string &symbol,
            const std::string &security_desc,
            char time_in_force)
    {
        cme::exchange::message::OrderMassStatusRequest orderMassStatus;
        orderMassStatus.set(FIX::MassStatusReqID(mass_status_req_id));
        orderMassStatus.set(FIX::MassStatusReqType(mass_status_req_type));  // 1: Instrument  3: Instrument Group  7: All Orders
        //orderMassStatus.set(FIX::Account(account));       // 目前不需要设置
        if(mass_status_req_type == 3) orderMassStatus.set(FIX::Symbol(symbol));
        if(mass_status_req_type == 1) orderMassStatus.set(FIX::SecurityDesc(security_desc));

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置：目前不需要设置
        //orderMassStatus.setField(fh::cme::exchange::CmeFixField::OrdStatusReqType, std::to_string(ord_status_req_type));

        // 下面的 tag，目前 quickfix 的 OrderMassStatusRequest 不支持，所以用下面这种方式设置
        //orderMassStatus.setField(FIX::FIELD::MarketSegmentID, market_segment_id);     // 目前不需要设置
        //orderMassStatus.setField(FIX::FIELD::TimeInForce, std::string(1, time_in_force));   // 目前不需要设置
        orderMassStatus.setField(FIX::FIELD::TransactTime, FIX::TransactTime(true).getString());
        orderMassStatus.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");

        setHeader(orderMassStatus.getHeader());

        return orderMassStatus;
    }

    cme::exchange::message::OrderMassActionRequest OrderManager::createOrderMassActionRequest42(
            const std::string &cl_order_id,
            std::uint8_t mass_action_type,
            std::uint8_t mass_action_scope,
            const std::string &market_segment_id,
            const std::string &symbol,
            const std::string &security_desc,
            std::uint8_t mass_cancel_req_type,
            const std::string &account,
            char side,
            char order_type,
            char time_in_force)
    {
        cme::exchange::message::OrderMassActionRequest orderMassAction;
        orderMassAction.set(FIX::ClOrdID(cl_order_id));
        orderMassAction.set(FIX::MassActionType(mass_action_type));
        orderMassAction.set(FIX::MassActionScope(mass_action_scope));
        //orderMassAction.set(FIX::MarketSegmentID(market_segment_id));  // 目前不需要设置
        //orderMassAction.set(FIX::Symbol(symbol)); // 目前不需要设置
        orderMassAction.set(FIX::SecurityDesc(security_desc));
        //orderMassAction.set(FIX::Side(side));     // 目前不需要设置
        orderMassAction.set(FIX::TransactTime(true));

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        orderMassAction.setField(fh::cme::exchange::CmeFixField::MassCancelRequestType, std::to_string(mass_cancel_req_type));

        // 下面的 tag，目前 quickfix 的 OrderMassActionRequest 不支持，所以用下面这种方式设置
        orderMassAction.setField(FIX::FIELD::Account, account);
        orderMassAction.setField(FIX::FIELD::OrdType, std::string(1, order_type));
        orderMassAction.setField(FIX::FIELD::TimeInForce, std::string(1, time_in_force));
        orderMassAction.setField(FIX::FIELD::ManualOrderIndicator, m_manual_flag ? "Y" : "N");

        setHeader(orderMassAction.getHeader());

        return orderMassAction;
    }

    void OrderManager::setCurrentReceivedSeq(const FIX::Message& message, const FIX::SessionID& sessionID)
    {
        FIX::MsgSeqNum seq;
        message.getHeader().getField(seq);
        m_current_received_seq[sessionID.toString()] = seq.getValue();
    }

    std::uint32_t OrderManager::getCurrentReceivedSeq(const FIX::SessionID& sessionID)
    {
        auto index = m_current_received_seq.find(sessionID.toString());
        if(index == m_current_received_seq.end()) return 0;
        return index->second;
    }

    void OrderManager::setHeader(FIX::Header& header)
    {
        // 下面这两个要设置上去才能确定 session id
        header.setField(FIX::SenderCompID(m_sender_comp_id));
        header.setField(FIX::TargetCompID(m_target_comp_id));
    }

    void OrderManager::appendLogonField(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // FIX 里面做成 logon message（tag 35-MsgType=A）时，有些 CME 要求的字段没有赋值，在这边给他加上去
        message.setField(FIX::FIELD::RawDataLength, m_raw_data_length);
        message.setField(FIX::FIELD::RawData, m_raw_data);
        message.setField(FIX::FIELD::EncryptMethod, "0");
        message.setField(FIX::FIELD::ResetSeqNumFlag, "N");

        message.setField(fh::cme::exchange::CmeFixField::ApplicationSystemName, m_app_name);
        message.setField(fh::cme::exchange::CmeFixField::TradingSystemVersion, m_trading_ver);
        message.setField(fh::cme::exchange::CmeFixField::ApplicationSystemVendor, m_app_vendor);
    }

    void OrderManager::appendLogoutField(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // FIX 里面做成 logout message（tag 35-MsgType=5）时，有些 CME 要求的字段没有赋值，在这边给他加上去
        message.setField(FIX::FIELD::NextExpectedMsgSeqNum, fh::core::assist::utility::Next_expected_seq_num(sessionID));
    }

    void OrderManager::appendResendRequestField(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // CME 要求一次最大请求的消息数是 2500，大于这个数字的话先只发送 2500 个（设置 EndSeqNo）
        FIX::BeginSeqNo b;
        message.getField(b);
        int begin = b.getValue();
        int end = this->getCurrentReceivedSeq(sessionID);

        if(end - begin  > MAX_RESEND_SIZE)
        {
            message.setField(FIX::EndSeqNo(begin + MAX_RESEND_SIZE - 1));
        }

        // 在配置文件中通过设定 SendRedundantResendRequests 来控制是否发送重复的 resend request，
        // CME 要求每次都发送这些重复的 resend request，所以该配置项设置成了 Y，但是需要带上 PossDupFlag = Y 发送到服务器；
        // 目前 quickfix 中重复发送的时候没有带上 PossDupFlag，所以在这里加上去
        if(fh::core::assist::utility::Is_duplicate_resend_request(sessionID))
        {
            message.getHeader().setField(FIX::PossDupFlag(true));
        }
    }

    void OrderManager::appendHeaderFieldForCME(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // 给 quickfix 做成的 message 的 header 部分添加一些 CME 要求必填的字段
        message.getHeader().setField(FIX::FIELD::SenderSubID, m_sender_sub_id);
        message.getHeader().setField(FIX::FIELD::TargetSubID, m_target_sub_id);
        message.getHeader().setField(FIX::FIELD::LastMsgSeqNumProcessed, fh::core::assist::utility::Last_processed_seq_num(sessionID));
        message.getHeader().setField(FIX::FIELD::SenderLocationID, m_sender_location_id);
    }

    fh::cme::exchange::SingleOrderReport OrderManager::processOrderResult(const FIX42::Message& message) const
    {
        fh::cme::exchange::SingleOrderReport report;
        OrderManager::Try_pick_value(message, report.exec_type, FIX::ExecType());
        OrderManager::Try_pick_value(message, report.cl_order_id, FIX::ClOrdID());
        OrderManager::Try_pick_value(message, report.account, FIX::Account());
        OrderManager::Try_pick_value(message, report.order_id, FIX::OrderID());
        OrderManager::Try_pick_value(message, report.exec_id, FIX::ExecID());
        OrderManager::Try_pick_value(message, report.orig_cl_order_id, FIX::OrigClOrdID());
        OrderManager::Try_pick_value(message, report.order_status, FIX::OrdStatus());
        OrderManager::Try_pick_value(message, report.order_type, FIX::OrdType());
        OrderManager::Try_pick_value(message, report.side, FIX::Side());
        OrderManager::Try_pick_value(message, report.symbol, FIX::Symbol());
        OrderManager::Try_pick_value(message, report.security_id, FIX::SecurityID());
        OrderManager::Try_pick_value(message, report.security_desc, FIX::SecurityDesc());
        OrderManager::Try_pick_value(message, report.cancel_rej_response_to, FIX::CxlRejResponseTo());
        OrderManager::Try_pick_value(message, report.time_in_force, FIX::TimeInForce());
        OrderManager::Try_pick_value(message, report.cum_qty, FIX::CumQty());
        OrderManager::Try_pick_value(message, report.leaves_qty, FIX::LeavesQty());
        OrderManager::Try_pick_value(message, report.order_qty, FIX::OrderQty());
        OrderManager::Try_pick_value(message, report.price, FIX::Price());
        OrderManager::Try_pick_value(message, report.last_px, FIX::LastPx());
        OrderManager::Try_pick_value(message, report.text, FIX::Text());
        OrderManager::Try_pick_string(message, report.transact_time, FIX::FIELD::TransactTime);
        OrderManager::Try_pick_value(message, report.trade_date, FIX::TradeDate());
        OrderManager::Try_pick_int(message, report.last_qty, FIX::FIELD::LastQty);
        OrderManager::Try_pick_value(message, report.mass_status_req_id, FIX::MassStatusReqID());
        OrderManager::Try_pick_int(message, report.rej_reason, fh::core::assist::utility::Fix_message_type(message) == "9"
                                                                                                        ? FIX::FIELD::CxlRejReason
                                                                                                        : (report.exec_type == 'I' ? FIX::FIELD::BusinessRejectReason : FIX::FIELD::OrdRejReason));

        return report;
    }

    fh::cme::exchange::MassOrderReport OrderManager::processMassOrderResult(const FIX42::Message &message) const
    {
        fh::cme::exchange::MassOrderReport report;
        OrderManager::Try_pick_value(message, report.cl_order_id, FIX::ClOrdID());
        OrderManager::Try_pick_value(message, report.mass_action_response, FIX::MassActionResponse());
        OrderManager::Try_pick_value(message, report.total_affected_orders, FIX::TotalAffectedOrders());
        OrderManager::Try_pick_value(message, report.no_affected_orders, FIX::NoAffectedOrders());
        OrderManager::Try_pick_string(message, report.transact_time, FIX::FIELD::TransactTime);
        OrderManager::Try_pick_value(message, report.text, FIX::Text());

        std::vector<std::string> orig_cl_order_ids;
        for ( std::uint32_t i = 1; i <= report.no_affected_orders; ++i )
        {
            FIX::OrigClOrdID ocoi;
            cme::exchange::message::OrderMassActionReport::NoAffectedOrders orders;
            message.getGroup( i, orders );
            orig_cl_order_ids.push_back(orders.get( ocoi ).getValue());
        }
        report.orig_cl_order_ids = boost::algorithm::join(orig_cl_order_ids, "^");

        return report;
    }

    // 从指定的 FIX message 中提取指定类型的字段的值
    template <typename FIXFieldType, typename Type>
    void OrderManager::Try_pick_value(const FIX::Message &message, Type &target, const FIXFieldType &fft)
    {
        FIXFieldType a(fft);
        target = message.getFieldIfSet(a) ? (Type)a : Type();
    }

    // 从指定的 FIX message 中提取指定 tag 的字段的原始值
    void OrderManager::Try_pick_string(const FIX::Message &message, std::string &target, int tagnum)
    {
        FIX::FieldBase fb(tagnum, "");
        target = message.getFieldIfSet(fb) ? fb.getString() : "";
    }

    // 从指定的 FIX message 中提取指定 tag 的字段的整数值
    template <typename IntType>
    void OrderManager::Try_pick_int(const FIX::Message &message, IntType &target, int num)
    {
        FIX::FieldBase fb(num, "");
        target = (message.getFieldIfSet(fb) && !fb.getString().empty()) ? boost::lexical_cast<IntType>(fb.getString()) : 0;
    }

} // namespace exchange
} // namespace cme
} // namespace fh
