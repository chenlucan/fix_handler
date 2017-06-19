
#include "core/assist/logger.h"
#include "gmock/gmock.h"
#include "mut_rem_exchange.h"

namespace fh
{
namespace rem
{
namespace exchange
{

CMutRemExchange::CMutRemExchange()
{
        // noop
        
}      


CMutRemExchange::~CMutRemExchange()
{
        // noop
}

TEST_F(CMutRemExchange, CMutRemExchange_Test001)
{    	
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7002","tcp://localhost:7001");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    EXPECT_EQ(10,m_pCRemGlobexCommunicator->m_itimeout);	
	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutRemExchange, CMutRemExchange_Test002)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:6002","tcp://localhost:6001");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    EXPECT_EQ(0,m_pCRemGlobexCommunicator->m_ReqId);	
	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutRemExchange, CMutRemExchange_Test003)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7102","tcp://localhost:7101");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
	
    bool b_ret=true;	
    if(NULL == pFileConfig || NULL == m_pCRemGlobexCommunicator)
    {
        b_ret = false;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;
    delete pFileConfig;	
}

TEST_F(CMutRemExchange, CMutRemExchange_Test004)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:6102","tcp://localhost:6101");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);

    pb ::ems::Order order;
	
    order.set_client_order_id("wwww");
    order.set_account("6660357901");	
    order.set_contract("ag1707");	 
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_price(std::to_string(1930));
    order.set_quantity(4);	 
    order.set_tif(pb::ems::TimeInForce::TIF_GFD);
    order.set_order_type(pb::ems::OrderType::OT_Limit);	

    m_pCRemGlobexCommunicator->Add(order);

    EES_OrderAcceptField tmpAccept;
    memset(&tmpAccept,0,sizeof(EES_OrderAcceptField));
    tmpAccept.m_ClientOrderToken=m_pCRemGlobexCommunicator->m_pEESTraderApiManger->MaxOrderLocalID-1;		
    tmpAccept.m_MarketOrderToken = 90714;
    tmpAccept.m_UserID = 6660357901;	
    strcpy(tmpAccept.m_Symbol,"ag1707");	
    tmpAccept.m_Side = EES_SideType_open_long;
    tmpAccept.m_Price = 1930.0;
    tmpAccept.m_Qty = 4;
    tmpAccept.m_OrderState = EES_OrderState_order_live;
	
    m_pCRemGlobexCommunicator->m_pEESTraderApiManger->OnOrderAccept(&tmpAccept);	
    bool b_ret=true;	
    if(strcmp(tmpAccept.m_Symbol,"ag1707") !=0 ||tmpAccept.m_OrderState != EES_OrderState_order_live)
    {
        b_ret = false;
    }
    
	
    ASSERT_TRUE(b_ret);	
	
    delete m_pCRemGlobexCommunicator;
    delete pFileConfig;	
}


} // namespace exchange
} // namespace rem
} // namespace fh