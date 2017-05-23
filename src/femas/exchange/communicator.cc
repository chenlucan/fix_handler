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
// 当客户端与飞马平台建立起通信连接，客户端需要进行登录
void CUstpFtdcTraderManger::OnFrontConnected()
{
    LOG_INFO("CUstpFtdcTraderManger::OnFrontConnected\n");
    CUstpFtdcReqUserLoginField reqUserLogin;

    std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
    std::string Passwordstr = m_pFileConfig->Get("femas-user.Password");
    strcpy(reqUserLogin.BrokerID, BrokerIDstr.c_str());
    LOG_INFO("femas-user.BrokerID = ",reqUserLogin.BrokerID);
    strcpy(reqUserLogin.UserID, UserIDstr.c_str());
    LOG_INFO("femas-user.UserID = ",reqUserLogin.UserID);
    strcpy(reqUserLogin.Password, Passwordstr.c_str());
    LOG_INFO("femas-user.Passwor = ",reqUserLogin.Password);
    m_pUserApi->ReqUserLogin(&reqUserLogin, 0);	
	
}
void CUstpFtdcTraderManger::OnFrontDisconnected(int nReason)
{
    // 当发生这个情况后，API会自动重新连接，客户端可不做处理
    LOG_INFO("CUstpFtdcTraderManger::OnFrontDisconnected");
}
void CUstpFtdcTraderManger::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspUserLogin:");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID, "]ErrorMsg=[",pRspInfo->ErrorMsg,"]");
    LOG_INFO("RequestID=[",nRequestID,"], Chain=[",bIsLast,"]");
    if (pRspInfo->ErrorID != 0) 
    {
		// 端登失败，客户端需进行错误处理
         LOG_ERROR("Failed to login, errorcode=",pRspInfo->ErrorID," errormsg=",pRspInfo->ErrorMsg,"	requestid=",nRequestID," chain=", bIsLast);
	  //exit(-1);
	  mIConnet = 1;
	  return;
    }
    LOG_INFO("MaxOrderLocalID = ", atoi(pRspUserLogin->MaxOrderLocalID));	
    MaxOrderLocalID = atoi(pRspUserLogin->MaxOrderLocalID)+1;	
    //strncpy(MaxOrderLocalID,pRspUserLogin->MaxOrderLocalID,strlen(pRspUserLogin->MaxOrderLocalID));	
    mIConnet = 0;
    return;	
}	
void CUstpFtdcTraderManger::OnRspOrderInsert(CUstpFtdcInputOrderField  *pInputOrder, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    // 输出报单录入结果
    LOG_INFO("CUstpFtdcTraderManger::OnRspOrderInsert");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );

    OnInsertOrder(pInputOrder,pRspInfo);	

    return;	
}

///报单回报
void CUstpFtdcTraderManger::OnRtnOrder(CUstpFtdcOrderField  *pOrder)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRtnOrder");
    LOG_INFO("OrderSysID=[",pOrder->OrderSysID,"]");
    OnOrder(pOrder);	
}

// 针对用户请求的出错通知
void CUstpFtdcTraderManger::OnRspError(CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspError");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );
    LOG_INFO("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    // 客户端需进行错误处理

}
void CUstpFtdcTraderManger::OnRtnTrade(CUstpFtdcTradeField *pTrade)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRtnTrade");
    OnFill(pTrade);	
}

void CUstpFtdcTraderManger::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{//是否使用 待定
    LOG_INFO("CUstpFtdcTraderManger::OnErrRtnOrderInsert");
}

void CUstpFtdcTraderManger::OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspOrderAction");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );	
    OnActionOrder(pOrderAction,pRspInfo);	
}

void CUstpFtdcTraderManger::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
    LOG_INFO("CUstpFtdcTraderManger::OnErrRtnOrderAction");
}

void CUstpFtdcTraderManger::OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspQryOrder");
    if(m_InitQueryNum > 0)
    {
        m_InitQueryNum--;
    }
    OnQryOrder(pOrder);
	
}

///成交单查询应答
void CUstpFtdcTraderManger::OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspQryTrade");
    OnQryTrade(pTrade,pRspInfo,nRequestID,bIsLast);	
}
///投资者持仓查询应答
void CUstpFtdcTraderManger::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspQryInvestorPosition");
    OnQryInvestorPosition(pRspInvestorPosition,pRspInfo,nRequestID,bIsLast);	
}


void CUstpFtdcTraderManger::OnQryOrder(CUstpFtdcOrderField *pOrder)
{
    LOG_INFO("CUstpFtdcTraderManger::OnQryOrder");
    if(NULL != m_strategy)
    {
        //make Order begin (have something todo)
        ::pb::ems::Order tmporder;  
        //tmporder.set_client_order_id(pOrder->UserOrderLocalID);
        
        std::string tmpc_OrderId = GetOrderId(std::atoi(pOrder->UserOrderLocalID));
        if(tmpc_OrderId == "")
	 {
	     //UserOrderLocalID|OrderUserID|OrderLocalID
            tmpc_OrderId = pOrder->UserOrderLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrder->OrderUserID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrder->OrderLocalID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);	
	 //tmporder.set_client_order_id(GetOrderId(std::atoi(pOrder->UserOrderLocalID)));	
        tmporder.set_account(pOrder->UserID);
        tmporder.set_contract(pOrder->InstrumentID);
	 if(pOrder->Direction == '0')
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
        tmporder.set_price(std::to_string(pOrder->LimitPrice));
        tmporder.set_quantity(pOrder->Volume);
        tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
	 if(pOrder->OrderPriceType == '2')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
	 }
	 else
	 //if(pOrder->OrderPriceType == '1')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Market);
	 }	     		
        tmporder.set_exchange_order_id(pOrder->OrderSysID);

	if(pOrder->OrderStatus == '0')//OS_Filled
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	 }
	 else
	 if(pOrder->OrderStatus == '1')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pOrder->OrderStatus == '2')
	 {
	     //something fill is fill ?
            tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	 }
	 else
	 if(pOrder->OrderStatus == '3')
	 {
           tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pOrder->OrderStatus == '4')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected); 
	 }
	 else
	 if(pOrder->OrderStatus == '5')//OS_Cancelled
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	 }
	 else
	 if(pOrder->OrderStatus == '6')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
	 else
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_None);
	 } 	
		
        tmporder.set_working_price(std::to_string(pOrder->LimitPrice));
	 tmporder.set_working_quantity(pOrder->VolumeRemain);
        tmporder.set_filled_quantity(pOrder->VolumeTraded);
		
	 std::string tmpActionDay = pOrder->ActionDay;	
	 std::string tmpActiontime = pOrder->InsertTime;
	 std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);
	 //end	
        //print message
        LOG_INFO("client_order_id:",pOrder->UserOrderLocalID);
	 LOG_INFO("contract:",pOrder->InstrumentID);
	 LOG_INFO("Direction:",pOrder->Direction);
	 LOG_INFO("OrderPriceType:",pOrder->OrderPriceType);
	 LOG_INFO("account:",pOrder->UserID);
	 LOG_INFO("LimitPrice:",pOrder->LimitPrice);
	 LOG_INFO("exchange_order_id:",pOrder->OrderSysID);
	 LOG_INFO("ActionDay:",pOrder->ActionDay);
	 //send message			
	 m_strategy->OnOrder(tmporder);		
    }
}

void CUstpFtdcTraderManger::OnActionOrder(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
    LOG_INFO("CUstpFtdcTraderManger::OnActionOrder");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;
        //make Order begin (have something todo) 
	 //tmporder.set_client_order_id(pOrderAction->UserOrderActionLocalID);
	 std::string tmpc_OrderId = GetOrderId(std::atoi(pOrderAction->UserOrderActionLocalID));
        if(tmpc_OrderId == "")
	 {
	     //UserOrderLocalID|UserOrderActionLocalID|OrderSysID            
	     tmpc_OrderId = pOrderAction->UserOrderLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrderAction->UserOrderActionLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrderAction->OrderSysID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);	
        //tmporder.set_client_order_id(GetOrderId(std::atoi(pOrderAction->UserOrderActionLocalID)));
		
        tmporder.set_account(pOrderAction->UserID);		
	 tmporder.set_price(std::to_string(pOrderAction->LimitPrice));
        tmporder.set_quantity(pOrderAction->VolumeChange);
	 tmporder.set_exchange_order_id(pOrderAction->OrderSysID);
	 if(pOrderAction->ActionFlag == USTP_FTDC_AF_Delete)
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	 }
	 else
	 if(pOrderAction->ActionFlag == USTP_FTDC_AF_Suspend)
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
	 else
	 if(pOrderAction->ActionFlag == USTP_FTDC_AF_Active)
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pOrderAction->ActionFlag == USTP_FTDC_AF_Modify)
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 //end	
        //print message	
	 LOG_INFO("client_order_id:",pOrderAction->UserOrderActionLocalID);
	 LOG_INFO("account:",pOrderAction->UserID);
	 LOG_INFO("LimitPrice:",pOrderAction->LimitPrice);
	 LOG_INFO("VolumeChange:",pOrderAction->VolumeChange);
	 LOG_INFO("UserOrderLocalID:",pOrderAction->UserOrderLocalID);
        //send message
	 m_strategy->OnOrder(tmporder);	
    }
}

void CUstpFtdcTraderManger::OnInsertOrder(CUstpFtdcInputOrderField  *pInputOrder,CUstpFtdcRspInfoField  *pRspInfo)
{
    LOG_INFO("CUstpFtdcTraderManger::OnInsertOrder");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;	
	 //make Order begin (have something todo) 	
	 //tmporder.set_client_order_id(pInputOrder->UserOrderLocalID);
	 std::string tmpc_OrderId = GetOrderId(std::atoi(pInputOrder->UserOrderLocalID));
        if(tmpc_OrderId == "")
	 {
	     //UserOrderLocalID|OrderSysID
	     tmpc_OrderId = pInputOrder->UserOrderLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pInputOrder->OrderSysID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
        //tmporder.set_client_order_id(GetOrderId(std::atoi(pInputOrder->UserOrderLocalID)));
	 
        tmporder.set_account(pInputOrder->UserID);
        tmporder.set_contract(pInputOrder->InstrumentID);
	 if(pInputOrder->Direction == '0')
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
        tmporder.set_price(std::to_string(pInputOrder->LimitPrice));
        tmporder.set_quantity(pInputOrder->Volume);
        tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
	 if(pInputOrder->OrderPriceType == '2')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
	 }
	 else
	 if(pInputOrder->OrderPriceType == '1')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Market);
	 }	       		
        tmporder.set_exchange_order_id(pInputOrder->OrderSysID);
	 if(pRspInfo->ErrorID != 0)	
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
	 }	
	 std::string tmpActionDay = pInputOrder->ActionDay;	
	 std::string tmpActiontime = "00:00:00";
	 std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);	
        //end	
        //print message
        LOG_INFO("client_order_id:",pInputOrder->OrderSysID);
	 LOG_INFO("account:",pInputOrder->UserID);
	 LOG_INFO("contract:",pInputOrder->InstrumentID);
	 LOG_INFO("buy_sell:",pInputOrder->Direction);
	 LOG_INFO("price:",pInputOrder->LimitPrice);
	 LOG_INFO("quantity:",pInputOrder->Volume); 
	 LOG_INFO("tif:",pb::ems::TimeInForce::TIF_GFD);
	 LOG_INFO("order_type:",pInputOrder->OrderPriceType);
	 LOG_INFO("exchange_order_id:",pInputOrder->OrderSysID);
	 LOG_INFO("submit_time:",tmpActionDay);
        //send message to strategy
	 m_strategy->OnOrder(tmporder);
    }	
}
void CUstpFtdcTraderManger::OnOrder(CUstpFtdcOrderField  *pOrder)
{
    LOG_INFO("CUstpFtdcTraderManger::OnOrder");

    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;
        //make Order begin (have something todo) 	
        //tmporder.set_client_order_id(pOrder->OrderUserID);
        std::string tmpc_OrderId = GetOrderId(std::atoi(pOrder->OrderUserID));
        if(tmpc_OrderId == "")
	 {
	     //UserOrderLocalID|OrderSysID|OrderUserID
	     tmpc_OrderId = pOrder->UserOrderLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrder->OrderSysID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pOrder->OrderUserID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
        //tmporder.set_client_order_id(GetOrderId(std::atoi(pOrder->OrderUserID)));

		
        tmporder.set_account(pOrder->UserID);
        tmporder.set_contract(pOrder->InstrumentID);
	 if(pOrder->Direction == '0')
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
        tmporder.set_price(std::to_string(pOrder->LimitPrice));
        tmporder.set_quantity(pOrder->Volume);
        tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
	 if(pOrder->OrderPriceType == '2')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
	 }
	 else
	 if(pOrder->OrderPriceType == '1')
	 {
            tmporder.set_order_type(pb::ems::OrderType::OT_Market);
	 }	        		
        tmporder.set_exchange_order_id(pOrder->OrderSysID);
        if(pOrder->OrderStatus == '0')//OS_Filled
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	 }
	 else
	 if(pOrder->OrderStatus == '1')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pOrder->OrderStatus == '2')
	 {
	     //something fill is fill ?
            tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	 }
	 else
	 if(pOrder->OrderStatus == '3')
	 {
           tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	 }
	 else
	 if(pOrder->OrderStatus == '4')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Rejected); 
	 }
	 else
	 if(pOrder->OrderStatus == '5')//OS_Cancelled
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	 }
	 else
	 if(pOrder->OrderStatus == '6')
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	 }
	 else
	 {
            tmporder.set_status(pb::ems::OrderStatus::OS_None);
	 }

	 tmporder.set_working_price(std::to_string(pOrder->LimitPrice));
	 tmporder.set_working_quantity(pOrder->VolumeRemain);
        tmporder.set_filled_quantity(pOrder->VolumeTraded);
   
        //tmporder.set_message(report.single_report.text);
	 std::string tmpActionDay = pOrder->ActionDay;	
	 std::string tmpActiontime = pOrder->InsertTime;
	 std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);	
        //end	
        //print message
        LOG_INFO("client_order_id:",pOrder->OrderUserID);
	 LOG_INFO("account:",pOrder->UserID);
	 LOG_INFO("contract:",pOrder->InstrumentID);
	 LOG_INFO("buy_sell:",pOrder->Direction);
	 LOG_INFO("price:",pOrder->LimitPrice);
	 LOG_INFO("quantity:",pOrder->Volume); 
	 LOG_INFO("tif:",pb::ems::TimeInForce::TIF_GFD);
	 LOG_INFO("order_type:",pOrder->OrderPriceType);
	 LOG_INFO("exchange_order_id:",pOrder->OrderSysID);
	 LOG_INFO("submit_time:",tmpActionDay); 
	 //send message to strategy	
	 m_strategy->OnOrder(tmporder);
    }
}
void CUstpFtdcTraderManger::OnFill(CUstpFtdcTradeField *pTrade)
{
    LOG_INFO("CUstpFtdcTraderManger::OnFill");

    if(NULL != m_strategy)
    {
        ::pb::ems::Fill tmpfill;
        //make Fill begin (have something todo) 
        tmpfill.set_fill_id(pTrade->TradeID);
        tmpfill.set_account(pTrade->UserID);
        tmpfill.set_contract(pTrade->InstrumentID);
	 if(pTrade->Direction == '0')
	 {
            tmpfill.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmpfill.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }
        tmpfill.set_fill_price(std::to_string(pTrade->TradePrice));
        tmpfill.set_fill_quantity(pTrade->TradeVolume);		
        //tmpfill.set_client_order_id(pTrade->OrderUserID);
	 //tmpfill.set_client_order_id(pTrade->UserOrderLocalID);
	 std::string tmpc_OrderId = GetOrderId(std::atoi(pTrade->UserOrderLocalID));
        if(tmpc_OrderId == "")
	 {
	     //OrderUserID|OrderSysID|UserOrderLocalID
	     tmpc_OrderId = pTrade->OrderUserID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pTrade->OrderSysID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pTrade->UserOrderLocalID;	 
	 }
	 tmpfill.set_client_order_id(tmpc_OrderId);
        //tmpfill.set_client_order_id(GetOrderId(std::atoi(pTrade->UserOrderLocalID)));
		
        tmpfill.set_exchange_order_id(pTrade->OrderSysID);		

	 std::string tmpActionDay = pTrade->ActionDay;	
	 std::string tmpActiontime = pTrade->TradeTime;
	 std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
        fh::core::assist::utility::To_pb_time(tmpfill.mutable_fill_time(), tmpalltime);
        //end	
        //print message
        LOG_INFO("fill_id:",pTrade->TradeID);
	 LOG_INFO("account:",pTrade->UserID);
	 LOG_INFO("contract:",pTrade->InstrumentID);
	 LOG_INFO("buy_sell:",pTrade->Direction);
	 LOG_INFO("price:",pTrade->TradePrice);
	 LOG_INFO("quantity:",pTrade->TradeVolume); 
	 LOG_INFO("client_order_id:",pTrade->OrderUserID);
	 LOG_INFO("exchange_order_id:",pTrade->OrderSysID);
	 LOG_INFO("TradeTime:",pTrade->TradeTime);
	 //send message to strategy		
	 m_strategy->OnFill(tmpfill);
    }	
}

void CUstpFtdcTraderManger::OnQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnQryTrade");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;

        std::string tmpc_OrderId = GetOrderId(std::atoi(pTrade->UserOrderLocalID));
        if(tmpc_OrderId == "")
	 {
	     //UserOrderLocalID|OrderSysID|OrderUserID
	     tmpc_OrderId = pTrade->UserOrderLocalID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pTrade->OrderSysID;
	     tmpc_OrderId = tmpc_OrderId + "|";
	     tmpc_OrderId = tmpc_OrderId + pTrade->OrderUserID;	 
	 }
	 tmporder.set_client_order_id(tmpc_OrderId);
	 //tmporder.set_client_order_id(GetOrderId(std::atoi(pTrade->UserOrderLocalID)));	

	 tmporder.set_account(pTrade->UserID);
        tmporder.set_contract(pTrade->InstrumentID);
	 if(pTrade->Direction == '0')
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
	 }
	 else
	 {
            tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
	 }	
	 tmporder.set_price(std::to_string(pTrade->TradePrice));
        tmporder.set_quantity(pTrade->TradeVolume);
        tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);

	 	     		
        tmporder.set_exchange_order_id(pTrade->OrderSysID);

	 tmporder.set_status(pb::ems::OrderStatus::OS_Filled);	

	 tmporder.set_working_price(std::to_string(pTrade->TradePrice));
	 tmporder.set_working_quantity(0);
        tmporder.set_filled_quantity(pTrade->TradeVolume);
		
	 std::string tmpActionDay = pTrade->ActionDay;	
	 std::string tmpActiontime = pTrade->TradeTime;
	 std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
        fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);

	 m_strategy->OnOrder(tmporder);	
    }
	
    if(bIsLast)
    {
        m_startfinish =  bIsLast;
    }	
    return;	
}

void CUstpFtdcTraderManger::OnQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnQryInvestorPosition");
    if(NULL != m_strategy)
    {
        ::pb::ems::Position tmpPosition;
	 core::exchange::PositionVec tmpPositionVec;
	 tmpPositionVec.clear();
		
        tmpPosition.set_account(pRspInvestorPosition->InvestorID);
        tmpPosition.set_contract(pRspInvestorPosition->InstrumentID);
        tmpPosition.set_position(pRspInvestorPosition->Position);

        if(pRspInvestorPosition->Direction == '0')
	 {
            tmpPosition.set_today_long(pRspInvestorPosition->Position);
	 }
	 else
	 if(pRspInvestorPosition->Direction == '1')	
	 {
            tmpPosition.set_today_short(pRspInvestorPosition->Position);
	 }

        tmpPositionVec.push_back(tmpPosition);
	 	

	 m_strategy->OnPosition(tmpPositionVec);	
    }
	
    if(bIsLast)
    {
        m_startfinish =  bIsLast;
    }	
    return;	
}

void CUstpFtdcTraderManger::SetFileConfigData(const std::string &FileConfig)
{
    LOG_INFO("CUstpFtdcTraderManger::SetFileConfigData file =  ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
}

void CUstpFtdcTraderManger::AddOrderId(std::string cl_orderid,int i_key)
{
    if(i_key != -1)
    {
        if(m_ordermap.count(i_key) == 0)
	 {
            m_ordermap[i_key] = cl_orderid;
	 }
	 else
	 if(m_ordermap[i_key] != cl_orderid)
	 {
            LOG_ERROR("CUstpFtdcTraderManger::AddOrderId error");    
	 }
    }
    else
    {
        if(m_ordermap.count(MaxOrderLocalID) == 0)
	 {
            m_ordermap[MaxOrderLocalID] = cl_orderid;
	 }
	 else
	 if(m_ordermap[MaxOrderLocalID] != cl_orderid)	
	 {
            LOG_ERROR("CUstpFtdcTraderManger::AddOrderId error MaxOrderLocalID");
	 }
    }
    return;	
}

std::string CUstpFtdcTraderManger::GetOrderId(int i_key)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////CFemasGlobexCommunicator//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFemasGlobexCommunicator::CFemasGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy)
{

     m_pFileConfig = new fh::core::assist::Settings(config_file);
     m_pUserApi = CUstpFtdcTraderApi::CreateFtdcTraderApi();	 
     m_pUstpFtdcTraderManger = new CUstpFtdcTraderManger(m_pUserApi);
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

CFemasGlobexCommunicator::~CFemasGlobexCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     m_pUserApi->Release();	 
}

bool CFemasGlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
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

void CFemasGlobexCommunicator::Stop()
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


void CFemasGlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // make Initialize	
         LOG_INFO("CFemasGlobexCommunicator::Initialize ");	
         return;	
}

void CFemasGlobexCommunicator::Add(const ::pb::ems::Order& order)
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

void CFemasGlobexCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Change ");		
        return;
}

void CFemasGlobexCommunicator::Delete(const ::pb::ems::Order& order)
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

void CFemasGlobexCommunicator::Query(const ::pb::ems::Order& order)
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

void CFemasGlobexCommunicator::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasGlobexCommunicator::Query_mass ");
        return;
}

void CFemasGlobexCommunicator::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasGlobexCommunicator::Delete_mass ");
        return;
}

bool CFemasGlobexCommunicator::SendReqQryTrade(const std::vector<::pb::ems::Order> &init_orders)
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

bool CFemasGlobexCommunicator::SendReqQryInvestorPosition(const std::vector<::pb::ems::Order> &init_orders)
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