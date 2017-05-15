
#include <gmock/gmock.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "cme/exchange/order_manager.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "core/assist/time_measurer.h"

#include "../../core/assist/mut_common.h"

#include "mut_order_manager.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    void fillHeader( FIX::Header& header, const char* sender, const char* target, int seq )
    {
      header.setField( FIX::SenderCompID( sender ) );
      header.setField( FIX::TargetCompID( target ) );
      header.setField( FIX::SendingTime() );
      header.setField( FIX::MsgSeqNum( seq ) );
    }
    
    FIX42::Logon createLogon( const char* sender, const char* target, int seq )
    {
        FIX42::Logon logon;
        logon.set( FIX::EncryptMethod( 0 ) );
        logon.set( FIX::HeartBtInt( 30 ) );
        fillHeader( logon.getHeader(), sender, target, seq );
        return logon;
    }
    
    // TestResponderCallback
    TestResponderCallback::TestResponderCallback()
    :m_pSession(0), m_msgType(FIX::MsgType_Logon)
    {
    }
    
    TestResponderCallback::~TestResponderCallback()
    {
    }
    
    bool TestResponderCallback::send( const std::string& )
    {
        LOG_DEBUG("===== TestResponderCallback::send ====="); 
                
        if(m_pSession)
        {
            LOG_DEBUG("================== m_msgType = [", m_msgType, "] =====================");
            
            if(FIX::MsgType_Logon == m_msgType)  // FixValues.h
            {
                m_pSession->next( fh::core::assist::common::createLogon( "CME", "2E0004N", 2 ), FIX::UtcTimeStamp() );
            }
            else if(FIX::MsgType_Logout == m_msgType)
            {
                m_pSession->next( fh::core::assist::common::createLogout( "CME", "2E0004N", 2 ), FIX::UtcTimeStamp() );
            }
            else if(FIX::MsgType_ExecutionReport == m_msgType)
            {                
                m_pSession->next( fh::core::assist::common::createLogon( "CME", "2E0004N", 2 ), FIX::UtcTimeStamp() );                
            }
            else
            {
                m_pSession->next( fh::core::assist::common::createSequenceReset( "CME", "2E0004N", 2, 5 ), FIX::UtcTimeStamp() );
            }
        }
        
        return true;
    }
    
    void TestResponderCallback::disconnect()
    {
        LOG_DEBUG("===== TestResponderCallback::disconnect =====");
    }
    
    // TestInitiator
    TestInitiator::TestInitiator( FIX::Application& application,
                                  FIX::MessageStoreFactory& factory,
                                  const FIX::SessionSettings& settings )
    throw( FIX::ConfigError )
    : Initiator( application, factory, settings ),
    m_application(&application), m_msgType(FIX::MsgType_Logon)
    {
    }

    TestInitiator::TestInitiator( FIX::Application& application,
                                  FIX::MessageStoreFactory& factory,
                                  const FIX::SessionSettings& settings,
                                  FIX::LogFactory& logFactory)
    throw( FIX::ConfigError )
    : Initiator( application, factory, settings, logFactory ),
    m_application(&application), m_msgType(FIX::MsgType_Logon)
    {
    }
    
    TestInitiator::~TestInitiator()
    {
    }
    
    void TestInitiator::onConfigure( const FIX::SessionSettings& s )
    throw ( FIX::ConfigError )
    {        
        LOG_DEBUG("===== TestInitiator::onConfigure =====");
    }

    void TestInitiator::onInitialize( const FIX::SessionSettings& s )
    throw ( FIX::RuntimeError )
    {
        LOG_DEBUG("===== TestInitiator::onInitialize =====");
    }
    
    void TestInitiator::onStart()
    {
        LOG_DEBUG("===== TestInitiator::onStart =====");
        // send message from exchange input file
        if(FIX::MsgType_Logon == m_msgType)  // FixValues.h
        {
            fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application ); 

            FIX42::Logout logout = fh::core::assist::common::createLogon( "2E0004N", "CME", 2 );
            std::string strLogOut = fh::core::assist::utility::Format_fix_message(logout.toString());
            //EXPECT_TRUE(orderManager->sendLogout(strLogOut));
        }
        else if(FIX::MsgType_Logout == m_msgType)
        {
            fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application ); 

            FIX42::Logout logout = fh::core::assist::common::createLogout( "2E0004N", "CME", 2 );
            std::string strLogOut = fh::core::assist::utility::Format_fix_message(logout.toString());
            EXPECT_TRUE(orderManager->sendLogout(strLogOut));
        }
        else if(FIX::MsgType_OrderMassStatusRequest == m_msgType)
        {
            fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application );
            
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
            FIX::Session *pSession = getSession(sessionID);
            
            // logon
            FIX42::Logon logon = fh::core::assist::common::createLogon( "2E0004N", "CME", 1 );
            std::string strLogOn = fh::core::assist::utility::Format_fix_message(logon.toString());
            EXPECT_TRUE(FIX::Session::sendToTarget(logon));
            // // mass_order_request
            // fh::cme::exchange::MassOrder mass_order;
            // mass_order.mass_status_req_id = "S" + std::to_string(fh::core::assist::utility::Current_time_ns());   // 使用 "S + 当前时间" 作为 ID
            // mass_order.mass_status_req_type = 7;    //  1: Instrument  3: Instrument Group  7: All Orders
            // EXPECT_TRUE(orderManager->sendOrderMassStatusRequest(mass_order));
                        

            if(pSession!=nullptr)
            {
                LOG_DEBUG("===== pSession->next [createNewOrderSingle] =====");      
                //pSession->next( fh::core::assist::common::createLogon( "2E0004N", "CME", 1 ), FIX::UtcTimeStamp() );
                
                FIX42::Message msg = fh::core::assist::common::createNewOrderSingle( "2E0004N", "CME", 2 );
                // const FIX::Header& header = msg.getHeader();

                // FIX::MsgType* pMsgType = 0;
                // pMsgType = FIX::getField( header, FIX::MsgType );
                //pSession->next( msg, FIX::UtcTimeStamp() );
                
            }
        }
        else
        {
            fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application ); 

            FIX42::Logout logout = fh::core::assist::common::createLogout( "2E0004N", "CME", 2 );
            std::string strLogOut = fh::core::assist::utility::Format_fix_message(logout.toString());
            EXPECT_TRUE(orderManager->sendLogout(strLogOut));
        }
    }
    
    bool TestInitiator::onPoll( double timeout )
    {
        LOG_DEBUG("===== TestInitiator::onPoll =====");
        return true;
    }
    
    void TestInitiator::onStop()
    {
        LOG_DEBUG("===== TestInitiator::onStop =====");
    }

    void TestInitiator::doConnect( const FIX::SessionID& s, const FIX::Dictionary& d )
    {
        LOG_DEBUG("===== TestInitiator::doConnect =====");        
        try
        {
        }
        catch ( std::exception& ) {}
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    // TestQuickFix
    TestQuickFix::TestQuickFix(const std::string &app_file, const std::string &config_file)
    :m_app_settings(app_file), m_order_manager(m_app_settings), m_settings(config_file), m_store(m_settings), m_logger(m_settings)    
    {
        m_order_manager.setCallback(std::bind(&TestQuickFix::Order_response, this, std::placeholders::_1));
    }
    
    TestQuickFix::~TestQuickFix()
    {
        // noop
    }
    
    // check the result of response
    void TestQuickFix::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        LOG_DEBUG("===== TestQuickFix::Order_response =====");
        if(report.message_type == "")
        {
            // 登录成功，说明可以开始交易了            
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
            //fill.set_buy_sell(GlobexCommunicator::Convert_buy_sell(report.single_report.side));
            fh::core::assist::utility::To_pb_time(fill.mutable_fill_time(), report.single_report.transact_time);

            //m_strategy->OnFill(fill);
        }
        else
        {
            // 9：Order Cancel Reject  8：Execution Report （other）
            // pb::ems::Order order;
            // order.set_client_order_id(report.single_report.cl_order_id);
            // order.set_account(report.single_report.account);
            // order.set_contract(report.single_report.security_desc);
            // order.set_buy_sell(GlobexCommunicator::Convert_buy_sell(report.single_report.side));
            // order.set_price(std::to_string(report.single_report.price));
            // order.set_quantity(report.single_report.order_qty);
            // order.set_tif(GlobexCommunicator::Convert_tif(report.single_report.time_in_force));
            // order.set_order_type(GlobexCommunicator::Convert_order_type(report.single_report.order_type));
            // order.set_exchange_order_id(report.single_report.order_id);
            // order.set_status(GlobexCommunicator::Convert_order_status(report.single_report.order_status));
            // order.set_working_price(std::to_string(report.single_report.price));
            // order.set_working_quantity(report.single_report.leaves_qty);
            // order.set_filled_quantity(report.single_report.cum_qty);
            // order.set_message(report.single_report.text);
            // fh::core::assist::utility::To_pb_time(order.mutable_submit_time(), report.single_report.transact_time);

            //m_strategy->OnOrder(order);
        }
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    // MutOrderManager
    MutOrderManager::MutOrderManager()
    {
    }
    
    MutOrderManager::~MutOrderManager()
    {
        // noop
    }
    
    void MutOrderManager::SetUp()
    {
    }
    
    void MutOrderManager::TearDown()
    {
    }
    #if 0    
    // logout
    TEST_F(MutOrderManager, OrderManager_Test001)
    {        
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        try
        {
            fh::cme::exchange::TestQuickFix *pTestQuickFix = new fh::cme::exchange::TestQuickFix(app_setting_file, fix_setting_file);
            if(nullptr == pTestQuickFix)
            {
                return;
            }
            
            TestResponderCallback *pTestResponderCallback = new TestResponderCallback();
            if(nullptr == pTestResponderCallback)
            {
                delete pTestQuickFix;
                pTestQuickFix = nullptr;
                return;
            }
            FIX::MsgType msgType(FIX::MsgType_Logout); 
            pTestResponderCallback->setMsgType(msgType);

            fh::cme::exchange::TestInitiator *pTestInitiator = new TestInitiator(
                                                                                pTestQuickFix->m_order_manager, 
                                                                                pTestQuickFix->m_store, 
                                                                                pTestQuickFix->m_settings, 
                                                                                pTestQuickFix->m_logger);
            if(pTestInitiator!=nullptr)
            {
                pTestInitiator->setMsgType(msgType); 
                FIX::Initiator * pInitiator = static_cast < FIX::Initiator* > ( pTestInitiator );  
                
                FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
                FIX::Session *pSession = pInitiator->getSession(sessionID, *pTestResponderCallback);

                //pSession->setResponder(pTestResponderCallback);       
                
                pTestResponderCallback->setSession(pSession);  
                
                
                pTestInitiator->start(); 
                
                LOG_DEBUG("==============pSession ", pSession, "=====================");
                
                if(pSession!=nullptr)
                {                    
                    // bool ret = pSession->receivedLogon();
                    // EXPECT_FALSE(ret);
                    
                    // pSession->next( createLogon( "2E0004N", "CME", 1 ), FIX::UtcTimeStamp() );
                    
                    // ret = pSession->receivedLogon();
                    // EXPECT_TRUE(ret);
                }
                
                
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                //pTestInitiator->stop();  // 不能调用stop，否则responder中回复的响应消息无法处理
                
                delete pTestInitiator;
                pTestInitiator = nullptr;
            }
            
            delete pTestResponderCallback;
            pTestResponderCallback = nullptr;

            delete pTestQuickFix;
            pTestQuickFix = nullptr;
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    
    // log
    TEST_F(MutOrderManager, OrderManager_Test002)
    {        
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        try
        {
            fh::cme::exchange::TestQuickFix *pTestQuickFix = new fh::cme::exchange::TestQuickFix(app_setting_file, fix_setting_file);
            if(nullptr == pTestQuickFix)
            {
                return;
            }
            
            TestResponderCallback *pTestResponderCallback = new TestResponderCallback();
            if(nullptr == pTestResponderCallback)
            {
                delete pTestQuickFix;
                pTestQuickFix = nullptr;
                return;
            }
            FIX::MsgType msgType("7"); // FIX::MsgType_Logout
            pTestResponderCallback->setMsgType(msgType);

            fh::cme::exchange::TestInitiator *pTestInitiator = new TestInitiator(
                                                                                pTestQuickFix->m_order_manager, 
                                                                                pTestQuickFix->m_store, 
                                                                                pTestQuickFix->m_settings, 
                                                                                pTestQuickFix->m_logger);
            if(pTestInitiator!=nullptr)
            {
                pTestInitiator->setMsgType(msgType); 
                FIX::Initiator * pInitiator = static_cast < FIX::Initiator* > ( pTestInitiator );  
                
                FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
                FIX::Session *pSession = pInitiator->getSession(sessionID, *pTestResponderCallback);

                //pSession->setResponder(pTestResponderCallback);       
                
                pTestResponderCallback->setSession(pSession);  
                
                
                pTestInitiator->start(); 
                
                LOG_DEBUG("==============pSession ", pSession, "=====================");
                
                
                
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                //pTestInitiator->stop();
                
                delete pTestInitiator;
                pTestInitiator = nullptr;                
            }
            
            delete pTestResponderCallback;
            pTestResponderCallback = nullptr;

            delete pTestQuickFix;
            pTestQuickFix = nullptr;
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    #endif
    // MsgType_OrderMassStatusRequest
    TEST_F(MutOrderManager, OrderManager_Test003)
    {        
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        try
        {
            fh::cme::exchange::TestQuickFix *pTestQuickFix = new fh::cme::exchange::TestQuickFix(app_setting_file, fix_setting_file);
            if(nullptr == pTestQuickFix)
            {
                return;
            }
            
            TestResponderCallback *pTestResponderCallback = new TestResponderCallback();
            if(nullptr == pTestResponderCallback)
            {
                delete pTestQuickFix;
                pTestQuickFix = nullptr;
                return;
            }
            FIX::MsgType responseMsgType(FIX::MsgType_ExecutionReport); 
            pTestResponderCallback->setMsgType(responseMsgType);

            fh::cme::exchange::TestInitiator *pTestInitiator = new TestInitiator(
                                                                                pTestQuickFix->m_order_manager, 
                                                                                pTestQuickFix->m_store, 
                                                                                pTestQuickFix->m_settings, 
                                                                                pTestQuickFix->m_logger);
            if(pTestInitiator!=nullptr)
            {
                FIX::MsgType reqMsgType(FIX::MsgType_OrderMassStatusRequest);
                pTestInitiator->setMsgType(reqMsgType);
                FIX::Initiator * pInitiator = static_cast < FIX::Initiator* > ( pTestInitiator );  
                
                FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
                FIX::Session *pSession = pInitiator->getSession(sessionID, *pTestResponderCallback);

                //pSession->setResponder(pTestResponderCallback);       
                
                pTestResponderCallback->setSession(pSession);  
                
                
                pTestInitiator->start(); 
                
                LOG_DEBUG("==============pSession ", pSession, "=====================");
                
                
                std::this_thread::sleep_for(std::chrono::milliseconds(9000));
                //pTestInitiator->stop();
                
                delete pTestInitiator;
                pTestInitiator = nullptr;                
            }
            
            delete pTestResponderCallback;
            pTestResponderCallback = nullptr;

            delete pTestQuickFix;
            pTestQuickFix = nullptr;
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    
} // namespace exchange
} // namespace cme
} // name