#include <unistd.h>
#include <time.h>
#include <boost/container/flat_map.hpp>
#include "communicator.h"
#include "core/assist/logger.h"


namespace fh
{
namespace femas
{
namespace exchange
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////CFemasCommunicator//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFemasCommunicator::CFemasCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy)
{

     m_pFileConfig = new fh::core::assist::Settings(config_file);
     m_pUserApi = CUstpFtdcTraderApi::CreateFtdcTraderApi();	 
     m_pUstpFtdcTraderManger = new fh::femas::exchange::CUstpFtdcTraderManger(m_pUserApi);
     m_pUserApi->RegisterSpi(m_pUstpFtdcTraderManger);	
     m_pUstpFtdcTraderManger->SetFileConfigData(config_file);
     m_itimeout = 10;
     m_strategy =  strategy;  
     if(m_pUstpFtdcTraderManger != NULL)
     {
         m_pUstpFtdcTraderManger->SetStrategy(m_strategy);
     }
     m_ReqId = 0;	 
}

CFemasCommunicator::~CFemasCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");	 
     m_pUserApi->Release();	
     delete m_pUstpFtdcTraderManger;	 
}

bool CFemasCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
    m_pUserApi->SubscribePrivateTopic(USTP_TERT_RESUME);	 
    m_pUserApi->SubscribePublicTopic(USTP_TERT_RESUME);
    std::string tmpurl = m_pFileConfig->Get("femas-exchange.url");
    LOG_INFO("femas exchange url = ",tmpurl.c_str());	 
    m_pUserApi->RegisterFront((char*)(tmpurl.c_str()));	
    m_pUserApi->Init();
    m_itimeout = std::atoi((m_pFileConfig->Get("femas-timeout.timeout")).c_str());
    time_t tmtimeout = time(NULL);
    while(0 != m_pUstpFtdcTraderManger->mIConnet)
    {
        if(time(NULL)-tmtimeout>m_itimeout)
	 {
             LOG_ERROR("CFemasGlobexCommunicator::mIConnet tiomeout ");
	      break;		  
	 }
        sleep(0.1);    
     }	 	
     if(m_pUstpFtdcTraderManger->mIConnet != 0)
    {
        return false;
    }
    LOG_INFO("CFemasGlobexCommunicator::mIConnet is ok ");
    m_pUstpFtdcTraderManger->m_InitQueryNum = init_orders.size();	
    for(int i=0;i<init_orders.size();i++)
    {
        sleep(1); 
        Query(init_orders[i]);
    }
    tmtimeout = time(NULL);	
    int tmpQueryNum = m_pUstpFtdcTraderManger->m_InitQueryNum;
    while(0 != m_pUstpFtdcTraderManger->m_InitQueryNum)
    {
        if(time(NULL)-tmtimeout>m_itimeout)
	 {
            LOG_ERROR("CFemasGlobexCommunicator::InitQuery tiomeout ");
	     return false;
	 }
	 if(tmpQueryNum != m_pUstpFtdcTraderManger->m_InitQueryNum)
	 {
            tmpQueryNum = m_pUstpFtdcTraderManger->m_InitQueryNum;
	     tmtimeout = time(NULL);		
	 }
	 sleep(0.1);  
    } 

    //check suss order
    if(!SendReqQryTrade(init_orders))
    {
        LOG_ERROR("CFemasGlobexCommunicator::SendReqQryTrade is over ");
        return false;
    }
    //check suss position
    if(!SendReqQryInvestorPosition(init_orders))
    {
        LOG_ERROR("CFemasGlobexCommunicator::SendReqQryInvestorPosition is over ");
        return false;
    }
	
    m_strategy->OnExchangeReady(boost::container::flat_map<std::string, std::string>());	
    LOG_INFO("CFemasGlobexCommunicator::InitQuery is over ");	
    return true;
}

void CFemasCommunicator::Stop()
{
    LOG_INFO("CFemasGlobexCommunicator::Stop ");	
    CUstpFtdcReqUserLogoutField reqUserLogout;
    //make reqUserLogout struct
    std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
    strcpy(reqUserLogout.BrokerID, BrokerIDstr.c_str());
    LOG_INFO("femas-user.BrokerID = ",reqUserLogout.BrokerID);
    strcpy(reqUserLogout.UserID, UserIDstr.c_str());
    LOG_INFO("femas-user.UserID = ",reqUserLogout.UserID);
    // send message	    
    m_pUserApi->ReqUserLogout(&reqUserLogout,m_pUstpFtdcTraderManger->MaxOrderLocalID++);
    return;
}


void CFemasCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // make Initialize	
         LOG_INFO("CFemasGlobexCommunicator::Initialize ");	
         return;	
}

void CFemasCommunicator::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Add ");  
	 if(NULL == m_pUstpFtdcTraderManger)
	 {
            return ;
	 }	
	//make SInputOrder begin (have something todo) 	
        CUstpFtdcInputOrderField SInputOrder;
	 memset(&SInputOrder,0,sizeof(CUstpFtdcInputOrderField));	
	 //strncpy(SInputOrder.UserOrderLocalID,order.client_order_id().c_str(),order.client_order_id().length());
	 strncpy(SInputOrder.UserOrderLocalID,std::to_string(m_pUstpFtdcTraderManger->MaxOrderLocalID).c_str(),std::to_string(m_pUstpFtdcTraderManger->MaxOrderLocalID).length());
        m_pUstpFtdcTraderManger->AddOrderId(order.client_order_id());

	 
	 std::string BrokerID = m_pFileConfig->Get("femas-user.BrokerID");
	 strncpy(SInputOrder.BrokerID,BrokerID.c_str(),BrokerID.length());
	 std::string UserID = order.account();
        strncpy(SInputOrder.UserID,UserID.c_str(),UserID.length());	 
	 std::string InstrumentID = order.contract();
        strncpy(SInputOrder.InstrumentID,InstrumentID.c_str(),InstrumentID.length()); 
        std::string InvestorID = m_pFileConfig->Get("femas-exchange.InvestorID");
	 strcpy(SInputOrder.InvestorID , InvestorID.c_str());   			 
	 pb::ems::BuySell BuySellval = order.buy_sell();
	 if(BuySellval == 1)
	 {
            SInputOrder.Direction='0';
	 }
	 else
	 if(BuySellval == 2)	
	 {
            SInputOrder.Direction='1';
	 }
	 else
	 {
            //return;
	 }
	 std::string OffsetFlag = m_pFileConfig->Get("femas-exchange.OffsetFlag");
	 SInputOrder.OffsetFlag = OffsetFlag.c_str()[0];
	 std::string HedgeFlag = m_pFileConfig->Get("femas-exchange.HedgeFlag");
        SInputOrder.HedgeFlag = HedgeFlag.c_str()[0];		
        SInputOrder.LimitPrice = atof(order.price().c_str());
        SInputOrder.Volume = order.quantity();
	 //
	 if(SInputOrder.LimitPrice <=0 || SInputOrder.Volume <=0)
	 {
	     ::pb::ems::Order tmporder;
	     tmporder.set_client_order_id(order.client_order_id());	
            tmporder.set_account(order.account());
            tmporder.set_contract(order.contract());	 
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);	
	     m_strategy->OnOrder(tmporder);
	     LOG_ERROR("Add Order Error");	 
	     return;
	 }
	 //
        std::string TimeCondition = m_pFileConfig->Get("femas-exchange.TimeCondition");
	 SInputOrder.TimeCondition = TimeCondition.c_str()[0];
        std::string IsAutoSuspend = m_pFileConfig->Get("femas-exchange.IsAutoSuspend"); 
	 SInputOrder.IsAutoSuspend = atoi(IsAutoSuspend.c_str());
        std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(SInputOrder.ExchangeID , ExchangeID.c_str());  
	 if(order.order_type() == pb::ems::OrderType::OT_Limit)
	 {
            SInputOrder.OrderPriceType = '2';
	 }
	 else
	 if(order.order_type() == pb::ems::OrderType::OT_Market)
	 {
            SInputOrder.OrderPriceType = '1';
	 }
	 SInputOrder.VolumeCondition=(m_pFileConfig->Get("femas-exchange.VolumeCondition")).c_str()[0];
	 SInputOrder.ForceCloseReason=(m_pFileConfig->Get("femas-exchange.ForceCloseReason")).c_str()[0];
	 
	 
	 //end	
        //print message
        LOG_INFO("CUstpFtdcInputOrderField: ");
        LOG_INFO("UserOrderLocalID:",SInputOrder.UserOrderLocalID);
	 LOG_INFO("BrokerID: ",SInputOrder.BrokerID);
	 LOG_INFO("UserID: ",SInputOrder.UserID);
	 LOG_INFO("InstrumentID: ",SInputOrder.InstrumentID);
	 LOG_INFO("InvestorID: ",SInputOrder.InvestorID);
	 LOG_INFO("Direction: ",SInputOrder.Direction);
	 LOG_INFO("OffsetFlag: ",SInputOrder.OffsetFlag);
	 LOG_INFO("HedgeFlag: ",SInputOrder.HedgeFlag);
	 LOG_INFO("LimitPrice: ",SInputOrder.LimitPrice);
	 LOG_INFO("Volume: ",SInputOrder.Volume);
	 LOG_INFO("TimeCondition: ",SInputOrder.TimeCondition);
	 LOG_INFO("IsAutoSuspend: ",SInputOrder.IsAutoSuspend);
	 LOG_INFO("ExchangeID: ",SInputOrder.ExchangeID);
	 LOG_INFO("OrderPriceType: ",SInputOrder.OrderPriceType);
	 LOG_INFO("VolumeCondition: ",SInputOrder.VolumeCondition);
	 LOG_INFO("ForceCloseReason: ",SInputOrder.ForceCloseReason);
	 // send message
        m_pUserApi->ReqOrderInsert(&SInputOrder, m_pUstpFtdcTraderManger->MaxOrderLocalID++);
        return;
}

void CFemasCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Change ");		
        return;
}

void CFemasCommunicator::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Delete ");
	 //make OrderAction begin (have something todo) 	
        CUstpFtdcOrderActionField OrderAction;
        memset(&OrderAction,0,sizeof(CUstpFtdcOrderActionField));
	 strncpy(OrderAction.OrderSysID,order.exchange_order_id().c_str(),order.exchange_order_id().length());	
        //femas 
        strncpy(OrderAction.UserOrderLocalID,order.client_order_id().c_str(),order.client_order_id().length()); 
	 //strcpy(OrderAction.UserOrderLocalID,"");	
	 strncpy(OrderAction.UserOrderActionLocalID,std::to_string(m_pUstpFtdcTraderManger->MaxOrderLocalID).c_str(),std::to_string(m_pUstpFtdcTraderManger->MaxOrderLocalID).length());
        m_pUstpFtdcTraderManger->AddOrderId(order.client_order_id());
	 
        std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(OrderAction.ExchangeID , ExchangeID.c_str());
        std::string BrokerID = m_pFileConfig->Get("femas-user.BrokerID");
	 strncpy(OrderAction.BrokerID,BrokerID.c_str(),BrokerID.length());
	 std::string UserID = order.account();
        strncpy(OrderAction.UserID,UserID.c_str(),UserID.length());
	 std::string InvestorID = m_pFileConfig->Get("femas-exchange.InvestorID");
	 strcpy(OrderAction.InvestorID , InvestorID.c_str());  		 
	 OrderAction.ActionFlag=USTP_FTDC_AF_Delete;
	 // send message	
	 m_pUserApi->ReqOrderAction(&OrderAction, m_pUstpFtdcTraderManger->MaxOrderLocalID++);	
        return;
}

void CFemasCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Query ");
        //make QryOrder begin (have something todo)
        CUstpFtdcQryOrderField QryOrder;
        memset(&QryOrder,0,sizeof(CUstpFtdcQryOrderField));
       
        m_pUstpFtdcTraderManger->AddOrderId(order.client_order_id());
		
        std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(QryOrder.ExchangeID , ExchangeID.c_str());
        std::string BrokerID = m_pFileConfig->Get("femas-user.BrokerID");
	 strncpy(QryOrder.BrokerID,BrokerID.c_str(),BrokerID.length());
	 std::string UserID = order.account();
        strncpy(QryOrder.UserID,UserID.c_str(),UserID.length());
	 std::string InvestorID = m_pFileConfig->Get("femas-exchange.InvestorID");
	 strcpy(QryOrder.InvestorID , InvestorID.c_str()); 
	 strcpy(QryOrder.OrderSysID,order.exchange_order_id().c_str());

        if(order.status() == pb::ems::OrderStatus::OS_Cancelled)
	 {
            QryOrder.OrderStatus = '5';
	 }
	 else
	 if(order.status() == pb::ems::OrderStatus::OS_Filled)
	 {
            QryOrder.OrderStatus = '0';
	 }
	 else
	 if(order.status() == pb::ems::OrderStatus::OS_Rejected)
	 {
            QryOrder.OrderStatus = '4';
	 }
	 else
	 if(order.status() == pb::ems::OrderStatus::OS_Working)
	 {
            QryOrder.OrderStatus = '3';
	 }
	 else
	 if(order.status() == pb::ems::OrderStatus::OS_Pending)
	 {
            QryOrder.OrderStatus = '6';
	 }
	 else
	 {
            LOG_ERROR("order.status = ",order.status());
	 }
	 // send message
	 m_pUserApi->ReqQryOrder(&QryOrder,m_pUstpFtdcTraderManger->MaxOrderLocalID++);		
        return;
}

void CFemasCommunicator::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasGlobexCommunicator::Query_mass ");
        return;
}

void CFemasCommunicator::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasGlobexCommunicator::Delete_mass ");
        return;
}

bool CFemasCommunicator::SendReqQryTrade(const std::vector<::pb::ems::Order> &init_orders)
{
    LOG_INFO("CFemasGlobexCommunicator::SendReqQryTrade ");
    for(int i=0;i<init_orders.size();i++)
    {
        CUstpFtdcQryTradeField mQryTrade;
	 memset(&mQryTrade,0,sizeof(CUstpFtdcQryTradeField));	
	 std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(mQryTrade.ExchangeID , ExchangeID.c_str());

	 std::string BrokerID = m_pFileConfig->Get("femas-user.BrokerID");
	 strncpy(mQryTrade.BrokerID,BrokerID.c_str(),BrokerID.length());

	 std::string InvestorID = m_pFileConfig->Get("femas-exchange.InvestorID");
	 strcpy(mQryTrade.InvestorID , InvestorID.c_str()); 

        std::string UserID = init_orders[i].account();
	 strcpy(mQryTrade.UserID,UserID.c_str());
	 
        m_pUserApi->ReqQryTrade(&mQryTrade,m_ReqId++);    

	 time_t tmtimeout = time(NULL);
	 m_pUstpFtdcTraderManger->m_startfinish = false;
	 while(!m_pUstpFtdcTraderManger->m_startfinish)
	 {
            if(time(NULL)-tmtimeout>m_itimeout)
	     {
                 LOG_ERROR("CRemGlobexCommunicator::SendReqQryTrade tiomeout ");
	         return false;
	     }
	     sleep(0.1);		
	 }
    }	
    return true;	
}

bool CFemasCommunicator::SendReqQryInvestorPosition(const std::vector<::pb::ems::Order> &init_orders)
{
    LOG_INFO("CFemasGlobexCommunicator::SendReqQryInvestorPosition ");
    for(int i=0;i<init_orders.size();i++)
    {
        CUstpFtdcQryInvestorPositionField mQryInvestorPosition;
        memset(&mQryInvestorPosition,0,sizeof(CUstpFtdcQryInvestorPositionField));

	 std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(mQryInvestorPosition.ExchangeID , ExchangeID.c_str());

	 std::string BrokerID = m_pFileConfig->Get("femas-user.BrokerID");
	 strncpy(mQryInvestorPosition.BrokerID,BrokerID.c_str(),BrokerID.length());

	 std::string InvestorID = m_pFileConfig->Get("femas-exchange.InvestorID");
	 strcpy(mQryInvestorPosition.InvestorID , InvestorID.c_str()); 	

	 std::string UserID = init_orders[i].account();
	 strcpy(mQryInvestorPosition.UserID,UserID.c_str());

	 std::string InstrumentID = init_orders[i].contract();
        strncpy(mQryInvestorPosition.InstrumentID,InstrumentID.c_str(),InstrumentID.length()); 
		
        m_pUserApi->ReqQryInvestorPosition(&mQryInvestorPosition,m_ReqId++);

	 time_t tmtimeout = time(NULL);
	 m_pUstpFtdcTraderManger->m_startfinish = false;
	 while(!m_pUstpFtdcTraderManger->m_startfinish)
	 {
            if(time(NULL)-tmtimeout>m_itimeout)
	     {
                 LOG_ERROR("CRemGlobexCommunicator::SendReqQryInvestorPosition tiomeout ");
	         return false;
	     }
	     sleep(0.1);		
	 }	
    }
    return true;	
}



}
}
}