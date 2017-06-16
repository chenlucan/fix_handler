
#include "gmock/gmock.h"

#include "core/assist/logger.h"
#include "core/assist/time_measurer.h"
#include "cme/exchange/order.h"
#include "core/strategy/invalid_order.h"
#include "core/strategy/strategy_communicator.h"

#include "../../core/assist/mut_common.h"
#include "mut_strategy_communicator.h"

namespace fh
{
namespace core
{
namespace strategy
{    
    MutStrategyCommunicator::MutStrategyCommunicator()
    {
    }
    
    MutStrategyCommunicator::~MutStrategyCommunicator()
    {
        // noop
    }
    
    void MutStrategyCommunicator::SetUp()
    {
    }
    
    void MutStrategyCommunicator::TearDown()
    {
    }
    #if 0
    // Missing field: cl_order_id
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test001)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    // Missing field: contract
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test002)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    // Missing field: side
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test003)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Invalid side
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test004)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_None);
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Missing field: price
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test005)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: invalid price
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test006)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price("assf398");
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: invalid price
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test007)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price(".398");
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Missing field: quantity
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test008)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price("100.398");
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Invalid OrderQty
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test009)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price("100.398");
                strategy_order.set_quantity(0);
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Missing field: order_type
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test010)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price("100.398");
                strategy_order.set_quantity(100);
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // invalid order: Invalid OrdType
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test011)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        try
        {
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);    
            app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
            
            fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
            std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
            strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
            
            if(strategy!=nullptr)
            {                
                strategy_order.set_client_order_id("client_order_id");
                strategy_order.set_account("YYC");   // unuse
                strategy_order.set_contract("1EHV8");
                strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
                strategy_order.set_price("100.398");
                strategy_order.set_quantity(100);
                strategy_order.set_order_type(pb::ems::OrderType::OT_None);
                
                strategy->Check_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            if(strategy!=nullptr)
            {                
                strategy->Reject_order(strategy_order);
                
                delete strategy;
                strategy = nullptr;
            }
        }
    }
    
    // valid order
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test012)
    {
        ::pb::ems::Order strategy_order;
        fh::core::strategy::StrategyCommunicator *strategy = nullptr;
        
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);    
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        strategy = new fh::core::strategy::StrategyCommunicator(url.first, url.second);
        
        if(strategy!=nullptr)
        {                
            strategy_order.set_client_order_id("client_order_id");
            strategy_order.set_account("YYC");   // unuse
            strategy_order.set_contract("1EHV8");
            strategy_order.set_buy_sell(pb::ems::BuySell::BS_Buy);
            strategy_order.set_price("100.398");
            strategy_order.set_quantity(100);
            strategy_order.set_order_type(pb::ems::OrderType::OT_Limit);
            
            strategy->Check_order(strategy_order);
            
            delete strategy;
            strategy = nullptr;
        }        
    }
    
    // check price
    TEST_F(MutStrategyCommunicator, StrategyCommunicator_Test013)
    {
        std::string price = "as.398";        
        bool isSuccess = fh::core::assist::utility::Is_price_valid(price);   
        EXPECT_FALSE(isSuccess);         
        
        price = ".398";        
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_FALSE(isSuccess);
        
        price = "1.q===98";        
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_FALSE(isSuccess);
        
        price = "11222.";        
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_FALSE(isSuccess);
        
        price = "11222p";        
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_FALSE(isSuccess);
        
        price = "112.22.10";        
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_FALSE(isSuccess);
        
        price = "112.....2200";
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
         EXPECT_FALSE(isSuccess);
        
        price = "adbbduti*";
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
         EXPECT_FALSE(isSuccess);
        
        price = "1122200";
        isSuccess = fh::core::assist::utility::Is_price_valid(price); 
        EXPECT_TRUE(isSuccess);               
        
        price = "11222.1";
        isSuccess = fh::core::assist::utility::Is_price_valid(price);       
        EXPECT_TRUE(isSuccess);
    }
    #endif
    
} // namespace strategy
} // namespace core
} // namespace fh
