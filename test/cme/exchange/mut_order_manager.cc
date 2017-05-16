
#include <gmock/gmock.h>

#include <gtest/gtest.h>


#include <semaphore.h>

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
    sem_t sem;
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
    :m_pSession(0), m_msgType(FIX::MsgType_Logon),
    m_nextMsgType(FIX::MsgType_Logon)
    {
    }
    
    TestResponderCallback::~TestResponderCallback()
    {
    }
    
    bool TestResponderCallback::send( const std::string& )
    {
        LOG_DEBUG("===== TestResponderCallback::send ====="); 
        
        FIX::SessionState* pSessionState = static_cast < FIX::SessionState*> (m_pSession->getLog());
        pSessionState->setNextTargetMsgSeqNum(1);
        
        if(m_pSession)
        {
            LOG_DEBUG("================== m_msgType = [", m_msgType, "] =====================");
            
            if(FIX::MsgType_Logon == m_msgType)  // FixValues.h
            {
                LOG_DEBUG("===== [FIX::MsgType_Logon == m_msgType] =====");
                
                FIX::MsgType tmpMsgType(m_nextMsgType);
                m_msgType = tmpMsgType;
                LOG_DEBUG("===== after m_msgType = ", m_msgType, "=====");
		        

				pSessionState->initiate(true);
				pSessionState->sentLogon(true);
				pSessionState->receivedReset(true);                
                m_pSession->next( fh::core::assist::common::createLogon( "CME", "2E0004N", 1 ), FIX::UtcTimeStamp() );    
            }
            else if(FIX::MsgType_Logout == m_msgType)
            {
                m_pSession->next( fh::core::assist::common::createLogout( "CME", "2E0004N", 1 ), FIX::UtcTimeStamp() );
            }
            else if(FIX::MsgType_ExecutionReport == m_msgType)
            { 
                LOG_DEBUG("===== [FIX::MsgType_ExecutionReport == m_msgType] =====");                
                m_pSession->next( fh::core::assist::common::createExecutionReport( "CME", "2E0004N", 1 ), FIX::UtcTimeStamp() );
            }
            else if(FIX::MsgType_OrderCancelReject== m_msgType)
            {
                pSessionState->setNextTargetMsgSeqNum(3);
                LOG_DEBUG("===== [FIX::MsgType_OrderCancelReject== m_msgType] =====");
                m_pSession->next( fh::core::assist::common::createReject( "CME", "2E0004N", 3, 2 ), FIX::UtcTimeStamp() );
                                
                LOG_DEBUG("===== pSessionState->getNextTargetMsgSeqNum() = [", pSessionState->getNextTargetMsgSeqNum(), "] =====");
                
                sem_post(&sem);
            }
            else
            {
                //m_pSession->next( fh::core::assist::common::createSequenceReset( "CME", "2E0004N", 2, 5 ), FIX::UtcTimeStamp() );
            }
        }
        
        return true;
    }
    
    void TestResponderCallback::disconnect()
    {
        LOG_DEBUG("===== TestResponderCallback::disconnect =====");
        LOG_DEBUG("===== [1] before sem_post =====");
        sem_post(&sem);
        LOG_DEBUG("===== [2] after sem_post =====");
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
            // fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application ); 

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
            // fh::cme::exchange::OrderManager *orderManager = dynamic_cast < fh::cme::exchange::OrderManager* > ( m_application );
            
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
            FIX::Session *pSession= getSession(sessionID);
            
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
        else if(FIX::MsgType_NewOrderSingle == m_msgType)
        {
            // logon
            FIX42::Logon logon = fh::core::assist::common::createLogon( "2E0004N", "CME", 1 );
            std::string strLogOn = fh::core::assist::utility::Format_fix_message(logon.toString());
            EXPECT_TRUE(FIX::Session::sendToTarget(logon));
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
    :m_app_settings(app_file), m_order_manager(m_app_settings), m_settings(config_file), m_store(m_settings), m_logger(m_settings),
    m_isContinueSend(false), m_appMsgType(FIX::MsgType_NewOrderSingle)   
    {
        m_order_manager.setCallback(std::bind(&TestQuickFix::Order_response, this, std::placeholders::_1));
    }
    
    TestQuickFix::~TestQuickFix()
    {
        // noop
    }
    
    void TestQuickFix::setContinueSendFlag(bool isConSendFlag)
    {
        m_isContinueSend = isConSendFlag;
    }   

    bool TestQuickFix::getContinueSendFlag()
    {
        return m_isContinueSend;
    }     
    
    	
    void TestQuickFix::setAppMsgType(const FIX::MsgType appMsgType)
    {
        m_appMsgType = appMsgType;
    }
	
    fh::cme::exchange::Order TestQuickFix::Create_order(const ::pb::ems::Order& strategy_order)
    {
        LOG_INFO("received order:  ", fh::core::assist::utility::Format_pb_message(strategy_order));

        fh::cme::exchange::Order order;
        order.cl_order_id = strategy_order.client_order_id();
        //order. = strategy_order.account();        // 这个域从配置文件读取
        order.symbol = strategy_order.contract();
        order.security_desc = strategy_order.contract();      // 这个域和 symbol 使用同一个值
        order.side = 0;
        order.price = std::stod(strategy_order.price());
        order.order_qty = strategy_order.quantity();
        order.time_in_force = 0;
        order.order_type = 0;
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

    // check the result of response
    void TestQuickFix::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        LOG_DEBUG("===== TestQuickFix::Order_response =====");
        if(report.message_type == "")
        {
			LOG_DEBUG("===== 1111111111111111111  =====");
            // 登录成功，说明可以开始交易了 
            if(getContinueSendFlag())   
            {
                if(FIX::MsgType_NewOrderSingle == m_appMsgType)
                {
                    pb::ems::Order order;
                    order.set_client_order_id("X" + std::to_string(fh::core::assist::utility::Current_time_ns()));
                    order.set_account("YYC");   // unuse
                    order.set_contract("1EHV8");
                    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                    order.set_price(std::to_string(398));
                    order.set_quantity(0+20);
                    order.set_tif(pb::ems::TimeInForce::TIF_GFD);
                    order.set_order_type(pb::ems::OrderType::OT_Limit);
                    order.set_exchange_order_id("9923898474");
                    order.set_status(pb::ems::OrderStatus::OS_Pending);
                    order.set_working_price("398");
                    order.set_working_quantity(0 / 2+ 10);
                    order.set_filled_quantity(0 - 0 / 2 + 10);
                    order.set_message("cme test order");
                    auto order_d = Create_order(order);
                    m_order_manager.sendNewOrderSingleRequest(order_d);                    
                }
                else if(FIX::MsgType_OrderCancelRequest == m_appMsgType)
                {
                    pb::ems::Order order;
                    order.set_client_order_id("X" + std::to_string(fh::core::assist::utility::Current_time_ns()));
                    order.set_account("YYC");   // unuse
                    order.set_contract("1EHV8");
                    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                    order.set_price(std::to_string(398));
                    order.set_quantity(0+20);
                    order.set_tif(pb::ems::TimeInForce::TIF_GFD);
                    order.set_order_type(pb::ems::OrderType::OT_Limit);
                    order.set_exchange_order_id("9923898474");
                    order.set_status(pb::ems::OrderStatus::OS_Pending);
                    order.set_working_price("398");
                    order.set_working_quantity(0 / 2+ 10);
                    order.set_filled_quantity(0 - 0 / 2 + 10);
                    order.set_message("cme test order");
                    auto order_f = Create_order(order);
                    m_order_manager.sendOrderCancelRequest(order_f); 
                }
                else if(FIX::MsgType_OrderCancelReplaceRequest== m_appMsgType)
                {
                    pb::ems::Order order;
                    order.set_client_order_id("X" + std::to_string(fh::core::assist::utility::Current_time_ns()));
                    order.set_account("YYC");   // unuse
                    order.set_contract("1EHV8");
                    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                    order.set_price(std::to_string(398));
                    order.set_quantity(0+20);
                    order.set_tif(pb::ems::TimeInForce::TIF_GFD);
                    order.set_order_type(pb::ems::OrderType::OT_Limit);
                    order.set_exchange_order_id("9923898474");
                    order.set_status(pb::ems::OrderStatus::OS_Pending);
                    order.set_working_price("398");
                    order.set_working_quantity(0 / 2+ 10);
                    order.set_filled_quantity(0 - 0 / 2 + 10);
                    order.set_message("cme test order");
                    auto order_f = Create_order(order);
                    m_order_manager.sendOrderCancelReplaceRequest(order_f); 
                }
                else
                {
                }
                
                setContinueSendFlag(false); 
            }      
        }
        else if(report.message_type == "BZ")
        {
            LOG_DEBUG("===== [BZ]  =====");
            // Order Mass Action Report
            sem_post(&sem);
            return;      // TODO 目前 protobuf 中还没有定义
        }
        else if(report.message_type == "8" && report.single_report.order_status == 'H')
        {
            LOG_DEBUG("===== [8-H]  =====");
            // Execution Report - Trade Cancel
            // 这个不用发送回去
            sem_post(&sem);
            return;
        }
        else if(report.message_type == "8" && (report.single_report.order_status == '1' || report.single_report.order_status == '2'))
        {
            LOG_DEBUG("===== [8-1-2]  =====");

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
            
            sem_post(&sem);
            //m_strategy->OnFill(fill);
        }
        else
        {
            // 9：Order Cancel Reject  8：Execution Report （other）
            LOG_DEBUG("===== report.message_type = [", report.message_type, "]  =====");
            
            sem_post(&sem);
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
            FIX::MsgType nextMsgType(FIX::MsgType_Logout);             
            pTestResponderCallback->setMsgType(msgType, nextMsgType);

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
            FIX::MsgType nextMsgType(FIX::MsgType_Logout);     
            pTestResponderCallback->setMsgType(msgType, nextMsgType);

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
                
                
                
                //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
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
            sem_init(&sem,0,0);
            
            fh::cme::exchange::TestQuickFix *pTestQuickFix = new fh::cme::exchange::TestQuickFix(app_setting_file, fix_setting_file);
            if(nullptr == pTestQuickFix)
            {
                return;
            }
            
            pTestQuickFix->setContinueSendFlag(true);  
            
            TestResponderCallback *pTestResponderCallback = new TestResponderCallback();
            if(nullptr == pTestResponderCallback)
            {
                delete pTestQuickFix;
                pTestQuickFix = nullptr;
                return;
            }
            FIX::MsgType responseMsgType(FIX::MsgType_Logon); 
            FIX::MsgType nextRespMsgType(FIX::MsgType_ExecutionReport); 
            pTestResponderCallback->setMsgType(responseMsgType, nextRespMsgType);

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
                
                
                //std::this_thread::sleep_for(std::chrono::milliseconds(9000));
                
                LOG_DEBUG("===== [1] before sem_wait =====");
                sem_wait(&sem);
                LOG_DEBUG("===== [2] after sem_wait =====");
                pTestInitiator->stop();
                
                delete pTestInitiator;
                pTestInitiator = nullptr;                
            }
            
            delete pTestResponderCallback;
            pTestResponderCallback = nullptr;

            delete pTestQuickFix;
            pTestQuickFix = nullptr;
            
            sem_destroy(&sem);
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    
    //MsgType_NewOrderSingle
    TEST_F(MutOrderManager, OrderManager_Test004)
    {        
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        try
        {
            sem_init(&sem,0,0);
            
            fh::cme::exchange::TestQuickFix *pTestQuickFix = new fh::cme::exchange::TestQuickFix(app_setting_file, fix_setting_file);
            if(nullptr == pTestQuickFix)
            {
                return;
            }
            
            pTestQuickFix->setContinueSendFlag(true);  
            
            TestResponderCallback *pTestResponderCallback = new TestResponderCallback();
            if(nullptr == pTestResponderCallback)
            {
                delete pTestQuickFix;
                pTestQuickFix = nullptr;
                return;
            }
            FIX::MsgType responseMsgType(FIX::MsgType_Logon); 
            FIX::MsgType nextRespMsgType(FIX::MsgType_OrderCancelReject); 
            pTestResponderCallback->setMsgType(responseMsgType, nextRespMsgType);

            fh::cme::exchange::TestInitiator *pTestInitiator = new TestInitiator(
                                                                                pTestQuickFix->m_order_manager, 
                                                                                pTestQuickFix->m_store, 
                                                                                pTestQuickFix->m_settings, 
                                                                                pTestQuickFix->m_logger);
            if(pTestInitiator!=nullptr)
            {
                FIX::MsgType reqMsgType(FIX::MsgType_NewOrderSingle);
                pTestInitiator->setMsgType(reqMsgType);
                FIX::Initiator * pInitiator = static_cast < FIX::Initiator* > ( pTestInitiator );  
                
                FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                 FIX::SenderCompID( "2E0004N" ), FIX::TargetCompID( "CME" ) ); 
                FIX::Session *pSession = pInitiator->getSession(sessionID, *pTestResponderCallback);

                //pSession->setResponder(pTestResponderCallback);       
                
                pTestResponderCallback->setSession(pSession);  
                
                
                pTestInitiator->start(); 
                
                LOG_DEBUG("==============pSession ", pSession, "=====================");
                
                
                //std::this_thread::sleep_for(std::chrono::milliseconds(9000));
                
                LOG_DEBUG("===== [1] before sem_wait =====");
                sem_wait(&sem);
                LOG_DEBUG("===== [2] after sem_wait =====");
                pTestInitiator->stop();
                
                delete pTestInitiator;
                pTestInitiator = nullptr;                
            }
            
            delete pTestResponderCallback;
            pTestResponderCallback = nullptr;

            delete pTestQuickFix;
            pTestQuickFix = nullptr;
            
            sem_destroy(&sem);
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    
} // namespace exchange
} // namespace cme
} // name