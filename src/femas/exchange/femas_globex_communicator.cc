#include <unistd.h>
#include <time.h>
#include "femas_globex_communicator.h"



namespace fh
{
namespace femas
{
namespace exchange
{

// 当客户端与飞马平台建立起通信连接，客户端需要进行登录
void CUstpFtdcTraderManger::OnFrontConnected()
{
    printf("CUstpFtdcTraderManger::OnFrontConnected\n");
    CUstpFtdcReqUserLoginField reqUserLogin;

    std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
    std::string Passwordstr = m_pFileConfig->Get("femas-user.Password");
    strcpy(reqUserLogin.BrokerID, BrokerIDstr.c_str());
    printf("femas-user.BrokerID = %s.\n",reqUserLogin.BrokerID);
    strcpy(reqUserLogin.UserID, UserIDstr.c_str());
    printf("femas-user.UserID = %s.\n",reqUserLogin.UserID);
    strcpy(reqUserLogin.Password, Passwordstr.c_str());
    printf("femas-user.Passwor = %s.\n",reqUserLogin.Password);
    m_pUserApi->ReqUserLogin(&reqUserLogin, 0);	
	
}
void CUstpFtdcTraderManger::OnFrontDisconnected(int nReason)
{
    // 当发生这个情况后，API会自动重新连接，客户端可不做处理
    printf("CUstpFtdcTraderManger::OnFrontDisconnected.\n");
}
void CUstpFtdcTraderManger::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("CUstpFtdcTraderManger::OnRspUserLogin:\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n",
    pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    if (pRspInfo->ErrorID != 0) 
    {
		// 端登失败，客户端需进行错误处理
         printf("Failed to login, errorcode=%d errormsg=%s	requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg,nRequestID, bIsLast);
	  //exit(-1);
	  mIConnet = 1;
    }
    printf("MaxOrderLocalID=%d", atoi(pRspUserLogin->MaxOrderLocalID));	
    mIConnet = 0;	
}	
void CUstpFtdcTraderManger::OnRspOrderInsert(CUstpFtdcInputOrderField  *pInputOrder, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    // 输出报单录入结果
    printf("CUstpFtdcTraderManger::OnRspOrderInsert\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n",pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

///报单回报
void CUstpFtdcTraderManger::OnRtnOrder(CUstpFtdcOrderField  *pOrder)
{
    printf("CUstpFtdcTraderManger::OnRtnOrder\n");
    printf("OrderSysID=[%s]\n", pOrder->OrderSysID);
}

// 针对用户请求的出错通知
void CUstpFtdcTraderManger::OnRspError(CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("CUstpFtdcTraderManger::OnRspError\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n",
    pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    // 客户端需进行错误处理

}
void CUstpFtdcTraderManger::SetFileConfigData(const std::string &FileConfig)
{
    printf("CUstpFtdcTraderManger::SetFileConfigData file = %s \n",FileConfig.c_str());
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
    printf("CFemasGlobexCommunicator::Stop \n");
	
    CUstpFtdcReqUserLogoutField reqUserLogout;

    std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
    strcpy(reqUserLogout.BrokerID, BrokerIDstr.c_str());
    printf("femas-user.BrokerID = %s.\n",reqUserLogout.BrokerID);
    strcpy(reqUserLogout.UserID, UserIDstr.c_str());
    printf("femas-user.UserID = %s.\n",reqUserLogout.UserID);
	
    
    m_pUserApi->ReqUserLogout(&reqUserLogout,0);

    printf("m_pUserApi::Release \n");
    m_pUserApi->Release();	
    return;
}


void CFemasGlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
        m_pUserApi->SubscribePrivateTopic(USTP_TERT_RESUME);	 
        m_pUserApi->SubscribePublicTopic(USTP_TERT_RESUME);

        std::string tmpurl = m_pFileConfig->Get("femas-exchange.url");
        printf("femas exchange url = %s \n",tmpurl.c_str());	 
        m_pUserApi->RegisterFront((char*)(tmpurl.c_str()));	

        m_pUserApi->Init();

	 time_t tmtimeout = time(NULL);
        while(0 != m_pUstpFtdcTraderManger->mIConnet)
        {
             if(time(NULL)-tmtimeout>m_itimeout)
	      {
                  printf("CFemasGlobexCommunicator::mIConnet tiomeout \n");
	           return;		  
	      }
             sleep(0.1);    
         }	 
         printf("CFemasGlobexCommunicator::mIConnet is ok \n");	 

         return;	
}

void CFemasGlobexCommunicator::Add(const ::pb::ems::Order& order)
{
        CUstpFtdcInputOrderField SInputOrder;
        m_pUserApi->ReqOrderInsert(&SInputOrder, 1);
        return;
}

void CFemasGlobexCommunicator::Change(const ::pb::ems::Order& order)
{
        return;
}

void CFemasGlobexCommunicator::Delete(const ::pb::ems::Order& order)
{
        return;
}

void CFemasGlobexCommunicator::Query(const ::pb::ems::Order& order)
{
        return;
}

void CFemasGlobexCommunicator::Query_mass(const char *data, size_t size)
{
        return;
}

void CFemasGlobexCommunicator::Delete_mass(const char *data, size_t size)
{
        return;
}













}
}
}