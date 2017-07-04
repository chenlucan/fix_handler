#include <unistd.h>
#include <time.h>
#include <boost/container/flat_map.hpp>
#include "ctp_trader_spi.h"
#include "core/assist/logger.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
//成交单查询请求
void CCtpTraderSpi::reqQryTrade(const std::vector<::pb::ems::Order> &orders)
{
	LOG_INFO("CCtpTraderSpi::reqQryOrder");
     
	for(auto& it : orders) 
	{
		std::string s = it.client_order_id();
		auto find_item = std::find_if(client_orders_index.begin(), client_orders_index.end(),
			[s](const std::map<int, std::string>::value_type item)
		{
			return item.second == s;
		});

		if (find_item!= client_orders_index.end())
		{
			CThostFtdcQryTradeField *orderField = new CThostFtdcQryTradeField();

			strcpy(orderField->BrokerID, id->getBrokerID().c_str());
			strcpy(orderField->InvestorID, id->getInvestorID().c_str());
			strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	
			strcpy(orderField->InstrumentID , it.contract().c_str());  //合约代码
			std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ReqQryTradeCommand>(api, orderField, requestID);
			commandQueue.addCommand(command);	
		}	
		else
		{
			LOG_INFO("之前没有提交过此报单！！！");
			continue;
		}	
	}		
}
	
//查询合约请求 SendReqQryInstrument
void CCtpTraderSpi::reqQueryInstrument()
{
	LOG_INFO("\n\nCCtpTraderSpi::reqQueryInstrument\n\n");
	CThostFtdcQryInstrumentField *orderField = new CThostFtdcQryInstrumentField();	//如果查询参数为空，说明需要查询所有数据。如果需要查询某个交易所的合约，就在查询参数中指定ExchangeID。
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ReqQryInstrumentCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);
}

//解析报单并发出交易信号 add
void CCtpTraderSpi::reqOrderInsert(const ::pb::ems::Order& order)
{	
    LOG_INFO("CCtpTraderSpi::reqOrderInsert");
	CThostFtdcInputOrderField *orderField = new CThostFtdcInputOrderField();
	strcpy(orderField->BrokerID, id->getBrokerID().c_str());
	strcpy(orderField->InvestorID, id->getInvestorID().c_str());
	std::string InstrumentID = order.contract();
	strcpy(orderField->InstrumentID, InstrumentID.c_str());
	itoa(maxOrderRef, orderField->OrderRef, 10);
	client_orders_index[maxOrderRef] = order.client_order_id();
	increaseRef();	//自增orderRef
	//报单价格条件 对不同类型的报单(限价单/市价单/条件单)，需要对相应的字段进行相应特别的赋值
	if(order.order_type() == pb::ems::OrderType::OT_Limit)   
	{
		orderField->OrderPriceType = THOST_FTDC_OPT_LimitPrice;  //限价
		orderField->LimitPrice = atof(order.price().c_str());		
	}
	else
	{
			orderField->OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //市价
			orderField->LimitPrice = 0;                           //价格
	}
	//买卖方向: 
	pb::ems::BuySell BuySellval = order.buy_sell(); 
	if(BuySellval == 1)
	{
		orderField->Direction = THOST_FTDC_D_Buy; //买
	}
	else
	{
		orderField->Direction = THOST_FTDC_D_Sell; //卖
	}
	
//	if (order->getOpenCloseFlag() == '0'){ 此处应该在Order中增加字段
//		orderField->CombOffsetFlag[0] = THOST_FTDC_OF_Open;				//开仓
//	}
//	else{
		orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;		//测试环境都用平今
//	}
	//if (order->getOpenCloseFlag() == '1'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_Close;				//平仓
	//}
	//if (order->getOpenCloseFlag() == '3'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;		//平今
	//}
	//if (order->getOpenCloseFlag() == '4'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;	//平昨
	//}
	//orderField->VolumeTotalOriginal = order.getOriginalVolume();		//数量
	//以下是固定的字段
	orderField->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		//投机 
	orderField->TimeCondition = THOST_FTDC_TC_GFD;				//当日有效 '3'
	orderField->VolumeCondition = THOST_FTDC_VC_AV;				//任何数量 '1'
	orderField->MinVolume = 1;
	orderField->ContingentCondition = THOST_FTDC_CC_Immediately;	//立即触发'1'
	orderField->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//非强平 '0'
	orderField->IsAutoSuspend = 1;
	orderField->UserForceClose = 0;
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::InsertOrderCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);	
}	

//发送报单查询请求 Query
void CCtpTraderSpi::reqQryOrder(const ::pb::ems::Order& order)
{
	LOG_INFO("CCtpTraderSpi::reqQryOrder");

	std::string s = order.client_order_id();
    auto find_item = std::find_if(client_orders_index.begin(), client_orders_index.end(),
        [s](const std::map<int, std::string>::value_type item)
    {
        return item.second == s;
    });

  //  int orderRef = 0;
    if (find_item!= client_orders_index.end())
    {
  //      orderRef = (*find_item).first;
		CThostFtdcQryOrderField *orderField = new CThostFtdcQryOrderField();

		strcpy(orderField->BrokerID, id->getBrokerID().c_str());
		strcpy(orderField->InvestorID, id->getInvestorID().c_str());
		strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	//测试环境下全都是上期所
		strcpy(orderField->InstrumentID , order.contract().c_str());  //合约代码
		///报单编号orderField->OrderSysID需要赋值 目前order里面没有
		std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::QueryOrderCommand>(api, orderField, requestID);
		commandQueue.addCommand(command);
    }	
	else
	{
		LOG_INFO("之前没有提交过此报单！！！");
		return;
	}	


}	

//撤单	cancle Order
void CCtpTraderSpi::reqOrderAction(const ::pb::ems::Order& order, TThostFtdcActionFlagType ActionFlag)
{    
	//交易所代码
	//const std::string &exchangeID = "SHFE";
	//设置撤单信息
	CThostFtdcInputOrderActionField *orderField = new CThostFtdcInputOrderActionField();
	strcpy(orderField->BrokerID, id->getBrokerID().c_str());
	strcpy(orderField->InvestorID, id->getInvestorID().c_str());
	//strcpy(orderField->ExchangeID, exchangeID.c_str());
	strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	//测试环境下全都是上期所
//	strcpy(orderField->OrderSysID, order.getSystemId().c_str());

	std::string s = order.client_order_id();
    auto find_item = std::find_if(client_orders_index.begin(), client_orders_index.end(),
        [s](const std::map<int, std::string>::value_type item)
    {
        return item.second == s;
    });

    int orderRef = 0;
    if (find_item!= client_orders_index.end())
    {
        orderRef = (*find_item).first;
    }	
	else
	{
		LOG_INFO("之前没有提交过此报单！！！");
		return;
	}
	
	itoa(orderRef, orderField->OrderRef, 10);
	orderField->ActionFlag = THOST_FTDC_AF_Delete;	//删除报单 '0'
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::WithdrawOrderCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);
}
	
//查询客户总体持仓情况 SendReqQryInvestorPosition
void CCtpTraderSpi::queryPosition(const std::vector<::pb::ems::Order> &init_orders) {
	for(auto& it : init_orders) 
	{  
	CThostFtdcQryInvestorPositionField *accountField = new CThostFtdcQryInvestorPositionField();
	strcpy(accountField->BrokerID, id->getBrokerID().c_str());
	strcpy(accountField->InvestorID, id->getInvestorID().c_str());
	strcpy(accountField->InstrumentID, it.contract().c_str());
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::QueryPositionCommand>(api, accountField, requestID);
	commandQueue.addCommand(command);
	}
}	
/****************************Api交易函数****************************************/
//登录
void CCtpTraderSpi::login(){
	LOG_INFO("CCtpTraderSpi::login()>>>>>>>>>>start login... ");
	CThostFtdcReqUserLoginField *loginField = new CThostFtdcReqUserLoginField();
	strcpy(loginField->BrokerID, id->getBrokerID().c_str()); //经纪公司代码
	strcpy(loginField->UserID, id->getUserID().c_str()); //用户代码
	strcpy(loginField->Password, id->getPassword().c_str());
	//把指令放到队列尾部,下面各条指令的执行方法类似
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::LoginCommand>(api, loginField, requestID);
	commandQueue.addCommand(command);
}	

//确认计算
void CCtpTraderSpi::comfirmSettlement(){
	CThostFtdcSettlementInfoConfirmField *comfirmField = new CThostFtdcSettlementInfoConfirmField();
	strcpy(comfirmField->BrokerID, id->getBrokerID().c_str());
	strcpy(comfirmField->InvestorID, id->getUserID().c_str());
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ComfirmSettlementCommand>(api, comfirmField, requestID);
	commandQueue.addCommand(command);
}

//退出
void CCtpTraderSpi::reqUserLogout()
{
	CThostFtdcUserLogoutField *loginField = new CThostFtdcUserLogoutField();
	strcpy(loginField->BrokerID, id->getBrokerID().c_str());
	strcpy(loginField->UserID, id->getUserID().c_str());
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::LoginOutCommand>(api, loginField, requestID);
	commandQueue.addCommand(command);
}
/****************************Api交易函数****************************************/
	
/****************************Spi回调函数****************************************/
///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CCtpTraderSpi::OnFrontConnected(){
	login();
}	

void CCtpTraderSpi::OnFrontDisconnected(int nReason)
{
    // 当发生这个情况后，API会自动重新连接，客户端可不做处理
    LOG_INFO("CCtpTraderSpi::OnFrontDisconnected nReason = ", nReason);
}

void CCtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG_INFO("--->>> OnRspError");
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		LOG_ERROR("--->>> ErrorID=", pRspInfo->ErrorID, ", ErrorMsg=", pRspInfo->ErrorMsg);
}
	
///登录请求响应
void CCtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		maxOrderRef = atoi(pRspUserLogin->MaxOrderRef); 
		++maxOrderRef;      //初始化最大报单引用(本地)
		comfirmSettlement();	//确认结算结果
	}
	else
	{
		LOG_INFO("用户: ", pRspUserLogin->UserID, "登录失败");
	}
}

//投资者结算结果确认响应
void CCtpTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		tradable = true;
		LOG_INFO("\n\naccount:", pSettlementInfoConfirm->InvestorID, " Has been able to trade.\n");
		// 请求查询合约
		reqQueryInstrument();	
	}
	else
	{
		LOG_INFO("account:", pSettlementInfoConfirm->InvestorID, "Confirm the settlement failed");
	}
}

///报单录入请求响应(参数不通过)
void CCtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG_INFO("报单录入请求响应(参数不通过)", pRspInfo->ErrorID);
    OnInsertOrder(pInputOrder, pRspInfo);
}

///撤单操作请求响应(参数不通过)
void CCtpTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	LOG_INFO("撤单操作请求响应(参数不通过)");
	OnActionOrder(pInputOrderAction,pRspInfo);
}
	
///报单通知
void CCtpTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	LOG_INFO("报单回报\n", 
	    "交易所编号:", pOrder->ExchangeID,
		" 合约代码:", pOrder->InstrumentID,
		" 报单引用:", pOrder->OrderRef,
		" 买卖方向:", pOrder->Direction,
		" 组合开平标志:", pOrder->CombOffsetFlag,
		" 价格:", pOrder->LimitPrice,
		" 数量:", pOrder->VolumeTotalOriginal,
		" 今成交数量:", pOrder->VolumeTraded,
		" 剩余数量:", pOrder->VolumeTotal,
		" 报单编号（判断报单是否有效）:", pOrder->OrderSysID,
		" 报单状态:", pOrder->OrderStatus,
		" 报单日期:", pOrder->InsertDate,
		" 序号:", pOrder->SequenceNo);	

	OnOrder(pOrder);

}

///成交回报
void CCtpTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	LOG_INFO("CCtpTraderSpi::OnRtnTrade\n=====Trade successful=====",
	"\nTradeTime: ",pTrade->TradeTime,
	"\nInstrumentID: ", pTrade->InstrumentID,
	"\nPrice: ", pTrade->Price,
	"\nVolume: ", pTrade->Volume,
	"\nDirection: ", pTrade->Direction);
	
	OnFill(pTrade);
}	

///请求查询投资者持仓响应
void CCtpTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	LOG_INFO("请求查询投资者", id->getInvestorID(), "持仓响应");
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		OnQryInvestorPosition(pInvestorPosition,pRspInfo,nRequestID,bIsLast);
	}
}

//报单录入错误回报
void CCtpTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    LOG_INFO("CCtpTraderSpi::OnErrRtnOrderInsert");
	printf("%s\n", pRspInfo->ErrorMsg);
    if(NULL == pInputOrder || NULL == pRspInfo)
    {
        LOG_ERROR("CCtpTraderSpi::OnErrRtnOrderInsert Error");
	    return ;	
    }		
}

void CCtpTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    LOG_INFO("CCtpTraderSpi::OnErrRtnOrderAction");
    if(NULL == pOrderAction || NULL == pRspInfo)
    {
        LOG_ERROR("CCtpTraderSpi::OnErrRtnOrderAction Error");
	 return ;	
    }		
}

//查询合约应答
void CCtpTraderSpi::OnRspQryInstrument(
	CThostFtdcInstrumentField *pInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	char filePath[100] = "InstrumentID.csv";
	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); // 文件追加写入 
	outFile << pInstrument->InstrumentID << std::endl;
	outFile.close();	
	OnQryInstrument(pInstrument);
}
/****************************Spi回调函数****************************************/
	
/****************************辅助函数******************************************/	
//与交易所建立连接，进入准备交易的状态(非本地)
void CCtpTraderSpi::readyToTrade(){
	LOG_INFO("账户即将登陆...");
	api = CThostFtdcTraderApi::CreateFtdcTraderApi();
	api->RegisterSpi(this);
	//注册前置机
	char *frontAddress = new char[100];
	strcpy(frontAddress, id->getExchangeFrontAddress().c_str());
	api->RegisterFront(frontAddress);	
	//订阅共有流、私有流
	api->SubscribePublicTopic(THOST_TERT_RESTART); //从当天的第一条记录开始接收数据流
	api->SubscribePrivateTopic(THOST_TERT_RESTART);
    //开启请求队列
	commandQueue.run();	
    api->Init(); 	
}	

//报单录入应答数据发送给策略端
void CCtpTraderSpi::OnInsertOrder(CThostFtdcInputOrderField  *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	LOG_INFO("--->>> ",  __FUNCTION__);
	if(NULL != m_strategy)
	{
		::pb::ems::Order tmporder;
		std::string tmpc_OrderId;
		
		if (client_orders_index.find(std::atoi(pInputOrder->OrderRef)) == client_orders_index.end()){
			tmpc_OrderId = pInputOrder->OrderRef; 
		}		
		else{
		    tmpc_OrderId = client_orders_index[std::atoi(pInputOrder->OrderRef)];
		}
		tmporder.set_client_order_id(tmpc_OrderId);
		tmporder.set_account(pInputOrder->InvestorID);
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
		tmporder.set_quantity(pInputOrder->VolumeTotalOriginal);
		tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);

		if(pInputOrder->OrderPriceType == '2')
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
		}
		else
		{
			if(pInputOrder->OrderPriceType == '1')
			{
			tmporder.set_order_type(pb::ems::OrderType::OT_Market);
			}	       		
        }
		
		if(pRspInfo->ErrorID != 0)	
		{
			tmporder.set_status(pb::ems::OrderStatus::OS_Rejected);
		}			
		m_strategy->OnOrder(tmporder);
	}	
}	

//提交报单操作的输入数据发送给策略端
void CCtpTraderSpi::OnActionOrder(CThostFtdcInputOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{	
	LOG_INFO("CCtpTraderSpi::OnActionOrder");
	if(NULL != m_strategy)
	{
		::pb::ems::Order tmporder;
		std::string tmpc_OrderId;
		
		if (client_orders_index.find(std::atoi(pOrderAction->OrderRef)) == client_orders_index.end()){
			tmpc_OrderId = pOrderAction->OrderRef; 
			tmpc_OrderId = tmpc_OrderId + "|";
	        tmpc_OrderId = tmpc_OrderId + pOrderAction->OrderSysID;	 
		}		
		else{
		    tmpc_OrderId = client_orders_index[std::atoi(pOrderAction->OrderRef)];
		}
		
		tmporder.set_client_order_id(tmpc_OrderId);	
		tmporder.set_price(std::to_string(pOrderAction->LimitPrice));
		tmporder.set_quantity(pOrderAction->VolumeChange);
		tmporder.set_exchange_order_id(pOrderAction->OrderSysID);
		if(pOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
		}
		else
		if(pOrderAction->ActionFlag == THOST_FTDC_AF_Modify)
		{
			tmporder.set_status(pb::ems::OrderStatus::OS_Working);
		}		

		m_strategy->OnOrder(tmporder);	
	}		
}	

void CCtpTraderSpi::OnOrder(CThostFtdcOrderField  *pOrder)
{
    LOG_INFO("CCtpTraderSpi::OnOrder");

    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;
		std::string tmpc_OrderId;
		
		if (client_orders_index.find(std::atoi(pOrder->OrderRef)) == client_orders_index.end()){
			tmpc_OrderId = pOrder->OrderRef; 
			tmpc_OrderId = tmpc_OrderId + "|";
	        tmpc_OrderId = tmpc_OrderId + pOrder->OrderSysID;	 
		}		
		else{
		    tmpc_OrderId = client_orders_index[std::atoi(pOrder->OrderRef)];
		}
	
		tmporder.set_client_order_id(tmpc_OrderId);
        tmporder.set_account(pOrder->UserID);
        tmporder.set_contract(pOrder->InstrumentID);
		
		//买卖方向  THOST_FTDC_D_Buy '0'(买)  THOST_FTDC_D_Sell '1'(卖)
		if(pOrder->Direction == THOST_FTDC_D_Buy)
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
		}
		else
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
		}
        tmporder.set_price(std::to_string(pOrder->LimitPrice));
        tmporder.set_quantity(pOrder->VolumeTotalOriginal);  //该报单的原始报单数量
		/*
			IOC '1' 立即完成，否则撤销          
			GFS '2' 本节有效
			GFD '3'当日有效
			GTD '4'指定日期前有效
			GTC '5'撤销前有效
			GFA '6'集合竞价有效
		*/
		char str[10];
		sprintf(str, "%d",pOrder->TimeCondition); 
		int TimeCondition= atoi(str)-48;	   
	    tmporder.set_tif((::pb::ems::TimeInForce)TimeCondition);
		
		//报单价格条件
		if(pOrder->OrderPriceType == '2')  //限价
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
		}
		else
		if(pOrder->OrderPriceType == '1')  //任意价
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Market);
		}	        		
        tmporder.set_exchange_order_id(pOrder->OrderSysID);
        SetOrderStatus(pOrder,tmporder);


		tmporder.set_working_price(std::to_string(pOrder->LimitPrice)); //价格
		tmporder.set_working_quantity(pOrder->VolumeTotal);  //该报单的剩余数量
		tmporder.set_filled_quantity(pOrder->VolumeTraded);  //该报单的已成交数量

		std::string tmpActionDay = pOrder->TradingDay;	//交易日
		std::string tmpActiontime = pOrder->InsertTime; //委托时间
		std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
		fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);	

	    m_strategy->OnOrder(tmporder);
    }
}

//成交数据发送给策略端
void CCtpTraderSpi::OnFill(CThostFtdcTradeField *pTrade)
{
    LOG_INFO("CCtpTraderSpi::OnFill");

	if(NULL == m_strategy)
		return;

	::pb::ems::Fill tmpfill;
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
	tmpfill.set_fill_price(std::to_string(pTrade->Price));
	tmpfill.set_fill_quantity(pTrade->Volume);		

	std::string tmpc_OrderId;
	
	if (client_orders_index.find(std::atoi(pTrade->OrderRef)) == client_orders_index.end()){
		tmpc_OrderId = pTrade->OrderRef; 
		tmpc_OrderId = tmpc_OrderId + "|";
		tmpc_OrderId = tmpc_OrderId + pTrade->OrderSysID;	 
	}		
	else{
		tmpc_OrderId = client_orders_index[std::atoi(pTrade->OrderRef)];
	}

	if(tmpc_OrderId == "")
	{
		tmpc_OrderId = pTrade->OrderSysID; //报单编号
		tmpc_OrderId = tmpc_OrderId + "|";
		tmpc_OrderId = tmpc_OrderId + pTrade->OrderRef; //本地报单编号
	}
	tmpfill.set_client_order_id(tmpc_OrderId);

	tmpfill.set_exchange_order_id(pTrade->OrderSysID);		

	std::string tmpTradeDay = pTrade->TradeDate;	
	std::string tmpTradeTime = pTrade->TradeTime;
	std::string tmpalltime = tmpTradeDay + "-" + tmpTradeTime + ".000";
	fh::core::assist::utility::To_pb_time(tmpfill.mutable_fill_time(), tmpalltime);
	
	m_strategy->OnFill(tmpfill);
	
}

//报单查询请求。当客户端发出报单查询指令后，交易托管系统返回响应时，该方法会被调用
void CCtpTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnRspQryOrder");
    if(NULL == pOrder || NULL == pRspInfo)
    {
		LOG_ERROR("CCtpTraderSpi::OnRspQryOrder Error");
		return ;	
    }	
	
	if(m_InitQueryNum > 0)
	{
		m_InitQueryNum--;
	}
	OnQryOrder(pOrder);		
}	

//报单响应数据发送给策略端
void CCtpTraderSpi::OnQryOrder(CThostFtdcOrderField *pOrder)
{
    LOG_INFO("CCtpTraderSpi::OnQryOrder");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;  
        
		std::string tmpc_OrderId;
		
		if (client_orders_index.find(std::atoi(pOrder->OrderRef)) == client_orders_index.end()){
			tmpc_OrderId = pOrder->OrderRef; 
			tmpc_OrderId = tmpc_OrderId + "|";
	        tmpc_OrderId = tmpc_OrderId + pOrder->OrderSysID;	 
		}		
		else{
		    tmpc_OrderId = client_orders_index[std::atoi(pOrder->OrderRef)];
		}		

		tmporder.set_client_order_id(tmpc_OrderId);		
        tmporder.set_account(pOrder->UserID); //用户代码
        tmporder.set_contract(pOrder->InstrumentID);	 //合约代码	
		tmporder.set_client_order_id(tmpc_OrderId);		
		tmporder.set_account(pOrder->UserID); //用户代码
		tmporder.set_contract(pOrder->InstrumentID); //合约代码
		
		if(pOrder->Direction == '0') //买卖方向
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
		}
		else
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
		}
		tmporder.set_price(std::to_string(pOrder->LimitPrice)); //价格
		tmporder.set_quantity(pOrder->VolumeTotalOriginal); //数量
		tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
		if(pOrder->OrderPriceType == '2')   //报单价格条件
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
		}
		else
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Market);
		}	     		
		tmporder.set_exchange_order_id(pOrder->OrderSysID); //报单编号

		SetOrderStatus(pOrder,tmporder);

		tmporder.set_working_price(std::to_string(pOrder->LimitPrice));
		tmporder.set_working_quantity(pOrder->VolumeTotal);
		tmporder.set_filled_quantity(pOrder->VolumeTraded);

		std::string tmpActionDay = pOrder->TradingDay;	
		std::string tmpActiontime = pOrder->InsertTime;
		std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
		fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);
		
		m_strategy->OnOrder(tmporder);		
	}
}	


//成交单查询应答。当客户端发出成交单查询指令后，交易托管系统返回响应时，该方法会被调用。 B
void CCtpTraderSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnRspQryTrade");
    if(NULL == pTrade || NULL == pRspInfo)
    {
        LOG_ERROR("CCtpThostFtdcTraderManger::OnRspQryTrade Error");
	    return ;	
    }		
    OnQryTrade(pTrade,pRspInfo,nRequestID,bIsLast);	
    
}
//成交信息结构数据发送给策略端。 B
void CCtpTraderSpi::OnQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnQryTrade");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;

		std::string tmpc_OrderId;
		
		if (client_orders_index.find(std::atoi(pTrade->OrderRef)) == client_orders_index.end()){
			tmpc_OrderId = pTrade->OrderRef; 
			tmpc_OrderId = tmpc_OrderId + "|";
	        tmpc_OrderId = tmpc_OrderId + pTrade->OrderSysID;	 
		}		
		else{
		    tmpc_OrderId = client_orders_index[std::atoi(pTrade->OrderRef)];
		}		
		
		tmporder.set_client_order_id(tmpc_OrderId);
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
		tmporder.set_price(std::to_string(pTrade->Price));
		tmporder.set_quantity(pTrade->Volume);
		tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);	
		tmporder.set_exchange_order_id(pTrade->OrderSysID);
		tmporder.set_status(pb::ems::OrderStatus::OS_Filled);	
		tmporder.set_working_price(std::to_string(pTrade->Price));
		tmporder.set_working_quantity(0);
		tmporder.set_filled_quantity(pTrade->Volume);

		std::string tmpActionDay = pTrade->TradeDate;	
		std::string tmpActiontime = pTrade->TradeTime;
		std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
		fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);

		m_strategy->OnOrder(tmporder);	
	}
	return;	
}

//投资者持仓数据发送给策略端 E
void CCtpTraderSpi::OnQryInvestorPosition(CThostFtdcInvestorPositionField *pRspInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnQryInvestorPosition");
    if(NULL != m_strategy)
    {
		::pb::ems::Position tmpPosition;
		core::exchange::PositionVec tmpPositionVec;
		tmpPositionVec.clear();
		
        tmpPosition.set_account(pRspInvestorPosition->InvestorID);
        tmpPosition.set_contract(pRspInvestorPosition->InstrumentID);
        tmpPosition.set_position(pRspInvestorPosition->Position);

		if(pRspInvestorPosition->PosiDirection == '0')
		{
			tmpPosition.set_today_long(pRspInvestorPosition->Position);
		}
		else
		if(pRspInvestorPosition->PosiDirection == '1')	
		{
			tmpPosition.set_today_short(pRspInvestorPosition->Position);
		}
		tmpPositionVec.push_back(tmpPosition);


		m_strategy->OnPosition(tmpPositionVec);	
	}	
}

void CCtpTraderSpi::OnQryInstrument(CThostFtdcInstrumentField *pRspInstrument)
{
    LOG_INFO("CCtpTraderSpi::OnQryInstrument");
    if(NULL == m_strategy)
    {
         LOG_ERROR("CCtpTraderSpi::OnQryInstrument m_strategy is NULL");
	  return;
    }
    pb::dms::Contract tmpcontract;
    tmpcontract.set_name(pRspInstrument->InstrumentID);
    tmpcontract	.set_tick_size(std::to_string(pRspInstrument->PriceTick));
    tmpcontract	.set_tick_value(std::to_string(pRspInstrument->PriceTick * pRspInstrument->VolumeMultiple));	
    tmpcontract	.set_contract_type(::pb::dms::ContractType::CT_Futures);	
    m_strategy->OnContractDefinition(tmpcontract);

    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("ExchangeID", T(pRspInstrument->ExchangeID)));
    tmjson.append(bsoncxx::builder::basic::kvp("ProductID", T(pRspInstrument->ProductID)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T(pRspInstrument->InstrumentID)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentName", T(pRspInstrument->InstrumentName)));	
    tmjson.append(bsoncxx::builder::basic::kvp("DeliveryYear", T(pRspInstrument->DeliveryYear)));
    tmjson.append(bsoncxx::builder::basic::kvp("DeliveryMonth", T(pRspInstrument->DeliveryMonth)));
    tmjson.append(bsoncxx::builder::basic::kvp("MaxLimitOrderVolume", T(pRspInstrument->MaxLimitOrderVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("MinLimitOrderVolume", T(pRspInstrument->MinLimitOrderVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("MaxMarketOrderVolume", T(pRspInstrument->MaxMarketOrderVolume)));	
    tmjson.append(bsoncxx::builder::basic::kvp("MinMarketOrderVolume", T(pRspInstrument->MinMarketOrderVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("VolumeMultiple", T(pRspInstrument->VolumeMultiple)));
    tmjson.append(bsoncxx::builder::basic::kvp("PriceTick", T(pRspInstrument->PriceTick)));
    tmjson.append(bsoncxx::builder::basic::kvp("CreateDate", T(pRspInstrument->CreateDate)));	
    tmjson.append(bsoncxx::builder::basic::kvp("OpenDate", T(pRspInstrument->OpenDate)));
    tmjson.append(bsoncxx::builder::basic::kvp("ExpireDate", T(pRspInstrument->ExpireDate)));
    tmjson.append(bsoncxx::builder::basic::kvp("StartDelivDate", T(pRspInstrument->StartDelivDate)));
    tmjson.append(bsoncxx::builder::basic::kvp("EndDelivDate", T(pRspInstrument->EndDelivDate)));
    tmjson.append(bsoncxx::builder::basic::kvp("IsTrading", T(pRspInstrument->IsTrading)));	
    tmjson.append(bsoncxx::builder::basic::kvp("UnderlyingInstrID", T(pRspInstrument->UnderlyingInstrID)));
    tmjson.append(bsoncxx::builder::basic::kvp("UnderlyingMultiple", T(pRspInstrument->UnderlyingMultiple)));
    tmjson.append(bsoncxx::builder::basic::kvp("PositionType", T(pRspInstrument->PositionType)));
    tmjson.append(bsoncxx::builder::basic::kvp("StrikePrice", T(pRspInstrument->StrikePrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("OptionsType", T(pRspInstrument->OptionsType)));			
	CtpDateToString(tmjson,pRspInstrument->InstrumentID,pRspInstrument->VolumeMultiple);
}


void CCtpTraderSpi::CtpDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,int VolumeMultiple)
{
    LOG_INFO("CCtpTraderSpi::CtpDateToString");
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("ctp_contract")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T(InstrumentID)));	
    tmjson.append(bsoncxx::builder::basic::kvp("VolumeMultiple", T(VolumeMultiple)));	
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    m_strategy->OnOrginalMessage(bsoncxx::to_json(tmjson.view()));
    return;	
}

void CCtpTraderSpi::Initialize(std::vector<::pb::dms::Contract> contracts)
{
    //m_contracts = contracts;
}

void CCtpTraderSpi::SetOrderStatus(CThostFtdcOrderField *pOrder, ::pb::ems::Order &tmporder)
{
	LOG_INFO("CCtpTraderSpi::SetOrderStatus");
	if(pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)//OS_Filled
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
	{
	 //something fill is fill ?
		tmporder.set_status(pb::ems::OrderStatus::OS_Filled);
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
	{
	   tmporder.set_status(pb::ems::OrderStatus::OS_Working);
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing)
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_Rejected); 
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled)//撤单
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_Cancelled);
	}
	else
	if(pOrder->OrderStatus == THOST_FTDC_OST_Unknown)
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_Pending);
	}
	else
	{
		tmporder.set_status(pb::ems::OrderStatus::OS_None);
	}	
}

char* CCtpTraderSpi::itoa(int val, char *buf, unsigned radix)
{
    char   *p;             
    char   *firstdig;      
    char   temp;           
    unsigned   digval;     
    p = buf;
    if(val <0)
    {
        *p++ = '-';
        val = (unsigned long)(-(long)val);
    }
    firstdig = p; 
    do{
        digval = (unsigned)(val % radix);
        val /= radix;
       
        if  (digval > 9)
            *p++ = (char)(digval - 10 + 'a'); 
        else
            *p++ = (char)(digval + '0');      
    }while(val > 0);
   
    *p-- = 0;         
    do{
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;        
    }while(firstdig < p);  
    return buf;
}

}
}
}
