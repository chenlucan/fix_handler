#include "core/assist/logger.h"
#include "gmock/gmock.h"
#include "mut_femas_exchange.h"

namespace fh
{
namespace femas
{
namespace exchange
{

CMutExchangeFemas::CMutExchangeFemas()
{
        // noop
        
}      


CMutExchangeFemas::~CMutExchangeFemas()
{
        // noop
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test001)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    bool b_ret=false;	
    if(NULL != pFileConfig)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete pFileConfig;	
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test002)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7202","tcp://localhost:7201");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(10 == m_pCRemGlobexCommunicator->m_itimeout)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test003)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7302","tcp://localhost:7301");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(0 == m_pCRemGlobexCommunicator->m_ReqId)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test004)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7402","tcp://localhost:7401");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=true;

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

    CUstpFtdcInputOrderField  tmpInputOrder;
    memset(&tmpInputOrder,0,sizeof(CUstpFtdcInputOrderField));	
    strncpy(tmpInputOrder.UserOrderLocalID,std::to_string(m_pCRemGlobexCommunicator->m_pUstpFtdcTraderManger->MaxOrderLocalID-1).c_str(),std::to_string(m_pCRemGlobexCommunicator->m_pUstpFtdcTraderManger->MaxOrderLocalID-1).length());		
    strcpy(tmpInputOrder.OrderSysID,"       90714");
    strcpy(tmpInputOrder.UserID,"6660357901");
    strcpy(tmpInputOrder.InstrumentID,"ag1707");	
    tmpInputOrder.Direction = '0';
    tmpInputOrder.LimitPrice = 1930.0;
    tmpInputOrder.Volume = 5;		


    CUstpFtdcRspInfoField  pRspInfo;
    memset(&pRspInfo,0,sizeof(CUstpFtdcRspInfoField));
    pRspInfo.ErrorID = 0;
    strcpy(pRspInfo.ErrorMsg, "ÕýÈ·");	
    int  nRequestID = 100;
    bool bIsLast = true;
    m_pCRemGlobexCommunicator->m_pUstpFtdcTraderManger->OnRspOrderInsert(&tmpInputOrder, &pRspInfo, nRequestID,bIsLast);	


    if(0 != pRspInfo.ErrorID)
    {
        b_ret = false; 
    }
	
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}


} // namespace market
} // namespace femas
} // namespace fh