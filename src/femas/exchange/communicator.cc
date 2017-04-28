#include <unistd.h>
#include <time.h>
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
    }
    LOG_INFO("MaxOrderLocalID = ", atoi(pRspUserLogin->MaxOrderLocalID));	
    MaxOrderLocalID = atoi(pRspUserLogin->MaxOrderLocalID)+1;	
    //strncpy(MaxOrderLocalID,pRspUserLogin->MaxOrderLocalID,strlen(pRspUserLogin->MaxOrderLocalID));	
    mIConnet = 0;	
}	
void CUstpFtdcTraderManger::OnRspOrderInsert(CUstpFtdcInputOrderField  *pInputOrder, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    // 输出报单录入结果
    LOG_INFO("CUstpFtdcTraderManger::OnRspOrderInsert");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );
    return;	
}

///报单回报
void CUstpFtdcTraderManger::OnRtnOrder(CUstpFtdcOrderField  *pOrder)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRtnOrder");
    LOG_INFO("OrderSysID=[",pOrder->OrderSysID,"]");
}

// 针对用户请求的出错通知
void CUstpFtdcTraderManger::OnRspError(CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG_INFO("CUstpFtdcTraderManger::OnRspError");
    LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );
    LOG_INFO("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    // 客户端需进行错误处理

}
void CUstpFtdcTraderManger::SetFileConfigData(const std::string &FileConfig)
{
    LOG_INFO("CUstpFtdcTraderManger::SetFileConfigData file =  ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
}



CFemasGlobexCommunicator::CFemasGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy)
{

     m_pFileConfig = new fh::core::assist::Settings(config_file);
     m_pUserApi = CUstpFtdcTraderApi::CreateFtdcTraderApi();	 
     m_pUstpFtdcTraderManger = new CUstpFtdcTraderManger(m_pUserApi);
     m_pUserApi->RegisterSpi(m_pUstpFtdcTraderManger);	
     m_pUstpFtdcTraderManger->SetFileConfigData(config_file);
     m_itimeout = 10;
      
}

CFemasGlobexCommunicator::~CFemasGlobexCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     m_pUserApi->Release();	
     //m_pUserApi->Release();	 
}

bool CFemasGlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     if(m_pUstpFtdcTraderManger->mIConnet != 0)
    {
        return false;
    }
    return true;
}

void CFemasGlobexCommunicator::Stop()
{
    LOG_INFO("CFemasGlobexCommunicator::Stop ");
	
    CUstpFtdcReqUserLogoutField reqUserLogout;

    std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
    strcpy(reqUserLogout.BrokerID, BrokerIDstr.c_str());
    LOG_INFO("femas-user.BrokerID = ",reqUserLogout.BrokerID);
    strcpy(reqUserLogout.UserID, UserIDstr.c_str());
    LOG_INFO("femas-user.UserID = ",reqUserLogout.UserID);
	
    
    m_pUserApi->ReqUserLogout(&reqUserLogout,0);

    //printf("m_pUserApi::Release \n");
    //m_pUserApi->Release();	
    return;
}


void CFemasGlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
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
	           return;		  
	      }
             sleep(0.1);    
         }	 

	
         LOG_INFO("CFemasGlobexCommunicator::mIConnet is ok ");	 
         return;	
}

void CFemasGlobexCommunicator::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Add ");
        LOG_INFO("CUstpFtdcInputOrderField: ");
	 //return;	
		
        CUstpFtdcInputOrderField SInputOrder;
	 memset(&SInputOrder,0,sizeof(CUstpFtdcInputOrderField));	
	 strncpy(SInputOrder.UserOrderLocalID,order.client_order_id().c_str(),order.client_order_id().length());	
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
            return;
	 }


	 std::string OffsetFlag = m_pFileConfig->Get("femas-exchange.OffsetFlag");
	 SInputOrder.OffsetFlag = OffsetFlag.c_str()[0];

	 std::string HedgeFlag = m_pFileConfig->Get("femas-exchange.HedgeFlag");
        SInputOrder.HedgeFlag = HedgeFlag.c_str()[0];

		
        SInputOrder.LimitPrice = atof(order.price().c_str());
        SInputOrder.Volume = order.quantity();

        std::string TimeCondition = m_pFileConfig->Get("femas-exchange.TimeCondition");
	 SInputOrder.TimeCondition = TimeCondition.c_str()[0];

        std::string IsAutoSuspend = m_pFileConfig->Get("femas-exchange.IsAutoSuspend"); 
	 SInputOrder.IsAutoSuspend = atoi(IsAutoSuspend.c_str());

        std::string ExchangeID = m_pFileConfig->Get("femas-exchange.ExchangeID");
	 strcpy(SInputOrder.ExchangeID , ExchangeID.c_str());  

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

	 
        m_pUserApi->ReqOrderInsert(&SInputOrder, 1);
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
        return;
}

void CFemasGlobexCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasGlobexCommunicator::Query ");
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













}
}
}