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
    LOG_INFO("m_ReasonText:",pReject->m_ReasonText);	
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
    LOG_INFO("m_ReasonCode:",pReject->m_ReasonCode);	
    LOG_INFO("m_ReasonText:",pReject->m_ReasonText);	
    SendCxlOrderReject(pReject);	
}
void CEESTraderApiManger::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrder");
    if(m_InitQueryNum > 0)
    {
        m_InitQueryNum--; 
    }
    SendQueryTradeOrder(pAccount,pQueryOrder);	
}
void CEESTraderApiManger::OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrderExec");
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
     m_ReqId = 0;	 
      
}

CRemGlobexCommunicator::~CRemGlobexCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     DestroyEESTraderApi(m_pUserApi);	 
}

bool CRemGlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
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
    for(int i=0;i<init_orders.size();i++)
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
    for(int i=0;i<init_orders.size();i++)
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
    for(int i=0;i<init_orders.size();i++)
    {
        if(oldaccount == "" || oldaccount != init_orders[i].account())
	 {
	     oldaccount =  init_orders[i].account();
            m_pUserApi->QueryAccountPosition(init_orders[i].account().c_str(),m_ReqId++); 
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
	
    m_strategy->OnExchangeReady(boost::container::flat_map<std::string, std::string>());	
    LOG_INFO("CRemGlobexCommunicator::InitQuery is over ");	
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
		
         return;	
}

void CRemGlobexCommunicator::Add(const ::pb::ems::Order& order)
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
	if(temp.LimitPrice <=0 || temp.Volume <=0)
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

void CRemGlobexCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Change ");
        return;
}

void CRemGlobexCommunicator::Delete(const ::pb::ems::Order& order)
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

void CRemGlobexCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Query ");
		
	 m_pUserApi->QueryAccountOrder(order.account().c_str()); 	
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