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
//�ɽ�����ѯ����
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

		//int orderRef = 0;
		if (find_item!= client_orders_index.end())
		{
			//orderRef = (*find_item).first;
			CThostFtdcQryTradeField *orderField = new CThostFtdcQryTradeField();

			strcpy(orderField->BrokerID, id->getBrokerID().c_str());
			strcpy(orderField->InvestorID, id->getInvestorID().c_str());
			strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	//���Ի�����ȫ����������
			strcpy(orderField->InstrumentID , it.contract().c_str());  //��Լ����
			std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ReqQryTradeCommand>(api, orderField, requestID);
			commandQueue.addCommand(command);	
		}	
		else
		{
			std::cout << "֮ǰû���ύ���˱���������" << std::endl;
			continue;
		}	
	}		
}
	
//��ѯ��Լ���� SendReqQryInstrument
void CCtpTraderSpi::reqQueryInstrument()
{
	std::cout << "\n\nCCtpTraderSpi::reqQueryInstrument\n\n" << std::endl;
	CThostFtdcQryInstrumentField *orderField = new CThostFtdcQryInstrumentField();	//�����ѯ����Ϊ�գ�˵����Ҫ��ѯ�������ݡ������Ҫ��ѯĳ���������ĺ�Լ�����ڲ�ѯ������ָ��ExchangeID��
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ReqQryInstrumentCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);
	#if 0
	for(auto& it : m_contracts) 
	{  
		strcpy(orderField->InstrumentID, it.name().c_str());
		strcpy(orderField->ExchangeID , id->getExchangeID().c_str()); 	
		std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ReqQryInstrumentCommand>(api, orderField, requestID);
	    commandQueue.addCommand(command);	
    }  
	#endif
}

//�������������������ź� add
void CCtpTraderSpi::reqOrderInsert(const ::pb::ems::Order& order)
{	
    std::cout<< "CCtpTraderSpi::reqOrderInsert" <<std::endl;
	CThostFtdcInputOrderField *orderField = new CThostFtdcInputOrderField();
	strcpy(orderField->BrokerID, id->getBrokerID().c_str());
	strcpy(orderField->InvestorID, id->getInvestorID().c_str());
	std::string InstrumentID = order.contract();
	strcpy(orderField->InstrumentID, InstrumentID.c_str());
	itoa(maxOrderRef, orderField->OrderRef, 10);
	client_orders_index[maxOrderRef] = order.client_order_id();
	increaseRef();	//����orderRef
	//�����۸����� �Բ�ͬ���͵ı���(�޼۵�/�м۵�/������)����Ҫ����Ӧ���ֶν�����Ӧ�ر�ĸ�ֵ
	if(order.order_type() == pb::ems::OrderType::OT_Limit)   
	{
		orderField->OrderPriceType = THOST_FTDC_OPT_LimitPrice;  //�޼�
		orderField->LimitPrice = atof(order.price().c_str());		
	}
	else
	{
			orderField->OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //�м�
			orderField->LimitPrice = 0;                           //�۸�
	}
	//��������: 
	pb::ems::BuySell BuySellval = order.buy_sell(); 
	if(BuySellval == 1)
	{
		orderField->Direction = THOST_FTDC_D_Buy; //��
	}
	else
	{
		orderField->Direction = THOST_FTDC_D_Sell; //��
	}
	
//	if (order->getOpenCloseFlag() == '0'){ �˴�Ӧ����Order�������ֶ�
//		orderField->CombOffsetFlag[0] = THOST_FTDC_OF_Open;				//����
//	}
//	else{
		orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;		//���Ի�������ƽ��
//	}
	//if (order->getOpenCloseFlag() == '1'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_Close;				//ƽ��
	//}
	//if (order->getOpenCloseFlag() == '3'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;		//ƽ��
	//}
	//if (order->getOpenCloseFlag() == '4'){
	//	orderField->CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;	//ƽ��
	//}
	//orderField->VolumeTotalOriginal = order.getOriginalVolume();		//����
	//�����ǹ̶����ֶ�
	orderField->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		//Ͷ�� 
	orderField->TimeCondition = THOST_FTDC_TC_GFD;				//������Ч '3'
	orderField->VolumeCondition = THOST_FTDC_VC_AV;				//�κ����� '1'
	orderField->MinVolume = 1;
	orderField->ContingentCondition = THOST_FTDC_CC_Immediately;	//��������'1'
	orderField->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//��ǿƽ '0'
	orderField->IsAutoSuspend = 1;
	orderField->UserForceClose = 0;
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::InsertOrderCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);	
}	

//���ͱ�����ѯ���� Query
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
		strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	//���Ի�����ȫ����������
		strcpy(orderField->InstrumentID , order.contract().c_str());  //��Լ����
		///�������orderField->OrderSysID��Ҫ��ֵ Ŀǰorder����û��
		std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::QueryOrderCommand>(api, orderField, requestID);
		commandQueue.addCommand(command);
    }	
	else
	{
		std::cout << "֮ǰû���ύ���˱���������" << std::endl;
		return;
	}	


}	

//����	cancle Order
void CCtpTraderSpi::reqOrderAction(const ::pb::ems::Order& order, TThostFtdcActionFlagType ActionFlag)
{    
	//����������
	//const std::string &exchangeID = "SHFE";
	//���ó�����Ϣ
	CThostFtdcInputOrderActionField *orderField = new CThostFtdcInputOrderActionField();
	strcpy(orderField->BrokerID, id->getBrokerID().c_str());
	strcpy(orderField->InvestorID, id->getInvestorID().c_str());
	//strcpy(orderField->ExchangeID, exchangeID.c_str());
	strcpy(orderField->ExchangeID, id->getExchangeID().c_str());	//���Ի�����ȫ����������
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
		std::cout << "֮ǰû���ύ���˱���������" << std::endl;
		return;
	}
	
	itoa(orderRef, orderField->OrderRef, 10);
	orderField->ActionFlag = THOST_FTDC_AF_Delete;	//ɾ������ '0'
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::WithdrawOrderCommand>(api, orderField, requestID);
	commandQueue.addCommand(command);
}
	
//��ѯ�ͻ�����ֲ���� SendReqQryInvestorPosition
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
/****************************Api���׺���****************************************/
//��¼
void CCtpTraderSpi::login(){
	std::cout << "CCtpTraderSpi::login()>>>>>>>>>>start login... " << std::endl;
	CThostFtdcReqUserLoginField *loginField = new CThostFtdcReqUserLoginField();
	strcpy(loginField->BrokerID, id->getBrokerID().c_str()); //���͹�˾����
	strcpy(loginField->UserID, id->getUserID().c_str()); //�û�����
	strcpy(loginField->Password, id->getPassword().c_str());
	//��ָ��ŵ�����β��,�������ָ���ִ�з�������
	std::cout<<loginField->BrokerID<<" "<< loginField->UserID << " " << loginField->Password << std::endl;
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::LoginCommand>(api, loginField, requestID);
	commandQueue.addCommand(command);
}	

//ȷ�ϼ���
void CCtpTraderSpi::comfirmSettlement(){
	std::cout<< "CCtpTraderSpi::comfirmSettlement()" << std::endl;
	CThostFtdcSettlementInfoConfirmField *comfirmField = new CThostFtdcSettlementInfoConfirmField();
	strcpy(comfirmField->BrokerID, id->getBrokerID().c_str());
	strcpy(comfirmField->InvestorID, id->getUserID().c_str());
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::ComfirmSettlementCommand>(api, comfirmField, requestID);
	commandQueue.addCommand(command);
}

//�˳�
void CCtpTraderSpi::reqUserLogout()
{
	std::cout<< "CCtpTraderSpi::reqUserLogout" << std::endl;
	CThostFtdcUserLogoutField *loginField = new CThostFtdcUserLogoutField();
	strcpy(loginField->BrokerID, id->getBrokerID().c_str());
	strcpy(loginField->UserID, id->getUserID().c_str());
	std::shared_ptr<fh::ctp::exchange::ApiCommand> command = std::make_shared<fh::ctp::exchange::LoginOutCommand>(api, loginField, requestID);
	commandQueue.addCommand(command);
}
/****************************Api���׺���****************************************/
	
/****************************Spi�ص�����****************************************/
///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void CCtpTraderSpi::OnFrontConnected(){
	std::cout << "CCtpTraderSpi::OnFrontConnected" << std::endl;
	login();
}	

void CCtpTraderSpi::OnFrontDisconnected(int nReason)
{
    // ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
    LOG_INFO("CCtpTraderSpi::OnFrontDisconnected nReason = ", nReason);
}

void CCtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG_INFO("--->>> OnRspError");
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		LOG_ERROR("--->>> ErrorID=", pRspInfo->ErrorID, ", ErrorMsg=", pRspInfo->ErrorMsg);
}
	
///��¼������Ӧ
void CCtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		maxOrderRef = atoi(pRspUserLogin->MaxOrderRef); 
		++maxOrderRef;      //��ʼ����󱨵�����(����)
		comfirmSettlement();	//ȷ�Ͻ�����
	}
	else
	{
		std::cout << pRspUserLogin->UserID << "��¼ʧ��"  << std::endl;
	}
}

//Ͷ���߽�����ȷ����Ӧ
void CCtpTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		tradable = true;
		std::cout << "\n\naccount:" << pSettlementInfoConfirm->InvestorID << " Has been able to trade.\n" << std::endl;
		// �����ѯ��Լ
		reqQueryInstrument();	
	}
	else
	{
		std::cout << "account:" << pSettlementInfoConfirm->InvestorID << "Confirm the settlement failed" << std::endl;
	}
}

///����¼��������Ӧ(������ͨ��)
void CCtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "����¼��������Ӧ(������ͨ��)" << pRspInfo->ErrorID << std::endl;
	if (client_orders_index.find(std::atoi(pInputOrder->OrderRef)) != client_orders_index.end()){
		OnInsertOrder(pInputOrder, pRspInfo);
	}
}

///��������������Ӧ(������ͨ��)
void CCtpTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	std::cout << "��������������Ӧ(������ͨ��)"<<std::endl;
	if (client_orders_index.find(std::atoi(pInputOrderAction->OrderRef)) != client_orders_index.end()){
	    OnActionOrder(pInputOrderAction,pRspInfo);
	}
}
	
///����֪ͨ
void CCtpTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	std::cout << "�����ر�" << std::endl;
	std::cout << "���������:" << pOrder->ExchangeID
		 << " ��Լ����:" << pOrder->InstrumentID
		 << " ��������:" << pOrder->OrderRef
		 << " ��������:" << pOrder->Direction
		 << " ��Ͽ�ƽ��־:" << pOrder->CombOffsetFlag
		 << " �۸�:" << pOrder->LimitPrice
		 << " ����:" << pOrder->VolumeTotalOriginal
		 << " ��ɽ�����:" << pOrder->VolumeTraded
		 << " ʣ������:" << pOrder->VolumeTotal
		 << " ������ţ��жϱ����Ƿ���Ч��:" << pOrder->OrderSysID
		 << " ����״̬:" << pOrder->OrderStatus
		 << " ��������:" << pOrder->InsertDate
		 << " ���:" << pOrder->SequenceNo << std::endl;	

	if (client_orders_index.find(std::atoi(pOrder->OrderRef)) != client_orders_index.end()){	//��������	
		OnOrder(pOrder);
	}	
}

///�ɽ��ر�
void CCtpTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	std::cout << "CCtpTraderSpi::OnRtnTrade" << std::endl;
	std::cout << "=====Trade successful=====" << std::endl;
	std::cout << "TradeTime: " << pTrade->TradeTime << std::endl;
	std::cout << "InstrumentID: " << pTrade->InstrumentID << std::endl;
	std::cout << "Price: " << pTrade->Price << std::endl;
	std::cout << "Volume: " << pTrade->Volume << std::endl;
	std::cout << "Direction: " << pTrade->Direction << std::endl;
	
	if (client_orders_index.find(std::atoi(pTrade->OrderRef)) != client_orders_index.end()){
		OnFill(pTrade);
	}
}	

///�����ѯͶ���ֲ߳���Ӧ
void CCtpTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	std::cout << "�����ѯͶ����" << id->getInvestorID() << "�ֲ���Ӧ" << std::endl;
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
		OnQryInvestorPosition(pInvestorPosition,pRspInfo,nRequestID,bIsLast);
	}
}

//����¼�����ر�
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

//��ѯ��ԼӦ��
void CCtpTraderSpi::OnRspQryInstrument(
	CThostFtdcInstrumentField *pInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	char filePath[100] = "InstrumentID.csv";
	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); // �ļ�׷��д�� 
	outFile << pInstrument->InstrumentID << std::endl;
	outFile.close();	
	
	std::cout << "\nCCtpTraderSpi::OnRspQryInstrument\n" << std::endl;

	OnQryInstrument(pInstrument);
}
/****************************Spi�ص�����****************************************/
	
/****************************��������******************************************/	
//�뽻�����������ӣ�����׼�����׵�״̬(�Ǳ���)
void CCtpTraderSpi::readyToTrade(){
	std::cout << "�˻�������½..." << std::endl;
	api = CThostFtdcTraderApi::CreateFtdcTraderApi();
	api->RegisterSpi(this);
	//ע��ǰ�û�
	char *frontAddress = new char[100];
	strcpy(frontAddress, id->getExchangeFrontAddress().c_str());
	std::cout << frontAddress << std::endl;
	api->RegisterFront(frontAddress);	
	//���Ĺ�������˽����
	api->SubscribePublicTopic(THOST_TERT_RESTART); //�ӵ���ĵ�һ����¼��ʼ����������
	api->SubscribePrivateTopic(THOST_TERT_RESTART);
    //�����������
	commandQueue.run();	
    api->Init(); 	
	std::cout << "readyToTrade end" << std::endl;
}	

//����¼��Ӧ�����ݷ��͸����Զ�
void CCtpTraderSpi::OnInsertOrder(CThostFtdcInputOrderField  *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	LOG_INFO("--->>> ",  __FUNCTION__);
	if(NULL != m_strategy)
	{
		::pb::ems::Order tmporder;
		std::string tmpc_OrderId = client_orders_index[std::atoi(pInputOrder->OrderRef)];
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

//�ύ�����������������ݷ��͸����Զ�
void CCtpTraderSpi::OnActionOrder(CThostFtdcInputOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{	
	LOG_INFO("CCtpTraderSpi::OnActionOrder");
	if(NULL != m_strategy)
	{
		::pb::ems::Order tmporder;
		std::string tmpc_OrderId = client_orders_index[std::atoi(pOrderAction->OrderRef)];
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
	    std::string tmpc_OrderId = client_orders_index[std::atoi(pOrder->OrderRef)];
		tmporder.set_client_order_id(tmpc_OrderId);
        tmporder.set_account(pOrder->UserID);
        tmporder.set_contract(pOrder->InstrumentID);
		
		//��������  THOST_FTDC_D_Buy '0'(��)  THOST_FTDC_D_Sell '1'(��)
		if(pOrder->Direction == THOST_FTDC_D_Buy)
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
		}
		else
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
		}
        tmporder.set_price(std::to_string(pOrder->LimitPrice));
        tmporder.set_quantity(pOrder->VolumeTotalOriginal);  //�ñ�����ԭʼ��������
		/*
			IOC '1' ������ɣ�������          
			GFS '2' ������Ч
			GFD '3'������Ч
			GTD '4'ָ������ǰ��Ч
			GTC '5'����ǰ��Ч
			GFA '6'���Ͼ�����Ч
		*/
		char str[10];
		sprintf(str, "%d",pOrder->TimeCondition); 
		int TimeCondition= atoi(str)-48;	   
	    tmporder.set_tif((::pb::ems::TimeInForce)TimeCondition);
		
		//�����۸�����
		if(pOrder->OrderPriceType == '2')  //�޼�
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
		}
		else
		if(pOrder->OrderPriceType == '1')  //�����
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Market);
		}	        		
        tmporder.set_exchange_order_id(pOrder->OrderSysID);
        SetOrderStatus(pOrder,tmporder);


		tmporder.set_working_price(std::to_string(pOrder->LimitPrice)); //�۸�
		tmporder.set_working_quantity(pOrder->VolumeTotal);  //�ñ�����ʣ������
		tmporder.set_filled_quantity(pOrder->VolumeTraded);  //�ñ������ѳɽ�����

		std::string tmpActionDay = pOrder->TradingDay;	//������
		std::string tmpActiontime = pOrder->InsertTime; //ί��ʱ��
		std::string tmpalltime = tmpActionDay + "-" + tmpActiontime + ".000";
		fh::core::assist::utility::To_pb_time(tmporder.mutable_submit_time(), tmpalltime);	

	    m_strategy->OnOrder(tmporder);
    }
}

//�ɽ����ݷ��͸����Զ�
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

	std::string tmpc_OrderId = client_orders_index[std::atoi(pTrade->OrderRef)];
	if(tmpc_OrderId == "")
	{
		tmpc_OrderId = pTrade->OrderSysID; //�������
		tmpc_OrderId = tmpc_OrderId + "|";
		tmpc_OrderId = tmpc_OrderId + pTrade->OrderRef; //���ر������
	}
	tmpfill.set_client_order_id(tmpc_OrderId);

	tmpfill.set_exchange_order_id(pTrade->OrderSysID);		

	std::string tmpTradeDay = pTrade->TradeDate;	
	std::string tmpTradeTime = pTrade->TradeTime;
	std::string tmpalltime = tmpTradeDay + "-" + tmpTradeTime + ".000";
	fh::core::assist::utility::To_pb_time(tmpfill.mutable_fill_time(), tmpalltime);
	
	m_strategy->OnFill(tmpfill);
	
}

//������ѯ���󡣵��ͻ��˷���������ѯָ��󣬽����й�ϵͳ������Ӧʱ���÷����ᱻ����
void CCtpTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnRspQryOrder");
    if(NULL == pOrder || NULL == pRspInfo)
    {
		LOG_ERROR("CCtpTraderSpi::OnRspQryOrder Error");
		return ;	
    }	

    int orderRef = atoi(pOrder->OrderRef);
	if (client_orders_index.find(orderRef) != client_orders_index.end())	//��������
	{	
	    if(m_InitQueryNum > 0)
        {
            m_InitQueryNum--;
        }
		OnQryOrder(pOrder);
	}		
}	

//������Ӧ���ݷ��͸����Զ�
void CCtpTraderSpi::OnQryOrder(CThostFtdcOrderField *pOrder)
{
    LOG_INFO("CCtpTraderSpi::OnQryOrder");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;  
        
        std::string tmpc_OrderId = client_orders_index[std::atoi(pOrder->OrderRef)]; 

		tmporder.set_client_order_id(tmpc_OrderId);		
        tmporder.set_account(pOrder->UserID); //�û�����
        tmporder.set_contract(pOrder->InstrumentID);	 //��Լ����	
		tmporder.set_client_order_id(tmpc_OrderId);		
		tmporder.set_account(pOrder->UserID); //�û�����
		tmporder.set_contract(pOrder->InstrumentID); //��Լ����
		
		if(pOrder->Direction == '0') //��������
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Buy);
		}
		else
		{
			tmporder.set_buy_sell(pb::ems::BuySell::BS_Sell);
		}
		tmporder.set_price(std::to_string(pOrder->LimitPrice)); //�۸�
		tmporder.set_quantity(pOrder->VolumeTotalOriginal); //����
		tmporder.set_tif(pb::ems::TimeInForce::TIF_GFD);
		if(pOrder->OrderPriceType == '2')   //�����۸�����
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Limit);
		}
		else
		{
			tmporder.set_order_type(pb::ems::OrderType::OT_Market);
		}	     		
		tmporder.set_exchange_order_id(pOrder->OrderSysID); //�������

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


//�ɽ�����ѯӦ�𡣵��ͻ��˷����ɽ�����ѯָ��󣬽����й�ϵͳ������Ӧʱ���÷����ᱻ���á� B
void CCtpTraderSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnRspQryTrade");
    if(NULL == pTrade || NULL == pRspInfo)
    {
        LOG_ERROR("CCtpThostFtdcTraderManger::OnRspQryTrade Error");
	    return ;	
    }		
	if (client_orders_index.find(std::atoi(pTrade->OrderRef)) != client_orders_index.end()){	//��������	
		OnQryTrade(pTrade,pRspInfo,nRequestID,bIsLast);	
	}	
    
}
//�ɽ���Ϣ�ṹ���ݷ��͸����Զˡ� B
void CCtpTraderSpi::OnQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CCtpTraderSpi::OnQryTrade");
    if(NULL != m_strategy)
    {
        ::pb::ems::Order tmporder;

        std::string tmpc_OrderId = client_orders_index[std::atoi(pTrade->OrderRef)];
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

//Ͷ���ֲ߳����ݷ��͸����Զ� E
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
   
    *p-- = '\0 ';         
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
