#include <unistd.h>
#include <time.h>
#include "rem_communicator.h"
#include "core/assist/logger.h"
#include <boost/container/flat_map.hpp>


namespace fh
{
namespace rem
{
namespace exchange
{


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////CRemCommunicator///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRemCommunicator::CRemCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy)
{

     m_pFileConfig = new fh::core::assist::Settings(config_file);
     m_pUserApi = CreateEESTraderApi();	 
     m_pEESTraderApiManger = new CEESTraderApiManger(m_pUserApi);

     m_pEESTraderApiManger->SetFileConfigData(config_file);

     m_strategy = strategy;
     if(m_pEESTraderApiManger != NULL)
     {
         m_pEESTraderApiManger->SetStrategy(m_strategy);
     }	 
     m_itimeout = 10;
     m_ReqId = 0;	 
      
}

CRemCommunicator::~CRemCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     DestroyEESTraderApi(m_pUserApi);	 
}

bool CRemCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
    if(NULL == m_pFileConfig)
    {
        LOG_ERROR("CRemGlobexCommunicator::Initialize m_pFileConfig == NULL"); 
	 return false;		 
    }
    m_itimeout = std::atoi((m_pFileConfig->Get("rem-timeout.timeout")).c_str());
    std::string svrAddr = m_pFileConfig->Get("rem-exchange.IP");
    int  Port = std::atoi((m_pFileConfig->Get("rem-exchange.Port")).c_str());	 
    m_pUserApi->ConnServer(svrAddr.c_str(), Port, m_pEESTraderApiManger);

    time_t tmtimeout = time(NULL);
    while(0 != m_pEESTraderApiManger->mIConnet)
    {
        if(time(NULL)-tmtimeout>m_itimeout)
	 {
              LOG_ERROR("CRemGlobexCommunicator::mIConnet tiomeout ");
	       break;	  
	  }
         sleep(0.1);    
    }
    if(m_pEESTraderApiManger->mIConnet != 0)
    {
        return false;
    }
    LOG_INFO("CRemGlobexCommunicator::mIConnet is ok ");
    m_pEESTraderApiManger->m_InitQueryNum = init_orders.size();
    for(unsigned int i=0;i<init_orders.size();i++)
    {
        Query(init_orders[i]);
    }
    tmtimeout = time(NULL);	
    int tmpQueryNum = m_pEESTraderApiManger->m_InitQueryNum;
    while(0 != m_pEESTraderApiManger->m_InitQueryNum)
    {
        if(time(NULL)-tmtimeout>m_itimeout)
	 {
            LOG_ERROR("CRemGlobexCommunicator::InitQuery tiomeout ");
	     return false;
	 }
	 if(tmpQueryNum != m_pEESTraderApiManger->m_InitQueryNum)
	 {
            tmpQueryNum = m_pEESTraderApiManger->m_InitQueryNum;
	     tmtimeout = time(NULL);		
	 }
	 sleep(0.1);  
    } 
//check suss order
    std::string oldaccount = "";
    for(unsigned int i=0;i<init_orders.size();i++)
    {
        if(oldaccount == "" || oldaccount != init_orders[i].account())
	 {
	     oldaccount =  init_orders[i].account();
            m_pUserApi->QueryAccountOrderExecution(init_orders[i].account().c_str()); 
	     tmtimeout = time(NULL);
	     m_pEESTraderApiManger->m_startfinish = false;	 
	     while(!m_pEESTraderApiManger->m_startfinish)	
	     {
                 if(time(NULL)-tmtimeout>m_itimeout)
		   {
                      LOG_ERROR("CRemGlobexCommunicator::QueryAccountOrderExecution tiomeout ");
			 return false; 		  
		   }
		   sleep(0.1);		 
	     }	 	 
	 }
           
    }

//check suss position
    oldaccount = "";
    for(unsigned int i=0;i<init_orders.size();i++)
    {
        if(oldaccount == "" || oldaccount != init_orders[i].account())
	 {
	     oldaccount =  init_orders[i].account();
            m_pUserApi->QueryAccountPosition(init_orders[i].account().c_str(),m_pEESTraderApiManger->MaxOrderLocalID++); 
	     tmtimeout = time(NULL);
	     m_pEESTraderApiManger->m_startfinish = false;	 
	     while(!m_pEESTraderApiManger->m_startfinish)	
	     {
                 if(time(NULL)-tmtimeout>m_itimeout)
		   {
                      LOG_ERROR("CRemGlobexCommunicator::QueryAccountPosition tiomeout ");
			 return false; 		  
		   }
		   sleep(0.1);		 
	     }	 	 
	 }
           
    } 

    if(!SendQuerySymbolList())
    {
        LOG_ERROR("CRemGlobexCommunicator::SendQuerySymbolList tiomeout ");
	 return false; 	
    }
	
    m_strategy->OnExchangeReady(boost::container::flat_map<std::string, std::string>());	
    LOG_INFO("CRemGlobexCommunicator::Start is over ");	
    return true;
}

void CRemCommunicator::Stop()
{
    LOG_INFO("CRemGlobexCommunicator::Stop ");
    m_pUserApi->DisConnServer();	
	
    return;
}


void CRemCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
         LOG_INFO("CRemGlobexCommunicator::Initialize "); 	
		
         return;	
}

void CRemCommunicator::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Add ");
        if(NULL == m_pEESTraderApiManger)
	 {
            return ;
	 }
		
        EES_ClientToken order_token = 0;
	 m_pUserApi->GetMaxToken(&order_token);

	EES_EnterOrderField temp;	
	memset(&temp, 0, sizeof(EES_EnterOrderField));	


	if(order.tif() == pb::ems::TimeInForce::TIF_FAK || order.tif() == pb::ems::TimeInForce::TIF_FOK)
	{
           temp.m_Tif = EES_OrderTif_IOC;
	}
	else
	{
           temp.m_Tif = EES_OrderTif_Day;
	}					
       std::string HedgeFlag = m_pFileConfig->Get("rem-exchange.HedgeFlag");
       temp.m_HedgeFlag = std::atoi(HedgeFlag.c_str());	

       std::string SecType = m_pFileConfig->Get("rem-exchange.SecType");
       temp.m_SecType = std::atoi(SecType.c_str());	

	std::string ExchangeID = m_pFileConfig->Get("rem-exchange.ExchangeID");
	temp.m_Exchange = std::atoi(ExchangeID.c_str());    

       pb::ems::BuySell BuySellval = order.buy_sell();
	if(BuySellval == 1)
	{
           temp.m_Side = EES_SideType_open_long;
	}
	else
	if(BuySellval == 2)	
	{
           temp.m_Side = EES_SideType_open_short;
	}
	else
	{
           temp.m_Side = EES_SideType_open_long;
	}
	std::string UserID = order.account();
	strncpy(temp.m_Account,UserID.c_str(),UserID.length());		
	std::string InstrumentID = order.contract();
	strncpy(temp.m_Symbol,InstrumentID.c_str(),InstrumentID.length());	
       temp.m_Price = atof(order.price().c_str());
       temp.m_Qty = order.quantity();
	if(temp.m_Price <=0 || temp.m_Qty <=0)
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
//demo
/*temp.m_Tif = EES_OrderTif_Day;
temp.m_HedgeFlag = EES_HedgeFlag_Speculation;
strcpy(temp.m_Account, "000240");	
strcpy(temp.m_Symbol, "T1709");	
temp.m_Side = EES_SideType_open_long;	//temp.m_Side = EES_SideType_open_short;	
temp.m_Exchange = EES_ExchangeID_cffex;	
temp.m_SecType = EES_SecType_fut;	
temp.m_Price = 2100.0;	
temp.m_Qty = 1;
*/
	temp.m_ClientOrderToken = order_token + 1;
       m_pEESTraderApiManger->AddOrderId(order.client_order_id(),temp.m_ClientOrderToken);

	LOG_INFO("Tif:",temp.m_Tif);
	LOG_INFO("HedgeFlag:",(int)(temp.m_HedgeFlag));
	LOG_INFO("SecType:",(int)(temp.m_SecType));
	LOG_INFO("Account:",temp.m_Account);
	LOG_INFO("Symbol:",temp.m_Symbol);
	LOG_INFO("Side:",temp.m_Side);
	LOG_INFO("Exchange:",(int)(temp.m_Exchange));
	LOG_INFO("Price:",temp.m_Price);
	LOG_INFO("Qty:",temp.m_Qty);
	
	m_pUserApi->EnterOrder(&temp);			        
        return;
}

void CRemCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Change ");
        return;
}

void CRemCommunicator::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Delete ");
	 EES_CancelOrder tmpCxlOrder;
	 memset(&tmpCxlOrder,0,sizeof(EES_CancelOrder));

        std::string UserID = order.account();
        strncpy(tmpCxlOrder.m_Account,UserID.c_str(),UserID.length());

        tmpCxlOrder.m_Quantity = order.quantity();
		
	 tmpCxlOrder.m_MarketOrderToken = std::atoi(order.exchange_order_id().c_str());

	 m_pUserApi->CancelOrder(&tmpCxlOrder);	
        return;
}

void CRemCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Query ");
		
	 m_pUserApi->QueryAccountOrder(order.account().c_str()); 	
        return;
}

void CRemCommunicator::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Query_mass ");
        return;
}

void CRemCommunicator::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Delete_mass ");
        return;
}


bool CRemCommunicator::SendQuerySymbolList()
{
    LOG_INFO("CRemCommunicator::SendQuerySymbolList");
    m_pUserApi->QuerySymbolList();
    time_t tmtimeout = time(NULL);
    m_pEESTraderApiManger->m_startfinish = false;	 
    while(!m_pEESTraderApiManger->m_startfinish)	
    {
        if(time(NULL)-tmtimeout>m_itimeout)
	 {
             LOG_ERROR("CRemGlobexCommunicator::QueryAccountPosition tiomeout ");
	      return false; 		  
         }
	  sleep(0.1);		 
    }
    return true;	
}









}
}
}