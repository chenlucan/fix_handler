#include <unistd.h>
#include <time.h>
#include "rem_communicator.h"
#include "core/assist/logger.h"


namespace fh
{
namespace rem
{
namespace exchange
{


const char* con_prodInfo="REM";
const char* con_macAddr="00:0C:29:E3:32:E4";


/// \brief 服务器连接事件
void CEESTraderApiManger::OnConnection(ERR_NO errNo, const char* pErrStr )
{
    LOG_INFO("CEESTraderApiManger::OnConnection");
    if(NULL == m_pFileConfig)
    {
           LOG_ERROR("Error m_pFileConfig is NULL.");
	    return;	   
    }
    if(NULL == m_pUserApi)
    {
           LOG_ERROR("Error m_pUserApi is NULL.");
	    return;	   
    } 
    std::string LogIDstr = m_pFileConfig->Get("rem-user.LogID");
    std::string Passwordstr = m_pFileConfig->Get("rem-user.Password");
    LOG_INFO("LogIDstr = ",LogIDstr);
    LOG_INFO("Passwordstr = ",Passwordstr);	

    m_pUserApi->UserLogon(LogIDstr.c_str(),Passwordstr.c_str(),con_prodInfo,con_macAddr);
    return;	
}

/// \brief	服务器主动断开，会收到这个消息
void CEESTraderApiManger::OnDisConnection(ERR_NO errNo, const char* pErrStr )
{
    LOG_INFO("CEESTraderApiManger::OnDisConnection");
}
/// \brief	登录消息的回调
void CEESTraderApiManger::OnUserLogon(EES_LogonResponse* pLogon)
{
    LOG_INFO("CEESTraderApiManger::OnUserLogon");
    if(pLogon->m_Result != 0)
    {
        LOG_ERROR("CEESTraderApiManger::OnUserLogon fail  Result = ",pLogon->m_Result);
	 return;	
    }
    LOG_INFO("CEESTraderApiManger::OnUserLogon  suss");	
    mIConnet = 0;
    MaxOrderLocalID = pLogon->m_MaxToken;	
    return;	
}
void CEESTraderApiManger::OnRspChangePassword(EES_ChangePasswordResult nResult)
{
    LOG_INFO("CEESTraderApiManger::OnRspChangePassword");
}
void CEESTraderApiManger::OnQueryUserAccount(EES_AccountInfo * pAccoutnInfo, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryUserAccount");
}
void CEESTraderApiManger::OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountPosition");
}
void CEESTraderApiManger::OnQueryAccountBP(const char* pAccount, EES_AccountBP* pAccoutnPosition, int nReqId )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountBP");
}
void CEESTraderApiManger::OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQuerySymbol");
}
void CEESTraderApiManger::OnQueryAccountTradeMargin(const char* pAccount, EES_AccountMargin* pSymbolMargin, bool bFinish )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountTradeMargin");
}
void CEESTraderApiManger::OnQueryAccountTradeFee(const char* pAccount, EES_AccountFee* pSymbolFee, bool bFinish )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountTradeFee");
}
/// \brief	下单被REM接受的事件
void CEESTraderApiManger::OnOrderAccept(EES_OrderAcceptField* pAccept )
{
    LOG_INFO("CEESTraderApiManger::OnOrderAccept");
    SendOrderAccept(pAccept);	
}
/// \brief	下单被市场接受的事件
void CEESTraderApiManger::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketAccept");
    SendOrderMarketAccept(pAccept);	
}
///	\brief	下单被REM拒绝的事件
void CEESTraderApiManger::OnOrderReject(EES_OrderRejectField* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnOrderReject");	
    LOG_INFO("m_ReasonCode: ",(int)pReject->m_ReasonCode);
    SendOrderReject(pReject);	
		
}
///	\brief	下单被市场拒绝的事件
void CEESTraderApiManger::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketReject");
    SendOrderMarketReject(pReject);	
}
///	\brief	订单成交的消息事件
void CEESTraderApiManger::OnOrderExecution(EES_OrderExecutionField* pExec )
{
    LOG_INFO("CEESTraderApiManger::OnOrderExecution");
    SendOrderExecution(pExec);	
}
///	\brief	订单成功撤销事件
void CEESTraderApiManger::OnOrderCxled(EES_OrderCxled* pCxled )
{
    LOG_INFO("CEESTraderApiManger::OnOrderCxled");
    SendOrderCxled(pCxled);	
}
///	\brief	撤单被拒绝的消息事件
void CEESTraderApiManger::OnCxlOrderReject(EES_CxlOrderRej* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnCxlOrderReject");
    SendCxlOrderReject(pReject);	
}
void CEESTraderApiManger::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrder");
}
void CEESTraderApiManger::OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrderExec");
}
void CEESTraderApiManger::OnPostOrder(EES_PostOrder* pPostOrder )
{
    LOG_INFO("CEESTraderApiManger::OnPostOrder");
}
void CEESTraderApiManger::OnPostOrderExecution(EES_PostOrderExecution* pPostOrderExecution )
{
    LOG_INFO("CEESTraderApiManger::OnPostOrderExecution");
}
void CEESTraderApiManger::OnQueryMarketSession(EES_ExchangeMarketSession* pMarketSession, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryMarketSession");
}
void CEESTraderApiManger::OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood)
{
    LOG_INFO("CEESTraderApiManger::OnMarketSessionStatReport");
}
void CEESTraderApiManger::OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus)
{
    LOG_INFO("CEESTraderApiManger::OnSymbolStatusReport");
}
void CEESTraderApiManger::OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQuerySymbolStatus");
}

		   
void CEESTraderApiManger::SetFileConfigData(const std::string &FileConfig)
{
    LOG_INFO("CUstpFtdcTraderManger::SetFileConfigData file =  ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
}



void CEESTraderApiManger::SendOrderAccept(EES_OrderAcceptField* pAccept)
{

}
void CEESTraderApiManger::SendOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)	
{

}
void CEESTraderApiManger::SendOrderReject(EES_OrderRejectField* pReject)
{

}
void CEESTraderApiManger::SendOrderMarketReject(EES_OrderMarketRejectField* pReject)
{

}
void CEESTraderApiManger::SendOrderExecution(EES_OrderExecutionField* pExec)
{

}
void CEESTraderApiManger::SendOrderCxled(EES_OrderCxled* pCxled)
{

}
void CEESTraderApiManger::SendCxlOrderReject(EES_CxlOrderRej* pReject)
{

}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////CRemGlobexCommunicator///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRemGlobexCommunicator::CRemGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
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
      
}

CRemGlobexCommunicator::~CRemGlobexCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     DestroyEESTraderApi(m_pUserApi);	 
}

bool CRemGlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     if(m_pEESTraderApiManger->mIConnet != 0)
    {
        return false;
    }
    return true;
}

void CRemGlobexCommunicator::Stop()
{
    LOG_INFO("CRemGlobexCommunicator::Stop ");
    m_pUserApi->DisConnServer();	
	
    return;
}


void CRemGlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
         LOG_INFO("CRemGlobexCommunicator::Initialize "); 

	  if(NULL == m_pFileConfig)
	  {
             LOG_ERROR("CRemGlobexCommunicator::Initialize m_pFileConfig == NULL"); 
	      return;		 
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
	           return;		  
	      }
             sleep(0.1);    
          }	
		
          return;	
}

void CRemGlobexCommunicator::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Add ");
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
       temp.m_HedgeFlag = HedgeFlag.c_str()[0];	

       std::string SecType = m_pFileConfig->Get("rem-exchange.SecType");
       temp.m_SecType = SecType.c_str()[0];	

	std::string ExchangeID = m_pFileConfig->Get("rem-exchange.ExchangeID");
	temp.m_Exchange = ExchangeID.c_str()[0];    

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
           //return;
	}
	std::string UserID = order.account();
	strncpy(temp.m_Account,UserID.c_str(),UserID.length());		
	std::string InstrumentID = order.contract();
	strncpy(temp.m_Symbol,InstrumentID.c_str(),InstrumentID.length());
	
       temp.m_Price = atof(order.price().c_str());
       temp.m_Qty = order.quantity();


	temp.m_ClientOrderToken = order_token + 1;

	RESULT ret = m_pUserApi->EnterOrder(&temp);	
		
        
        return;
}

void CRemGlobexCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Change ");
        return;
}

void CRemGlobexCommunicator::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Delete ");
        return;
}

void CRemGlobexCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Query ");
        return;
}

void CRemGlobexCommunicator::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Query_mass ");
        return;
}

void CRemGlobexCommunicator::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Delete_mass ");
        return;
}












}
}
}