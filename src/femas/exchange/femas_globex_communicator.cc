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

    std::string BrokerIDstr = m_pFileConfig->Get("femas-market.BrokerID");
    std::string UserIDstr = m_pFileConfig->Get("femas-market.UserID");
    std::string Passwordstr = m_pFileConfig->Get("femas-market.Password");
    strcpy(reqUserLogin.BrokerID, BrokerIDstr.c_str());
    printf("femas-market.BrokerID = %s.\n",reqUserLogin.BrokerID);
    strcpy(reqUserLogin.UserID, UserIDstr.c_str());
    printf("femas-market.UserID = %s.\n",reqUserLogin.UserID);
    strcpy(reqUserLogin.Password, Passwordstr.c_str());
    printf("femas-market.Passwor = %s.\n",reqUserLogin.Password);
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
	  exit(-1);
    }
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
void CUstpFtdcTraderManger::SetFileConfigData(std::string &FileConfig)
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
}

CFemasGlobexCommunicator::~CFemasGlobexCommunicator()
{
     delete m_pFileConfig;
}


}
}
}