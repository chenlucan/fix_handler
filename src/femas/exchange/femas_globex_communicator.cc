#include "femas_globex_communicator.h"



namespace fh
{
namespace femas
{
namespace exchange
{

// ���ͻ��������ƽ̨������ͨ�����ӣ��ͻ�����Ҫ���е�¼
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
    // ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
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
		// �˵�ʧ�ܣ��ͻ�������д�����
         printf("Failed to login, errorcode=%d errormsg=%s	requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg,nRequestID, bIsLast);
	  exit(-1);
    }
}	
void CUstpFtdcTraderManger::OnRspOrderInsert(CUstpFtdcInputOrderField  *pInputOrder, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    // �������¼����
    printf("CUstpFtdcTraderManger::OnRspOrderInsert\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n",pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

///�����ر�
void CUstpFtdcTraderManger::OnRtnOrder(CUstpFtdcOrderField  *pOrder)
{
    printf("CUstpFtdcTraderManger::OnRtnOrder\n");
    printf("OrderSysID=[%s]\n", pOrder->OrderSysID);
}

// ����û�����ĳ���֪ͨ
void CUstpFtdcTraderManger::OnRspError(CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("CUstpFtdcTraderManger::OnRspError\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n",
    pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    // �ͻ�������д�����

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