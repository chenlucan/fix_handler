#ifndef __FH_CME_EXCHANGE_MUT_ORDER_MANAGER_H__
#define __FH_CME_EXCHANGE_MUT_ORDER_MANAGER_H__

#include <gtest/gtest.h>

#include <unordered_map>

#include <quickfix/Responder.h>
#include <quickfix/FileStore.h>
#include <quickfix/Dictionary.h>
#include <quickfix/Exceptions.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Initiator.h>
#include <quickfix/SessionState.h>

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
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/order.h"
#include "cme/exchange/globex_logger.h"





namespace fh
{
namespace cme
{
namespace exchange
{    
    class TestResponderCallback : public FIX::Responder
    {
    public:
        TestResponderCallback();
        virtual ~TestResponderCallback();
        
        void setSession( FIX::Session* pS )
        {
           m_pSession = pS;
        }
    
        void setMsgType( const FIX::MsgType &msgType, const FIX::MsgType &nextMsgType)
        {
           m_msgType = msgType;
           m_nextMsgType = nextMsgType;
        }

    private:
        /// TestResponderCallback implementation of Responder.
        bool send( const std::string& );
        void disconnect();
        

        FIX::Session *m_pSession;
        FIX::MsgType m_msgType;
        FIX::MsgType m_nextMsgType;
    };
    
    class TestInitiator : public FIX::Initiator
    {        
    public:
        //TestInitiator(Application&);        
        TestInitiator( FIX::Application&, FIX::MessageStoreFactory&,
             const FIX::SessionSettings& ) throw( FIX::ConfigError );
        TestInitiator( FIX::Application&, FIX::MessageStoreFactory&,
             const FIX::SessionSettings&, FIX::LogFactory& ) throw( FIX::ConfigError );
        
        virtual ~TestInitiator();
        
        void setMsgType( const FIX::MsgType &msgType )
        {
           m_msgType = msgType;
        }
        
    private:
        /// TestInitiator implementation of Initiator.
        void onConfigure( const FIX::SessionSettings& ) throw ( FIX::ConfigError );
        void onInitialize( const FIX::SessionSettings& ) throw ( FIX::RuntimeError );

        void onStart();
        bool onPoll( double timeout );
        void onStop();

        void doConnect( const FIX::SessionID&, const FIX::Dictionary& d );    

        FIX::Application *m_application;     
        FIX::MsgType m_msgType;    
        
    };
    
    class TestQuickFix
    {
    public:
        TestQuickFix( 
        const std::string &app_file,
        const std::string &config_file );
        virtual ~TestQuickFix();
        
        void Order_response(const fh::cme::exchange::OrderReport& report);
		
		std::string make_order();
		fh::cme::exchange::Order Create_order(const ::pb::ems::Order& strategy_order);
        fh::cme::exchange::MassOrder Create_mass_order(const char *data, size_t size);
        
        void setContinueSendFlag(bool isConSendFlag);
        bool getContinueSendFlag();
        
        void setAppMsgType(const FIX::MsgType appMsgType);
        
    public:
        fh::cme::exchange::ExchangeSettings m_app_settings;
        fh::cme::exchange::OrderManager m_order_manager;
        FIX::SessionSettings m_settings;
        FIX::FileStoreFactory m_store;
        fh::cme::exchange::GlobexLogFactory m_logger;
        bool m_isContinueSend;
        FIX::MsgType m_appMsgType;
        
    };
    
    class MutOrderManager : public testing::Test
    {
    public:
        explicit MutOrderManager();
        virtual ~MutOrderManager(); 
        
        virtual void SetUp();
        virtual void TearDown();            

    private:
         DISALLOW_COPY_AND_ASSIGN(MutOrderManager);
    };
    
    
    
    
} // namespace exchange
} // namespace cme
} // namespace fh

#endif // __FH_CME_EXCHANGE_MUT_ORDER_MANAGER_H__
