#ifndef __FH_CME_EXCHANGE_ORDER_MANAGER_H__
#define __FH_CME_EXCHANGE_ORDER_MANAGER_H__

#include <unordered_map>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/Logout.h>
#include <quickfix/fix42/Reject.h>
#include <quickfix/fix42/ResendRequest.h>
#include <quickfix/fix42/TestRequest.h>
#include <quickfix/fix42/SequenceReset.h>
#include <quickfix/fix42/Heartbeat.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/BusinessMessageReject.h>
#include <quickfix/fix42/OrderCancelReject.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include "cme/exchange/message/OrderMassActionReport.h"
#include "cme/exchange/message/OrderMassStatusRequest.h"
#include "cme/exchange/message/OrderMassActionRequest.h"
#include "core/global.h"
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/order.h"

#define  MAX_RESEND_SIZE 2500

namespace fh
{
namespace cme
{
namespace exchange
{
    // cme 的有些 tag，quickfix 里面不支持，需要在这里单独定义
    enum CmeFixField
    {
        ApplicationSystemName = 1603,
        TradingSystemVersion = 1604,
        ApplicationSystemVendor = 1605,
        OrdStatusReqType = 5000,
        MassCancelRequestType = 6115,
        CtiCode = 9702,
        CorrelationClOrdID = 9717
    };

    class OrderManager: public FIX::Application, public FIX::MessageCracker
    {
        public:
            explicit OrderManager(const fh::cme::exchange::ExchangeSettings &app_settings);
            virtual ~OrderManager();

        public:
            void setCallback(std::function<void(const fh::cme::exchange::OrderReport&)> processor);

        public:
            bool sendLogout(const std::string &text);
            bool sendNewOrderSingleRequest(const fh::cme::exchange::Order &order);
            bool sendOrderCancelRequest(const fh::cme::exchange::Order &order);
            bool sendOrderCancelReplaceRequest(const fh::cme::exchange::Order &order);
            bool sendOrderStatusRequest(const fh::cme::exchange::Order &order);
            bool sendOrderMassStatusRequest(const fh::cme::exchange::MassOrder &mass_order);
            bool sendOrderMassActionRequest(const fh::cme::exchange::MassOrder &mass_order);

        private:
            void Mass_order_status();

        private:
            void onCreate(const FIX::SessionID&);
            void onLogon(const FIX::SessionID& sessionID);
            void onLogout(const FIX::SessionID& sessionID);
            void onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&);
            void onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&);
            void onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::SequenceReset& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::Reject& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::ResendRequest& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::BusinessMessageReject& message, const FIX::SessionID& sessionID);
            void onMessage(const FIX42::Message& message, const FIX::SessionID& sessionID);

        private:
            void toAdmin(FIX::Message&, const FIX::SessionID&);
            void toApp(FIX::Message&, const FIX::SessionID&) throw (FIX::DoNotSend);
            void fromAdmin(const FIX::Message&, const FIX::SessionID&)
                    throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
            void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
                    throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

        private:
            FIX42::Logout createLogout42(const std::string &text);
            FIX42::NewOrderSingle createNewOrderSingle42(
                    char order_type,
                    const std::string &cl_order_id,
                    char side,
                    const std::string &symbol,
                    char time_in_force,
                    std::uint64_t order_qty,
                    double price,
                    double stop_px,
                    const std::string &security_desc,
                    const std::string &expire_date);
            FIX42::OrderCancelRequest createOrderCancelRequest42(
                    const std::string &cl_order_id,
                    const std::string &orig_cl_order_id,
                    char side,
                    const std::string &symbol,
                    const std::string &order_id,
                    const std::string &security_desc);
            FIX42::OrderCancelReplaceRequest createOrderCancelReplaceRequest42(
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
                    const std::string &expire_date);
            FIX42::OrderStatusRequest createOrderStatusRequest42(
                    const std::string &cl_order_id,
                    const std::string &symbol,
                    char side,
                    const std::string &order_id,
                    const std::string &security_desc);
            cme::exchange::message::OrderMassStatusRequest createOrderMassStatusRequest42(
                    const std::string &mass_status_req_id,
                    std::uint8_t mass_status_req_type,
                    const std::string &market_segment_id,
                    std::uint8_t ord_status_req_type,
                    const std::string &account,
                    const std::string &symbol,
                    const std::string &security_desc,
                    char time_in_force);
            cme::exchange::message::OrderMassActionRequest createOrderMassActionRequest42(
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
                    char time_in_force);

        private:
            void setCurrentReceivedSeq(const FIX::Message& message, const FIX::SessionID& sessionID);
            std::uint32_t getCurrentReceivedSeq(const FIX::SessionID& sessionID);
            void setHeader(FIX::Header& header);
            void appendLogonField(FIX::Message& message, const FIX::SessionID& sessionID);
            void appendLogoutField(FIX::Message& message, const FIX::SessionID& sessionID);
            void appendResendRequestField(FIX::Message& message, const FIX::SessionID& sessionID);
            void appendHeaderFieldForCME(FIX::Message& message, const FIX::SessionID& sessionID);

        private:
            fh::cme::exchange::SingleOrderReport processOrderResult(const FIX42::Message& message) const;
            fh::cme::exchange::MassOrderReport  processMassOrderResult(const FIX42::Message& message) const;

        private:
            template <typename FIXFieldType, typename Type>
            static void Try_pick_value(const FIX::Message &message, Type &target, const FIXFieldType &fft);
            static void Try_pick_string(const FIX::Message &message, std::string &target, int tagnum);
            template <typename IntType>
            static void Try_pick_int(const FIX::Message &message, IntType &target, int num);

        private:
            std::function<void(const fh::cme::exchange::OrderReport&)> m_processor;
            std::string m_target_comp_id;   // tag 56 in header
            std::string m_target_sub_id;   // tag 57 in header
            std::string m_sender_comp_id;   // tag 49 in header
            std::string m_sender_sub_id;   // tag 50 in header
            std::string m_sender_location_id;   // tag 142 in header
            std::string m_raw_data;   // tag 96 in Logon（tag 35-MsgType=A）
            std::string m_raw_data_length;   // tag 95 in Logon（tag 35-MsgType=A）
            std::string m_app_name;   // tag 1603 in Logon（tag 35-MsgType=A）
            std::string m_trading_ver;   // tag 1604 in Logon（tag 35-MsgType=A）
            std::string m_app_vendor;   // tag 1605 in Logon（tag 35-MsgType=A）
            std::string m_account;      // tag 1 for New Order (tag 35-MsgType=D)
            bool m_manual_flag;        // tag 1028 for New Order (tag 35-MsgType=D)
            std::string m_security_type;       // tag 167 for New Order (tag 35-MsgType=D)
            std::uint8_t m_customer_flag;     // tag 204 for New Order (tag 35-MsgType=D)
            std::string m_cti_code;         // tag 9702 for New Order (tag 35-MsgType=D)
            std::unordered_map<std::string, std::uint32_t> m_current_received_seq;

        private:
             DISALLOW_COPY_AND_ASSIGN(OrderManager);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif // __FH_CME_EXCHANGE_ORDER_MANAGER_H__
