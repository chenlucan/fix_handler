#include <unistd.h>
#include <time.h>
#include "RemEESTraderApiManger.h"
#include "core/assist/logger.h"
#include <boost/container/flat_map.hpp>


namespace fh
{
namespace rem
{
namespace exchange
{


const char* con_prodInfo="REM";
const char* con_macAddr="00:0C:29:E3:32:E4";


/// \brief �����������¼�
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

/// \brief	�����������Ͽ������յ������Ϣ
void CEESTraderApiManger::OnDisConnection(ERR_NO errNo, const char* pErrStr )
{
    LOG_INFO("CEESTraderApiManger::OnDisConnection");
}
/// \brief	��¼��Ϣ�Ļص�
void CEESTraderApiManger::OnUserLogon(EES_LogonResponse* pLogon)
{
    LOG_INFO("CEESTraderApiManger::OnUserLogon");

    if(NULL == pLogon)
    {
        LOG_ERROR("CEESTraderApiManger::OnUserLogon Error");
	 return;	
    }
	 
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
/// \brief	�µ���REM���ܵ��¼�
void CEESTraderApiManger::OnOrderAccept(EES_OrderAcceptField* pAccept )
{
    LOG_INFO("CEESTraderApiManger::OnOrderAccept");
    if(NULL == pAccept)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderAccept Error");
	 return;	
    }	
    SendOrderAccept(pAccept);	
}
/// \brief	�µ����г����ܵ��¼�
void CEESTraderApiManger::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketAccept");
    if(NULL == pAccept)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderMarketAccept Error");
	 return;	
    }		
    SendOrderMarketAccept(pAccept);	
}
///	\brief	�µ���REM�ܾ����¼�
void CEESTraderApiManger::OnOrderReject(EES_OrderRejectField* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnOrderReject");
    if(NULL == pReject)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderReject Error");
	 return;	
    }		
    LOG_INFO("m_ReasonCode: ",(int)pReject->m_ReasonCode);
    SendOrderReject(pReject);	
		
}
///	\brief	�µ����г��ܾ����¼�
void CEESTraderApiManger::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketReject");
    if(NULL == pReject)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderMarketReject Error");
	 return;	
    }		
    LOG_INFO("m_ReasonText:",pReject->m_ReasonText);	
    SendOrderMarketReject(pReject);	
}
///	\brief	�����ɽ�����Ϣ�¼�
void CEESTraderApiManger::OnOrderExecution(EES_OrderExecutionField* pExec )
{
    LOG_INFO("CEESTraderApiManger::OnOrderExecution");
    if(NULL == pExec)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderExecution Error");
	 return;	
    }		
    SendOrderExecution(pExec);	
}
///	\brief	�����ɹ������¼�
void CEESTraderApiManger::OnOrderCxled(EES_OrderCxled* pCxled )
{
    LOG_INFO("CEESTraderApiManger::OnOrderCxled");
    if(NULL == pCxled)
    {
        LOG_ERROR("CEESTraderApiManger::OnOrderCxled Error");
	 return;	
    }	
    SendOrderCxled(pCxled);	
}
///	\brief	�������ܾ�����Ϣ�¼�
void CEESTraderApiManger::OnCxlOrderReject(EES_CxlOrderRej* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnCxlOrderReject");
    if(NULL == pReject)
    {
        LOG_ERROR("CEESTraderApiManger::OnCxlOrderReject Error");
	 return;	
    }		
    LOG_INFO("m_ReasonCode:",pReject->m_ReasonCode);	
    LOG_INFO("m_ReasonText:",pReject->m_ReasonText);	
    SendCxlOrderReject(pReject);	
}
void CEESTraderApiManger::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrder");
    if(NULL == pQueryOrder)
    {
        LOG_ERROR("CEESTraderApiManger::OnQueryTradeOrder Error");
	 return;	
    }	
    if(m_InitQueryNum > 0)
    {
        m_InitQueryNum--; 
    }
    SendQueryTradeOrder(pAccount,pQueryOrder);	
}
void CEESTraderApiManger::OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrderExec");
    if(NULL == pQueryOrderExec)
    {
        LOG_ERROR("CEESTraderApiManger::OnQueryTradeOrderExec Error");
	 return;	
    }		
    SendQueryTradeOrderExec(pAccount,pQueryOrderExec,bFinish);	
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
    LOG_INFO("CEESTraderApiManger::SendOrderAccept");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        std::string tmpc_OrderId = GetOrderId(pAccept->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_ClientOrderToken|m_MarketOrderToken
	     tmpc_OrderId = std::to_string(pAccept->m_ClientOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pAccept->m_MarketOrderToken); 
	     tmporder.set_client_order_id(tmpc_OrderId);	 
	 }
	 else
	 {
            tmporder.set_client_order_id(GetOrderId(pAccept->m_ClientOrderToken));
	     AddOrderToken(pAccept->m_ClientOrderToken,pAccept->m_MarketOrderToken);	
	 }
	 

        
        //tmporder.set_client_order_id(GetOrderId(pAccept->m_ClientOrderToken));
	 //AddOrderToken(pAccept->m_ClientOrderToken,pAccept->m_MarketOrderToken);	
		
	 tmporder.set_account(std::to_string(pAccept->m_UserID));	
	 tmporder.set_exchange_order_id(std::to_string(pAccept->m_MarketOrderToken));
	 if(pAccept->m_OrderState == EES_OrderState_order_dead)
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
	 }
	 else
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
        
	 
	 if(pAccept->m_Side == EES_SideType_open_long ||pAccept->m_Side == EES_SideType_close_today_short
	      	||pAccept->m_Side == EES_SideType_close_ovn_short || pAccept->m_Side == EES_SideType_force_close_ovn_short
	      	||pAccept->m_Side == EES_SideType_force_close_today_short)
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
	 tmporder.set_contract(pAccept->m_Symbol);

	 tmporder.set_price(std::to_string(pAccept->m_Price));
        tmporder.set_quantity(pAccept->m_Qty);

	 tmporder.set_working_price(std::to_string(pAccept->m_Price));
        tmporder.set_working_quantity(pAccept->m_Qty);	
		
        if(pAccept->m_Tif == EES_OrderTif_IOC)
	{
            tmporder.set_tif(pb::ems::TimeInForce::TIF_FOK);
	}
	else
	{
            tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
	}
				
		
	 	
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pAccept->m_AcceptTime));

	 LOG_INFO("client_order_id:",pAccept->m_ClientOrderToken);
	 LOG_INFO("account:",pAccept->m_UserID);
	 LOG_INFO("exchange_order_id:",pAccept->m_MarketOrderToken);
	 LOG_INFO("OrderState:",pAccept->m_OrderState);
	 LOG_INFO("Side:",pAccept->m_Side);

	 LOG_INFO("contract:",pAccept->m_Symbol);
	 LOG_INFO("price:",pAccept->m_Price);
	 LOG_INFO("quantity:",pAccept->m_Qty);
	 LOG_INFO("ActionDay:",pAccept->m_AcceptTime);
		
	 m_strategy->OnOrder(tmporder);	
    }
}
void CEESTraderApiManger::SendOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)	
{
    LOG_INFO("CEESTraderApiManger::SendOrderMarketAccept");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        std::string tmpc_OrderId; //= GetOrderToken(pAccept->m_MarketOrderToken);
        if(GetOrderToken(pAccept->m_MarketOrderToken) == -1)
	 {
	     //m_MarketOrderToken|m_MarketOrderId
	     tmpc_OrderId = std::to_string(pAccept->m_MarketOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pAccept->m_MarketOrderId; 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);

        //tmporder.set_client_order_id(GetOrderId(GetOrderToken(pAccept->m_MarketOrderToken)));
	 tmporder.set_account(pAccept->m_Account);	
	 tmporder.set_exchange_order_id(std::to_string(pAccept->m_MarketOrderToken));
	 tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pAccept->m_MarketTime));
	 
        LOG_INFO("account:",pAccept->m_Account);
	 LOG_INFO("client_order_id:",pAccept->m_MarketOrderToken);
	 LOG_INFO("exchange_order_id:",pAccept->m_MarketOrderToken);
		
	 m_strategy->OnOrder(tmporder);	
    }	
}
void CEESTraderApiManger::SendOrderReject(EES_OrderRejectField* pReject)
{
    LOG_INFO("CEESTraderApiManger::SendOrderReject");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        tmporder.set_account(std::to_string(pReject->m_Userid));

	 std::string tmpc_OrderId = GetOrderId(pReject->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_Userid|m_ClientOrderToken
	     tmpc_OrderId = std::to_string(pReject->m_Userid);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pReject->m_ClientOrderToken); 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
	 
	 //tmporder.set_client_order_id(GetOrderId(pReject->m_ClientOrderToken));
        tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
	 //AddOrderToken(pReject->m_ClientOrderToken,pReject->m_MarketOrderToken);	
	 fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pReject->m_Timestamp));
	 
	 LOG_INFO("account:",pReject->m_Userid);
	 LOG_INFO("client_order_id:",pReject->m_ClientOrderToken);
		
	 m_strategy->OnOrder(tmporder);	
    }	
}
void CEESTraderApiManger::SendOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
    LOG_INFO("CEESTraderApiManger::SendOrderMarketReject");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        tmporder.set_account(pReject->m_Account);

        std::string tmpc_OrderId; //= GetOrderToken(pAccept->m_MarketOrderToken);
        if(GetOrderToken(pReject->m_MarketOrderToken) == -1)
	 {
	     //m_Account|m_MarketOrderToken
	     tmpc_OrderId = pReject->m_Account;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pReject->m_MarketOrderToken); 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
		
	 //tmporder.set_client_order_id(GetOrderId(GetOrderToken(pReject->m_MarketOrderToken)));	
	 tmporder.set_exchange_order_id(std::to_string(pReject->m_MarketOrderToken));	
        tmporder.set_status(pb::ems::OrderStatus::OS_Rejected); 
	 fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pReject->m_MarketTimestamp));	
	 
	 LOG_INFO("account:",pReject->m_Account);
	 LOG_INFO("exchange_order_id:",pReject->m_MarketOrderToken);	
		
	 m_strategy->OnOrder(tmporder);	
    }	
}
void CEESTraderApiManger::SendOrderExecution(EES_OrderExecutionField* pExec)
{
    LOG_INFO("CEESTraderApiManger::SendOrderExecution");
    if(NULL != m_strategy)
    {
        ::pb::ems::Fill tmpfill;

	 std::string tmpc_OrderId = GetOrderId(pExec->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_ClientOrderToken|m_MarketOrderToken|m_MarketExecID
	     tmpc_OrderId = std::to_string(pExec->m_ClientOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pExec->m_MarketOrderToken); 
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pExec->m_MarketExecID; 	 
	     tmpfill.set_client_order_id(tmpc_OrderId);	 
	 }
	 else
	 {
            tmpfill.set_client_order_id(GetOrderId(pExec->m_ClientOrderToken));
	     AddOrderToken(pExec->m_ClientOrderToken,pExec->m_MarketOrderToken);
	 }	

        //tmpfill.set_client_order_id(GetOrderId(pExec->m_ClientOrderToken));
	 tmpfill.set_account(std::to_string(pExec->m_Userid));	
	 tmpfill.set_exchange_order_id(std::to_string(pExec->m_MarketOrderToken));
	 //AddOrderToken(pExec->m_ClientOrderToken,pExec->m_MarketOrderToken);
	 
	 tmpfill.set_fill_price(std::to_string(pExec->m_Price));
        tmpfill.set_fill_quantity(pExec->m_Quantity);
	 tmpfill.set_fill_id(pExec->m_MarketExecID);
	 fh::core::assist::utility::To_pb_time(tmpfill.mutable_fill_time(), fh::core::assist::utility::To_time_str(pExec->m_Timestamp));


	 LOG_INFO("client_order_id:",pExec->m_ClientOrderToken);
	 LOG_INFO("account:",pExec->m_Userid);	
	 LOG_INFO("exchange_order_id:",pExec->m_MarketOrderToken);
	 LOG_INFO("fill_price:",pExec->m_Price);
	 LOG_INFO("fill_quantity:",pExec->m_Quantity);
	 LOG_INFO("fill_id:",pExec->m_MarketExecID);
		
	 m_strategy->OnFill(tmpfill);	
    }	
}
void CEESTraderApiManger::SendOrderCxled(EES_OrderCxled* pCxled)
{
    LOG_INFO("CEESTraderApiManger::SendOrderCxled");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

	 std::string tmpc_OrderId = GetOrderId(pCxled->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_ClientOrderToken|m_MarketOrderToken
	     tmpc_OrderId = std::to_string(pCxled->m_ClientOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pCxled->m_MarketOrderToken); 	 
	     tmporder.set_client_order_id(tmpc_OrderId);	 
	 }
	 else
	 {
            tmporder.set_client_order_id(GetOrderId(pCxled->m_ClientOrderToken));
	     AddOrderToken(pCxled->m_ClientOrderToken,pCxled->m_MarketOrderToken);
	 }	

        tmporder.set_account(std::to_string(pCxled->m_Userid));
	 //tmporder.set_client_order_id(GetOrderId(pCxled->m_ClientOrderToken));	
	 tmporder.set_exchange_order_id(std::to_string(pCxled->m_MarketOrderToken));

        //AddOrderToken(pCxled->m_ClientOrderToken,pCxled->m_MarketOrderToken);
	 
        tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled); 
	 fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pCxled->m_Timestamp));	
	 
	 LOG_INFO("client_order_id:",pCxled->m_ClientOrderToken);
	 LOG_INFO("account:",pCxled->m_Userid);	
	 LOG_INFO("exchange_order_id:",pCxled->m_MarketOrderToken);
		
	 m_strategy->OnOrder(tmporder);	
    }	
}
void CEESTraderApiManger::SendCxlOrderReject(EES_CxlOrderRej* pReject)
{
    LOG_INFO("CEESTraderApiManger::SendCxlOrderReject");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        tmporder.set_account(pReject->m_account);
        std::string tmpc_OrderId; //= GetOrderToken(pAccept->m_MarketOrderToken);
        if(GetOrderToken(pReject->m_MarketOrderToken) == -1)
	 {
	     //m_account|m_MarketOrderToken
	     tmpc_OrderId = pReject->m_account;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pReject->m_MarketOrderToken); 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId); 
		
	 //tmporder.set_client_order_id(GetOrderId(GetOrderToken(pReject->m_MarketOrderToken)));		
	 tmporder.set_exchange_order_id(std::to_string(pReject->m_MarketOrderToken));	
        tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
	 	
	 
	 LOG_INFO("account:",pReject->m_account);	
	 LOG_INFO("exchange_order_id:",pReject->m_MarketOrderToken);
		
	 m_strategy->OnOrder(tmporder);	
    }	
}
void CEESTraderApiManger::SendQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder)
{
    LOG_INFO("CEESTraderApiManger::SendQueryTradeOrder");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	

        std::string tmpc_OrderId = GetOrderId(pQueryOrder->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_ClientOrderToken|m_MarketOrderToken|m_MarketOrderId
	     tmpc_OrderId = std::to_string(pQueryOrder->m_ClientOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pQueryOrder->m_MarketOrderToken); 	 
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pQueryOrder->m_MarketOrderId; 	  
	     tmporder.set_client_order_id(tmpc_OrderId);	 
	 }
	 else
	 {
            tmporder.set_client_order_id(GetOrderId(pQueryOrder->m_ClientOrderToken));
	     AddOrderToken(pQueryOrder->m_ClientOrderToken,pQueryOrder->m_MarketOrderToken); 
	 }

        //tmporder.set_client_order_id(GetOrderId(pQueryOrder->m_ClientOrderToken));
	 tmporder.set_account(std::to_string(pQueryOrder->m_Userid));	
	 tmporder.set_exchange_order_id(std::to_string(pQueryOrder->m_MarketOrderToken));
        //AddOrderToken(pQueryOrder->m_ClientOrderToken,pQueryOrder->m_MarketOrderToken); 
	 
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_cancelled)//OS_Cancelled
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	 }
	 else
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_executed)//OS_Working	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_closed)//OS_Rejected	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
	 }
	 else
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_mkt_accept)//OS_Pending	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
	 else
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_shengli_accept)//OS_Pending	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
	 else
	 	//////////////???????????????????
	 if(pQueryOrder->m_OrderStatus == EES_OrderStatus_cxl_requested)//OS_Cancelled	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	 }
	 
	 if(pQueryOrder->m_SideType == EES_SideType_open_long)
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 if(pQueryOrder->m_SideType == EES_SideType_open_short)	
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
	 tmporder.set_contract(pQueryOrder->m_symbol);

	 tmporder.set_price(std::to_string(pQueryOrder->m_Price));
        tmporder.set_quantity(pQueryOrder->m_Quantity);
		
	 //std::string tmpActionDay = pAccept->m_AcceptTime;	
        //fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpActionDay);
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pQueryOrder->m_Timestamp));	

	 LOG_INFO("client_order_id:",pQueryOrder->m_ClientOrderToken);
	 LOG_INFO("account:",pQueryOrder->m_Userid);
	 LOG_INFO("exchange_order_id:",pQueryOrder->m_MarketOrderToken);
	 LOG_INFO("OrderState:",pQueryOrder->m_OrderStatus);
	 LOG_INFO("Side:",pQueryOrder->m_SideType);

	 LOG_INFO("contract:",pQueryOrder->m_symbol);
	 LOG_INFO("price:",pQueryOrder->m_Price);
	 LOG_INFO("quantity:",pQueryOrder->m_Quantity);
	 LOG_INFO("ActionDay:",pQueryOrder->m_Timestamp);
		
	 m_strategy->OnOrder(tmporder);	
    }	
}

void CEESTraderApiManger::SendQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::SendQueryTradeOrderExec");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;

        std::string tmpc_OrderId = GetOrderId(pQueryOrderExec->m_ClientOrderToken);
        if(tmpc_OrderId == "")
	 {
	     //m_ClientOrderToken|m_MarketOrderToken|m_MarketExecID
	     tmpc_OrderId = std::to_string(pQueryOrderExec->m_ClientOrderToken);
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + std::to_string(pQueryOrderExec->m_MarketOrderToken); 
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pQueryOrderExec->m_MarketExecID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
        //tmporder.set_client_order_id(GetOrderId(pQueryOrderExec->m_ClientOrderToken));
	 tmporder.set_account(std::to_string(pQueryOrderExec->m_Userid));	
	 tmporder.set_exchange_order_id(std::to_string(pQueryOrderExec->m_MarketOrderToken));
	 tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	 tmporder.set_price(std::to_string(pQueryOrderExec->m_ExecutionPrice));
        tmporder.set_quantity(pQueryOrderExec->m_ExecutedQuantity);
	 tmporder.set_working_price(std::to_string(pQueryOrderExec->m_ExecutionPrice))	;
	 tmporder.set_working_quantity(0);
	 tmporder.set_filled_quantity(pQueryOrderExec->m_ExecutedQuantity);
	 
        
		
	 fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), fh::core::assist::utility::To_time_str(pQueryOrderExec->m_Timestamp));	

	 m_strategy->OnOrder(tmporder);		
    }
	
    if(bFinish)
    {
        m_startfinish =  bFinish;
    }
    return;
}

void CEESTraderApiManger::SendQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish)
{
    if(NULL != m_strategy)
    {
        ::pb::ems::Position tmpPosition;
	 core::exchange::PositionVec tmpPositionVec;
	 tmpPositionVec.clear();

	 
        tmpPosition.set_account(pAccoutnPosition->m_actId);
	 tmpPosition.set_contract(pAccoutnPosition->m_Symbol);	
	 tmpPosition.set_position(pAccoutnPosition->m_TodayQty);

	 if(pAccoutnPosition->m_PosiDirection == EES_PosiDirection_long)
	 {
            tmpPosition.set_today_long(pAccoutnPosition->m_TodayQty);
	 }
	 else
	 if(pAccoutnPosition->m_PosiDirection == EES_PosiDirection_short)	
	 {
            tmpPosition.set_today_short(pAccoutnPosition->m_TodayQty);
	 }

	 tmpPositionVec.push_back(tmpPosition);
	 	

	 m_strategy->OnPosition(tmpPositionVec);		
    }
    if(bFinish)
    {
        m_startfinish =  bFinish;
    }
    return;
}

void CEESTraderApiManger::AddOrderToken(int MarketOrderToken,int i_key)
{
    if(m_orderTokenmap.count(i_key) == 0)
    {
        m_orderTokenmap[i_key] = MarketOrderToken;		
    } 
    else
    if(m_orderTokenmap[i_key] != MarketOrderToken)		
    {
        //m_orderTokenmap[i_key] = MarketOrderToken;
        LOG_ERROR("CEESTraderApiManger::AddOrderToken error");
    }
    return;
}
void CEESTraderApiManger::AddOrderId(std::string cl_orderid,int i_key)
{

    if(m_ordermap.count(i_key) == 0)
    {
        m_ordermap[i_key] = cl_orderid;		
    } 
    else
    if(m_ordermap[i_key] != cl_orderid)		
    {
        //m_ordermap[i_key] = cl_orderid;
        LOG_ERROR("CEESTraderApiManger::AddOrderId error");
    }		
    return;	
}

int CEESTraderApiManger::GetOrderToken(int i_key)
{
    if(m_orderTokenmap.count(i_key) != 0)
    {
        return m_orderTokenmap[i_key];
    }
    else
    {
        return -1;
    }
}

std::string CEESTraderApiManger::GetOrderId(int i_key)
{
    if(m_ordermap.count(i_key) != 0)
    {
        return m_ordermap[i_key];
    }
    else
    {
        return "";
    }		
}

}
}
}