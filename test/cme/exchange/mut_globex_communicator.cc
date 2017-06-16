
#include "gmock/gmock.h"

#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "cme/exchange/globex_communicator.h"
#include "cme/exchange/order.h"

#include "../../core/assist/mut_common.h"
#include "mut_globex_communicator.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    MutGlobexCommunicator::MutGlobexCommunicator()
    {
    }
    
    MutGlobexCommunicator::~MutGlobexCommunicator()
    {
        // noop
    }
    
    void MutGlobexCommunicator::SetUp()
    {
    }
    
    void MutGlobexCommunicator::TearDown()
    {
    }
    // logon logout
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test001)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            FIX::Message newMessage;
            newMessage.setString("8=FIX.4.29=17935=E49=FIXTEST56=TW128=SS134=252=20050225-16:54:3266=WMListOrID000000362394=368=173=111=SE102354=155=IBM67=163=021=381=060=20050225-16:54:3238=1000040=115=USD10=79");
            
            
            FIX::Message object;
            static const char * str =
                "54=1\00120=0\00131=109.03125\00160=00000000-00:00:00\001"
                "8=FIX.4.2\0016=109.03125\0011=acct1\001151=0\001150=2\001"
                "17=2\00139=2\00138=3000\00149=MEK\00115=USD\00137=1\001"
                "48=123ABC789\00114=3000\00135=8\00156=KEM\00134=2\001"
                "55=ABCD\00111=ID1\00122=1\001";

            // static const char * expected =
                // "8=FIX.4.2\0019=171\00135=8\00134=2\00149=MEK\00156=KEM\0011=acct1\001"
                // "6=109.03125\00111=ID1\00114=3000\00115=USD\00117=2\00120=0\00122=1\001"
                // "31=109.03125\00137=1\00138=3000\00139=2\00148=123ABC789\001"
                // "54=1\00155=ABCD\00160=00000000-00:00:00\001150=2\001151=0\00110=225\001";

            object.setString( str, false );
            

            //             
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test002)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            // logic
            // reponse ExecutionReport
            FIX::Message message = fh::core::assist::common::createExecutionReport( "CME", "2E0004N", 2 );
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),
                         FIX::SenderCompID( "CME" ), FIX::TargetCompID( "2E0004N" ) );
                         
            LOG_DEBUG("GlobexCommunicator_Test002: ", sessionID);
            
            FIX::Application *pApplication = nullptr;
            pApplication = new OrderManager(app_settings);
            if(pApplication!=nullptr)
            {
                fh::cme::exchange::OrderManager *pOrderManager = dynamic_cast<fh::cme::exchange::OrderManager *>(pApplication);
                pOrderManager->setCallback(std::bind(&fh::cme::exchange::GlobexCommunicator::Order_response, pGlobex, std::placeholders::_1));
                
                pApplication->fromApp(message, sessionID);
                
                delete pApplication;
                pApplication = nullptr;
            } 
         
                     
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test003)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            // logic
            // reponse TestRequest
            FIX::Message message = fh::core::assist::common::createTestRequest( "CME", "2E0004N", 2, "HELLO" );
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                         FIX::SenderCompID( "CME" ), FIX::TargetCompID( "2E0004N" ) );
                         
            LOG_DEBUG("=== sessionID = [ ", sessionID, "] ===");
            
            FIX::Application *pApplication = nullptr;
            pApplication = new OrderManager(app_settings);
            if(pApplication!=nullptr)
            {
                fh::cme::exchange::OrderManager *pOrderManager = dynamic_cast<fh::cme::exchange::OrderManager *>(pApplication);
                pOrderManager->setCallback(std::bind(&fh::cme::exchange::GlobexCommunicator::Order_response, pGlobex, std::placeholders::_1));
                
                pApplication->fromApp(message, sessionID);
                
                delete pApplication;
                pApplication = nullptr;
            } 
         
                     
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test004)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            // logic
            // reponse Heartbeat
            FIX::Message message = fh::core::assist::common::createHeartbeat( "CME", "2E0004N", 10);
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                         FIX::SenderCompID( "CME" ), FIX::TargetCompID( "2E0004N" ) );
                         
            LOG_DEBUG("=== sessionID = [ ", sessionID, "] ===");
            
            FIX::Application *pApplication = nullptr;
            pApplication = new OrderManager(app_settings);
            if(pApplication!=nullptr)
            {
                fh::cme::exchange::OrderManager *pOrderManager = dynamic_cast<fh::cme::exchange::OrderManager *>(pApplication);
                pOrderManager->setCallback(std::bind(&fh::cme::exchange::GlobexCommunicator::Order_response, pGlobex, std::placeholders::_1));
                
                pApplication->fromApp(message, sessionID);
                
                delete pApplication;
                pApplication = nullptr;
            } 
         
                     
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test005)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            // logic
            // reponse SequenceReset
            FIX::Message message = fh::core::assist::common::createSequenceReset( "CME", "2E0004N", 10, 20);
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                         FIX::SenderCompID( "CME" ), FIX::TargetCompID( "2E0004N" ) );
                         
            LOG_DEBUG("=== sessionID = [ ", sessionID, "] ===");
            
            FIX::Application *pApplication = nullptr;
            pApplication = new OrderManager(app_settings);
            if(pApplication!=nullptr)
            {
                fh::cme::exchange::OrderManager *pOrderManager = dynamic_cast<fh::cme::exchange::OrderManager *>(pApplication);
                pOrderManager->setCallback(std::bind(&fh::cme::exchange::GlobexCommunicator::Order_response, pGlobex, std::placeholders::_1));
                
                pApplication->fromApp(message, sessionID);
                
                delete pApplication;
                pApplication = nullptr;
            } 
         
                     
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test006)
    {       
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            // logic
            // reponse OrderCancelReject    
            FIX::Message message = fh::core::assist::common::createOrderCancelRequest42( "123", "456", '1', "789", "135", "246", "CME", "2E0004N", 10);
            FIX::SessionID sessionID( FIX::BeginString( "FIX.4.2" ),    
                         FIX::SenderCompID( "CME" ), FIX::TargetCompID( "2E0004N" ) );
                             
            LOG_DEBUG("=== sessionID = [ ", sessionID, "] ===");            
            
            FIX::Application *pApplication = nullptr;
            pApplication = new OrderManager(app_settings);
            if(pApplication!=nullptr)
            {
                fh::cme::exchange::OrderManager *pOrderManager = dynamic_cast<fh::cme::exchange::OrderManager *>(pApplication);
                pOrderManager->setCallback(std::bind(&fh::cme::exchange::GlobexCommunicator::Order_response, pGlobex, std::placeholders::_1));
                                
                
                pApplication->fromApp(message, sessionID);
                
                delete pApplication;
                pApplication = nullptr;
            } 
         
                     
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test007)
    {       
        FIX::Message message;
        message.getHeader().setField(FIX::BeginString("FIX.4.2"));
        message.getHeader().setField(FIX::SenderCompID("CME")); //"CME"
        message.getHeader().setField(FIX::TargetCompID("2E0004N")); // "2E0004N"
        message.getHeader().setField(FIX::MsgType(FIX::MsgType_NewOrderSingle));
        message.getHeader().setField(FIX::MsgSeqNum( 1 ));
        char time_in_force = '3';
        message.setField(FIX::FIELD::TimeInForce, std::string(1, time_in_force));        
        message.setField(FIX::FIELD::OrderQty, std::to_string(100));
        
        double minQty(1.000);
        message.setField(FIX::FIELD::MinQty, std::to_string(minQty));
        
        FIX::MinQty min_qty;
        message.getField(min_qty);
        int iMinQty = min_qty.getValue();
        LOG_DEBUG("***** iMinQty = [", iMinQty, "], min_qty.getValue() = [", min_qty.getValue(), "] *****");
        
        LOG_DEBUG("message(xml): ", message.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(message.toString()));
    }
    
    template <typename IntType>
    void Try_pick_int1(const FIX::Message &message, IntType &target, int num)
    {
        FIX::FieldBase fb(num, "");
        target = (message.getFieldIfSet(fb) && !fb.getString().empty()) ? boost::lexical_cast<IntType>(fb.getString()) : 0;
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test008)
    {       
        FIX::Message message;
        message.getHeader().setField(FIX::BeginString("FIX.4.2"));
        message.getHeader().setField(FIX::SenderCompID("CME")); //"CME"
        message.getHeader().setField(FIX::TargetCompID("2E0004N")); // "2E0004N"
        message.getHeader().setField(FIX::MsgType(FIX::MsgType_NewOrderSingle));
        message.getHeader().setField(FIX::MsgSeqNum( 1 ));
        char time_in_force = '3';
        message.setField(FIX::FIELD::TimeInForce, std::string(1, time_in_force));        
        message.setField(FIX::FIELD::OrderQty, std::to_string(100));
        
        int minQty = 1;
        message.setField(FIX::FIELD::MinQty, std::to_string(minQty));
                        
        LOG_DEBUG("message(xml): ", message.toXML());
        LOG_INFO("message(string): ", fh::core::assist::utility::Format_fix_message(message.toString()));
                
        Try_pick_int1(message, minQty, FIX::FIELD::MinQty);
        EXPECT_EQ(minQty, 1);
    }  
    
} // namespace exchange
} // namespace cme
} // namespace fh
