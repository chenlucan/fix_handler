
#include <boost/algorithm/string.hpp>
#include "cme/exchange/order_manager.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    OrderManager::OrderManager(
            const fh::cme::exchange::ExchangeSettings &app_settings,
            bool is_week_begin) : m_is_week_begin(is_week_begin)
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
        LOG_DEBUG("on create: ", sessionID, m_is_week_begin ? " (reset seq num)" : "");

        if(m_is_week_begin)
        {
            fh::core::assist::utility::Reset_seq_num(sessionID);
        }
    }

    void OrderManager::onLogon(const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on logon: ", sessionID);
    }

    void OrderManager::onLogout(const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on Logout: ", sessionID);
    }

    void OrderManager::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        auto type = message. getHeader().getField(FIX::FIELD::MsgType);

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
            // TODO CME 要求一次最大请求的消息数是 2500，大于这个数字的话需要分段请求
            // 需要按照上述需求修改 quickfix
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
        LOG_DEBUG("from admin: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
        throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
    {
        crack(message, sessionID);
        LOG_DEBUG("from app: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));
    }

    void OrderManager::onMessage(const FIX42::ExecutionReport& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on ExecutionReport message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        fh::cme::exchange::OrderReport result;
        result.message_type = "8";
        result.single_report = this->processOrderResult(message);

        m_processor(result);
    }

    void OrderManager::onMessage(const FIX42::OrderCancelReject& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on OrderCancelReject message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        fh::cme::exchange::OrderReport result;
        result.message_type = "9";
        result.single_report = this->processOrderResult(message);

        m_processor(result);
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

    void OrderManager::onMessage(const FIX50SP2::OrderMassActionReport& message, const FIX::SessionID& sessionID)
    {
        LOG_DEBUG("on order mass action message: ", sessionID, " - ", fh::core::assist::utility::Format_fix_message(message));

        fh::cme::exchange::OrderReport result;
        result.message_type = "BZ";
        result.mass_report = this->processOrderResult(message);

        m_processor(result);
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

        LOG_INFO("message(xml): ", logout.toXML());
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

        LOG_INFO("message(xml): ", fix_order.toXML());
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

        LOG_INFO("message(xml): ", cancel.toXML());
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

        LOG_INFO("message(xml): ", replace.toXML());
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

        LOG_INFO("message(xml): ", status.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(status.toString()));

        return FIX::Session::sendToTarget(status);
    }

    bool OrderManager::sendOrderMassStatusRequest(const fh::cme::exchange::MassOrder &mass_order)
    {
        LOG_INFO("send OrderMassStatusRequest");
        FIX::Message status = createOrderMassStatusRequest50sp2(
                mass_order.mass_status_req_id,
                mass_order.mass_status_req_type,
                mass_order.market_segment_id,
                mass_order.ord_status_req_type,
                mass_order.account,
                mass_order.symbol,
                mass_order.security_desc,
                mass_order.time_in_force);

        LOG_INFO("message(xml): ", status.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(status.toString()));

        return FIX::Session::sendToTarget(status);
    }

    bool OrderManager::sendOrderMassActionRequest(const fh::cme::exchange::MassOrder &mass_order)
    {
        LOG_INFO("send sendOrderMassActionRequest");
        FIX::Message action = createOrderMassActionRequest50sp2(
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

        LOG_INFO("message(xml): ", action.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(action.toString()));

        return FIX::Session::sendToTarget(action);
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
        FIX::Account ac(m_account);
        FIX::ClOrdID coi(cl_order_id);
        FIX::HandlInst hi('1');     // 1: Automated execution
        FIX::OrderQty oq(order_qty);
        FIX::OrdType ot(order_type);
        FIX::Price p(price);
        FIX::Side s(side);
        FIX::Symbol sym(symbol);
        FIX::TimeInForce tif(time_in_force);
        FIX::TransactTime tt;
        FIX::ManualOrderIndicator moi(m_manual_flag);
        FIX::StopPx sp(stop_px);
        FIX::SecurityDesc sd(security_desc);
        FIX::SecurityType st(m_security_type);
        FIX::CustomerOrFirm cof(m_customer_flag);
        FIX::ExpireDate ed(expire_date);
        // FIX::CtiCode cc(m_cti_code);     // TODO 这个 tag 目前 quickfix 不支持
        // FIX::CorrelationClOrdID ccoi(cl_order_id);    // TODO 这个 tag 目前 quickfix 不支持

        FIX42::NewOrderSingle newOrderSingle(coi, hi, sym, s, tt, ot);
        newOrderSingle.set(ac);
        newOrderSingle.set(oq);
        newOrderSingle.set(tif);
        // newOrderSingle.set(moi);    // TODO 这个 tag 目前 quickfix 的 NewOrderSingle 不支持
        newOrderSingle.set(sd);
        newOrderSingle.set(st);
        newOrderSingle.set(cof);
        newOrderSingle.set(ed);
        // newOrderSingle.set(cc);  // TODO 这个 tag 目前 quickfix 不支持
        // newOrderSingle.set(ccoi);  // TODO 这个 tag 目前 quickfix 不支持

        if (order_type == FIX::OrdType_LIMIT || order_type == FIX::OrdType_STOP_LIMIT)
        {
            newOrderSingle.set(p);
        }
        if (order_type == FIX::OrdType_STOP || order_type == FIX::OrdType_STOP_LIMIT)
        {
            newOrderSingle.set(sp);
        }

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
        FIX::Account ac(m_account);
        FIX::ClOrdID coi(cl_order_id);
        FIX::OrderID oi(order_id);
        FIX::OrigClOrdID ocoi(orig_cl_order_id);
        FIX::Side s(side);
        FIX::Symbol sym(symbol);
        FIX::TransactTime tt;
        FIX::ManualOrderIndicator moi(m_manual_flag);
        FIX::SecurityDesc sd(security_desc);
        FIX::SecurityType st(m_security_type);
        // FIX::CorrelationClOrdID ccoi(cl_order_id);    // TODO 这个 tag 目前 quickfix 不支持

        FIX42::OrderCancelRequest orderCancelRequest(ocoi, coi, sym, s, tt);
        orderCancelRequest.set(ac);
        orderCancelRequest.set(oi);
        // orderCancelRequest.set(moi);    // TODO 这个 tag 目前 quickfix 的 OrderCancelRequest 不支持
        orderCancelRequest.set(sd);
        orderCancelRequest.set(st);
        // orderCancelRequest.set(ccoi);  // TODO 这个 tag 目前 quickfix 不支持

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
        FIX::Account ac(m_account);
        FIX::ClOrdID coi(cl_order_id);
        FIX::OrderID oi(order_id);
        FIX::HandlInst hi('1');     // 1: Automated execution
        FIX::OrderQty oq(order_qty);
        FIX::OrdType ot(order_type);
        FIX::OrigClOrdID ocoi(orig_cl_order_id);
        FIX::Price p(price);
        FIX::Side s(side);
        FIX::Symbol sym(symbol);
        FIX::TimeInForce tif(time_in_force);
        FIX::ManualOrderIndicator moi(m_manual_flag);
        FIX::TransactTime tt;
        FIX::StopPx sp(stop_px);
        FIX::SecurityDesc sd(security_desc);
        FIX::SecurityType st(m_security_type);
        FIX::CustomerOrFirm cof(m_customer_flag);
        FIX::ExpireDate ed(expire_date);
        // FIX::CtiCode cc(m_cti_code);     // TODO 这个 tag 目前 quickfix 不支持
        // FIX::CorrelationClOrdID ccoi(cl_order_id);    // TODO 这个 tag 目前 quickfix 不支持

        FIX42::OrderCancelReplaceRequest cancelReplaceRequest(ocoi, coi, hi, sym, s, tt, ot);
        cancelReplaceRequest.set(ac);
        cancelReplaceRequest.set(oi);
        cancelReplaceRequest.set(oq);
        cancelReplaceRequest.set(tif);
        // cancelReplaceRequest.set(moi);    // TODO 这个 tag 目前 quickfix 的 OrderCancelReplaceRequest 不支持
        cancelReplaceRequest.set(sd);
        cancelReplaceRequest.set(st);
        cancelReplaceRequest.set(cof);
        cancelReplaceRequest.set(ed);
        // cancelReplaceRequest.set(cc);  // TODO 这个 tag 目前 quickfix 不支持
        // cancelReplaceRequest.set(ccoi);  // TODO 这个 tag 目前 quickfix 不支持

        if (order_type == FIX::OrdType_LIMIT || order_type == FIX::OrdType_STOP_LIMIT)
        {
            cancelReplaceRequest.set(p);
        }
        if (order_type == FIX::OrdType_STOP || order_type == FIX::OrdType_STOP_LIMIT)
        {
            cancelReplaceRequest.set(sp);
        }

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
        FIX::ClOrdID coi(cl_order_id);
        FIX::OrderID oi(order_id);
        FIX::Side s(side);
        FIX::Symbol sym(symbol);
        FIX::ManualOrderIndicator moi(m_manual_flag);
        FIX::TransactTime tt;
        FIX::SecurityDesc sd(security_desc);
        FIX::SecurityType st(m_security_type);
        // FIX::CorrelationClOrdID ccoi(cl_order_id);    // TODO 这个 tag 目前 quickfix 不支持

        FIX42::OrderStatusRequest orderStatusRequest(coi, sym, s);
        orderStatusRequest.set(oi);
        //orderStatusRequest.set(moi);    // TODO 这个 tag 目前 quickfix 的 OrderStatusRequest 不支持
        //orderStatusRequest.set(tt);        // TODO 这个 tag 目前 quickfix 的 OrderStatusRequest 不支持
        orderStatusRequest.set(sd);
        orderStatusRequest.set(st);
        // orderStatusRequest.set(ccoi);  // TODO 这个 tag 目前 quickfix 不支持

        setHeader(orderStatusRequest.getHeader());

        return orderStatusRequest;
    }

    FIX50SP2::OrderMassStatusRequest OrderManager::createOrderMassStatusRequest50sp2(
            const std::string &mass_status_req_id,
            std::uint8_t mass_status_req_type,
            const std::string &market_segment_id,
            std::uint8_t ord_status_req_type,
            const std::string &account,
            const std::string &symbol,
            const std::string &security_desc,
            char time_in_force)
    {
        FIX::MassStatusReqID msri(mass_status_req_id);
        FIX::MassStatusReqType msrt(mass_status_req_type);
        FIX::MarketSegmentID msi(market_segment_id);
        //FIX::OrdStatusReqType osrt(ord_status_req_type);  // TODO 这个 tag 目前 quickfix 不支持
        FIX::Account a(account);
        FIX::Symbol s(symbol);
        FIX::SecurityDesc sd(security_desc);
        FIX::TimeInForce tif(time_in_force);
        FIX::TransactTime tt;
        FIX::ManualOrderIndicator moi(m_manual_flag);

        FIX50SP2::OrderMassStatusRequest orderMassStatus(msri, msrt);
        //orderMassStatus.set(msi);       // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassStatus.set(osrt);      // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        orderMassStatus.set(a);
        orderMassStatus.set(s);
        orderMassStatus.set(sd);
        //orderMassStatus.set(tif);           // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassStatus.set(tt);            // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassStatus.set(moi);        // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持

        setHeader(orderMassStatus.getHeader());

        return orderMassStatus;
    }

    FIX50SP2::OrderMassActionRequest OrderManager::createOrderMassActionRequest50sp2(
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
        FIX::ClOrdID coi(cl_order_id);
        FIX::MassActionType mat(mass_action_type);
        FIX::MassActionScope mas(mass_action_scope);
        FIX::MarketSegmentID msi(market_segment_id);
        FIX::Symbol sym(symbol);
        FIX::SecurityDesc sd(security_desc);
        FIX::MassCancelRequestType mcrt(mass_cancel_req_type);
        FIX::Account a(account);
        FIX::Side s(side);
        FIX::OrdType ot(order_type);
        FIX::TimeInForce tif(time_in_force);
        FIX::TransactTime tt;
        FIX::ManualOrderIndicator moi(m_manual_flag);

        FIX50SP2::OrderMassActionRequest orderMassAction(coi, mat, mas, tt);
        orderMassAction.set(msi);
        orderMassAction.set(sym);
        orderMassAction.set(sd);
        //orderMassAction.set(mcrt);      // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassAction.set(a);         // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        orderMassAction.set(s);
        //orderMassAction.set(ot);        // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassAction.set(tif);       // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持
        //orderMassAction.set(moi);   // TODO 这个 tag 目前 quickfix 的 OrderMassStatusRequest 不支持

        setHeader(orderMassAction.getHeader());

        return orderMassAction;
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

        // TODO 下面这几个 tag，FIX 不认识，需要修改 quickfix 对应
        //message.setField(1603, m_app_name);        // ApplicationSystemName
        //message.setField(1604, m_trading_ver);    // TradingSystemVersion
        //message.setField(1605, m_app_vendor);  // ApplicationSystemVendor
    }

    void OrderManager::appendLogoutField(FIX::Message& message, const FIX::SessionID& sessionID)
    {
        // FIX 里面做成 logout message（tag 35-MsgType=5）时，有些 CME 要求的字段没有赋值，在这边给他加上去
        // TODO 下面这个 tag，FIX 不认识，需要修改 quickfix 对应
        //message.setField(FIX::FIELD::NextExpectedMsgSeqNum, fh::core::assist::utility::Next_expected_seq_num(sessionID));
    }

    void OrderManager::appendResendRequestField(FIX::Message& message, const FIX::SessionID& sessionID)
    {
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

    fh::cme::exchange::SingleOrderReport OrderManager::processOrderResult(const FIX42::ExecutionReport& message) const
    {
        FIX::ExecType et;
        FIX::ClOrdID coi;
        FIX::Account a;
        FIX::OrderID oi;
        FIX::MassStatusReqID msri;
        FIX::ExecID ei;
        FIX::OrigClOrdID ocoi;
        FIX::OrdStatus os;
        FIX::OrdType ot;
        FIX::Side s;
        FIX::Symbol sym;
        FIX::SecurityID si;
        FIX::SecurityDesc sd;
        FIX::CumQty cq;
        FIX::LeavesQty leq;
        FIX::OrderQty oq;
        FIX::Price p;
        FIX::LastPx lp;
        FIX::LastQty lq;
        FIX::BusinessRejectReason brr;
        FIX::OrdRejReason orr;
        FIX::Text t;
        FIX::TransactTime tt;
        FIX::TradeDate td;

        message. get(et);
        message.get(coi);
        message. get(a);
        message.get(oi);
        //message.get(msri);      // TODO 这个 tag 目前 quickfix 的 ExecutionReport 不支持
        message.get(ei);
        message.get(ocoi);
        message.get(os);
        message.get(ot);
        message.get(s);
        message.get(sym);
        message.get(si);
        message.get(sd);
        message.get(cq);
        message.get(leq);
        message.get(oq);
        message.get(p);
        message.get(lp);
        //message.get(lq);        // TODO 这个 tag 目前 quickfix 的 ExecutionReport 不支持
        //message.get(brr);       // TODO 这个 tag 目前 quickfix 的 ExecutionReport 不支持
        message.get(orr);
        message.get(t);
        message.get(tt);
        message.get(td);

        fh::cme::exchange::SingleOrderReport report;
        report.exec_type = et;
        report.cl_order_id = coi;
        report.account = a;
        report.order_id = oi;
        report.mass_status_req_id = msri;
        report.exec_id = ei;
        report.orig_cl_order_id = ocoi;
        report.order_status = os;
        report.order_type = ot;
        report.side = s;
        report.symbol = sym;
        report.security_id = si;
        report.security_desc = sd;
        report.cum_qty = cq;
        report.leaves_qty = leq;
        report.order_qty = oq;
        report.price = p;
        report.last_px = lp;
        report.last_qty = lq;
        report.rej_reason = (et == 'I' ? brr : orr);
        report.text = t;
        report.transact_time = fh::core::assist::utility::Fix_time_to_posix(tt.getValue());
        report.trade_date = td;

        return report;
    }

    fh::cme::exchange::SingleOrderReport OrderManager::processOrderResult(const FIX42::OrderCancelReject& message) const
    {
        FIX::Account a;
        FIX::ClOrdID coi;
        FIX::ExecID ei;
        FIX::OrderID oi;
        FIX::OrdStatus os;
        FIX::OrigClOrdID ocoi;
        FIX::SecurityID si;
        FIX::Text t;
        FIX::TransactTime tt;
        FIX::CxlRejReason crr;
        FIX::SecurityDesc sd;
        FIX::CxlRejResponseTo crrt;

        message. get(a);
        message.get(coi);
        //message.get(ei);         // TODO 这个 tag 目前 quickfix 的 OrderCancelReject 不支持
        message.get(oi);
        message.get(os);
        message.get(ocoi);
        //message.get(si);        // TODO 这个 tag 目前 quickfix 的 OrderCancelReject 不支持
        message.get(t);
        message.get(tt);
        message.get(crr);
        //message.get(sd);        // TODO 这个 tag 目前 quickfix 的 OrderCancelReject 不支持
        message.get(crrt);

        fh::cme::exchange::SingleOrderReport report;
        report.cl_order_id = coi;
        report.account = a;
        report.order_id = oi;
        report.exec_id = ei;
        report.orig_cl_order_id = ocoi;
        report.order_status = os;
        report.security_id = si;
        report.security_desc = sd;
        report.cancel_rej_response_to = crrt;
        report.rej_reason = crr;
        report.text = t;
        report.transact_time = fh::core::assist::utility::Fix_time_to_posix(tt.getValue());

        return report;
    }

    fh::cme::exchange::MassOrderReport OrderManager::processOrderResult(const FIX50SP2::OrderMassActionReport& message) const
    {
        FIX::ClOrdID coi;
        FIX::MassActionResponse mar;
        FIX::TotalAffectedOrders tao;
        FIX::NoAffectedOrders nao;
        FIX50SP2::OrderMassActionReport::NoAffectedOrders orders;
        FIX::TransactTime tt;
        FIX::Text t;

        message.get(coi);
        message.get(mar);
        message.get(tao);
        message.get(nao);
        message.get(tt);
        message.get(t);

        std::vector<std::string> orig_cl_order_ids;
        for ( int i = 1; i <= nao; ++i )
        {
            FIX::OrigClOrdID ocoi;
            message.getGroup( i, orders );
            orders.get( ocoi );
            orig_cl_order_ids.push_back(ocoi.getValue());
        }

        fh::cme::exchange::MassOrderReport report;
        report.cl_order_id = coi;
        report.mass_action_response = mar;
        report.total_affected_orders = tao;
        report.no_affected_orders = nao;
        report.orig_cl_order_ids = boost::algorithm::join(orig_cl_order_ids, "^");
        report.transact_time = fh::core::assist::utility::Fix_time_to_posix(tt.getValue());
        report.text = t;

        return report;
    }

} // namespace exchange
} // namespace cme
} // namespace fh
