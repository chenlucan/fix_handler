
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


///////////////////////////////////////////////////////////
// test
#include <thread>   // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable

void thread_task(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "hello thread "
        << std::this_thread::get_id()
        << " paused " << n << " seconds" << std::endl;
}
//////////////////////////////////////////////////////////

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
            else if(FIX::MsgType_OrderCancelReject == m_msgType)
            {
                pSessionState->setNextTargetMsgSeqNum(3);
                LOG_DEBUG("===== [FIX::MsgType_OrderCancelReject== m_msgType] =====");
                // m_pSession->next( fh::core::assist::common::createReject( "CME", "2E0004N", 3, 2 ), FIX::UtcTimeStamp() );
                
                // [2017-05-17 08:32:59.259045] [7fb440d10700] [INFO ] [globex_logger.cc(45)          ] GLOBEX EVENT: Field not found
                // [2017-05-17 08:32:59.259162] [7fb440d10700] [INFO ] [globex_logger.cc(45)          ] GLOBEX EVENT: Disconnecting

                // [2017-05-17 16:36:14.740340] [7fb0abfff700] [INFO ] [globex_logger.cc(40)          ] SEND TO CME: 8=FIX.4.2 9=131 35=3 34=3382 49=2E0004N 50=qwc 52=20170517-08:36:14.740 56=CME 57=G 142=SG 369=3 45=3 58=SendingTime accuracy problem 372=9 373=10 10=015

                FIX42::OrderCancelReject object;

                //52=20170517-08:33:00.406\001
                object.setString
                ( "8=FIX.4.2\0019=102\00135=9\00134=3\00149=CME\00156=2E0004N\00152=20170517-08:33:00\00150=G\00111=CLIENTID\00137=ORDERID\001"
                  "39=1\00141=ORIGINALID\001434=2\00110=143\001" );                                                                                                                          
                //
                m_pSession->next(fh::core::assist::common::createOrderCancelReject( "CME", "2E0004N", 3), FIX::UtcTimeStamp() );
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
            // mass_order.mass_status_req_id = "S" + std::to_string(fh::core::assist::utility::Current_time_ns());   // ʹ�� "S + ��ǰʱ��" ��Ϊ ID
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
        //order. = strategy_order.account();        // �����������ļ���ȡ
        order.symbol = strategy_order.contract();
        order.security_desc = strategy_order.contract();      // ������ symbol ʹ��ͬһ��ֵ
        order.side = 0;
        order.price = std::stod(strategy_order.price());
        order.order_qty = strategy_order.quantity();
        order.time_in_force = 0;
        order.order_type = FIX::OrdType_LIMIT;
        order.order_id = strategy_order.exchange_order_id();
        //order. = strategy_order.status();         // ������ǻش����������
        //order. = strategy_order.working_price();        // ������ǻش����������
        //order. = strategy_order.working_quantity();         // ������ǻش����������
        //order. = strategy_order.filled_quantity();          // ������ǻش����������
        //order. = strategy_order.message();          // ������ǻش����������
        //order. = strategy_order.submit_time();          // ������ǻش����������
        //order.stop_px = ;                                  // �����ֻ�� OrdType = stop and stop-limit orders ʱ���ã�Ŀǰ����Ҫ
        //order.expire_date = ;                          // �����ֻ�� TimeInForce = Good Till Date (GTD) ʱ���ã�Ŀǰ����Ҫ
        order.orig_cl_order_id = order.cl_order_id;                 // ��������óɺ� cl_order_id һ��

        return order;
    }	
    
    fh::cme::exchange::MassOrder TestQuickFix::Create_mass_order(const char *data, size_t size)
    {
        // TODO Ŀǰ��û�� protobuf ���壬��ʱ��ʽ����Ϊ��
        // id��20 byte����CA ��Ϣ�ĳ����� cl_order_id��AF ��Ϣ�ĳ����� mass_status_req_id
        // req_type��1 byte����CA ��Ϣ�ĳ��ϲ�ʹ�ã�AF ��Ϣ�ĳ����� mass_status_req_type
        // name��20 byte����CA ��Ϣ�ĳ��� security_desc��AF ��Ϣ�ĳ��� security_desc ���� symbol
        fh::cme::exchange::MassOrder mass_order{};
        mass_order.cl_order_id =  std::string(data, 20);                              // ��CA��
        mass_order.mass_action_type = 3;      // ��CA��
        mass_order.mass_action_scope = 1;               // ��CA��     1: Instrument  9: Market Segment ID   10: Instrument Group
        mass_order.mass_status_req_id = mass_order.cl_order_id;                // ��AF��
        mass_order.mass_status_req_type = data[20] - '0';          // ��AF��         1: Instrument  3: Instrument Group  7: All Orders  100: Market Segment������Ӧ��
        mass_order.security_desc = size > 21 ? std::string(data + 21, size - 21) : "";                           // ��CA/AF��
        mass_order.symbol = mass_order.security_desc;  // ��AF��
        // ������Ŀ��������

        //LOG_DEBUG("MassOrder = ", mass_order);
        return mass_order;
    }

    // check the result of response
    void TestQuickFix::Order_response(const fh::cme::exchange::OrderReport& report)
    {
        LOG_DEBUG("===== TestQuickFix::Order_response =====");
        if(report.message_type == "")
        {
			LOG_DEBUG("===== 1111111111111111111  =====");
            // ��¼�ɹ���˵�����Կ�ʼ������ 
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
                else if(FIX::MsgType_OrderStatusRequest==m_appMsgType)
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
                    auto order_h = Create_order(order);
                    m_order_manager.sendOrderStatusRequest(order_h); 
                }
                else if(FIX::MsgType_OrderMassActionRequest==m_appMsgType)
                {  
                    std::string id = "X" + std::to_string(fh::core::assist::utility::Current_time_ns());
                    std::string order_type = "3";   // 1: Instrument  3: Instrument Group  7: All Orders
                    std::string name = "07";
                    std::string mass_order = id + order_type + name;
                    LOG_INFO("send mass order:  ", mass_order);
                                        
                    auto mass_order_ca = Create_mass_order(mass_order.data(), mass_order.size());
                    m_order_manager.sendOrderMassActionRequest(mass_order_ca);
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
            return;      // TODO Ŀǰ protobuf �л�û�ж���
        }
        else if(report.message_type == "8" && report.single_report.order_status == 'H')
        {
            LOG_DEBUG("===== [8-H]  =====");
            // Execution Report - Trade Cancel
            // ������÷��ͻ�ȥ
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
            // 9��Order Cancel Reject  8��Execution Report ��other��
            LOG_DEBUG("===== report.message_type = [", report.message_type, "]  =====");            
            sem_post(&sem);
        }
        
        //sem_post(&sem);
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
                //pTestInitiator->stop();  // ���ܵ���stop������responder�лظ�����Ӧ��Ϣ�޷�����
                
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
    
    //MsgType_OrderMassStatusRequest
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
    
    // MsgType_OrderCancelRequest
    TEST_F(MutOrderManager, OrderManager_Test005)
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
            FIX::MsgType appMsgType(FIX::MsgType_OrderCancelRequest);            
            pTestQuickFix->setAppMsgType(appMsgType);
            
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
                FIX::MsgType reqMsgType(FIX::MsgType_OrderCancelRequest);
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
    
    //MsgType_OrderCancelReplaceRequest
    TEST_F(MutOrderManager, OrderManager_Test006)
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
            FIX::MsgType appMsgType(FIX::MsgType_OrderCancelReplaceRequest);            
            pTestQuickFix->setAppMsgType(appMsgType);
            
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
                FIX::MsgType reqMsgType(FIX::MsgType_OrderCancelReplaceRequest);
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
    
    //MsgType_OrderStatusRequest
    TEST_F(MutOrderManager, OrderManager_Test007)
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
            FIX::MsgType appMsgType(FIX::MsgType_OrderStatusRequest);            
            pTestQuickFix->setAppMsgType(appMsgType);
            
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
                FIX::MsgType reqMsgType(FIX::MsgType_OrderStatusRequest);
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
    
    //MsgType_OrderMassActionRequest
    TEST_F(MutOrderManager, OrderManager_Test008)
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
            FIX::MsgType appMsgType(FIX::MsgType_OrderMassActionRequest);            
            pTestQuickFix->setAppMsgType(appMsgType);
            
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
                FIX::MsgType reqMsgType(FIX::MsgType_OrderMassActionRequest);
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
    
    #if 0
    //MsgType_
    TEST_F(MutOrderManager, OrderManager_Test009)
    {        
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        try
        {
            LOG_DEBUG("===== [1] start thread =====");            
            std::thread threads[5];
            std::cout << "Spawning 5 threads...\n";
            for (int i = 0; i < 5; i++) {
                threads[i] = std::thread(thread_task, i + 1);
            }
            std::cout << "Done spawning threads! Now wait for them to join\n";
            for (auto& t: threads) {
                t.join();
            }
            std::cout << "All threads joined.\n";                              
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    void do_print_id(int id)
    {
        std::unique_lock <std::mutex> lck(mtx);
        while (!ready) // �����־λ��Ϊ true, ��ȴ�...
            cv.wait(lck); // ��ǰ�̱߳�����, ��ȫ�ֱ�־λ��Ϊ true ֮��,
        // �̱߳�����, ��������ִ�д�ӡ�̱߳��id.
        std::cout << "thread " << id << '\n';
    }

    void go()
    {
        std::unique_lock <std::mutex> lck(mtx);
        ready = true; // ����ȫ�ֱ�־λΪ true.
        cv.notify_all(); // ���������߳�.
    }


    TEST_F(MutOrderManager, OrderManager_Test010)
    { 
        try
        {
            std::thread threads[10];
            // spawn 10 threads:
            for (int i = 0; i < 10; ++i)
                threads[i] = std::thread(do_print_id, i);

            std::cout << "10 threads ready to race...\n";
            go(); // go!

            for (auto & th:threads)
                th.join();
                        
        }
        catch ( std::exception& e)
        {
            LOG_DEBUG("catch exception: ", e.what());
        }
    }
    #endif
    
    
} // namespace exchange
} // namespace cme
} // name