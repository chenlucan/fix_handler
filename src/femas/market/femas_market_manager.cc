

#include "femas_market_manager.h"


namespace fh
{
namespace femas
{
namespace market
{







 void CFemasMarkrtManager::OnFrontConnected() 
 {
       printf("CFemasMarkrtManager::OnFrontConnected.\n");

       if(NULL == m_pFileConfig)
	{
           printf("Error m_pFileConfig is NULL.\n");
	    return;	   
	}
	if(NULL == m_pUserApi)
	{
           printf("Error m_pUserApi is NULL.\n");
	    return;	   
	}   
	   
	CUstpFtdcReqUserLoginField reqUserLogin;
	strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
	//strcpy(reqUserLogin.BrokerID, "0135");
	//strcpy(reqUserLogin.UserID, "000101");
	//strcpy(reqUserLogin.Password, "111111");
	std::string BrokerIDstr = m_pFileConfig->Get("femas-market.BrokerID");
	std::string UserIDstr = m_pFileConfig->Get("femas-market.UserID");
	std::string Passwordstr = m_pFileConfig->Get("femas-market.Password");
       strcpy(reqUserLogin.BrokerID, BrokerIDstr.c_str());
	printf("femas-market.BrokerID = %s.\n",reqUserLogin.BrokerID);
	strcpy(reqUserLogin.UserID, UserIDstr.c_str());
	printf("femas-market.UserID = %s.\n",reqUserLogin.UserID);
	strcpy(reqUserLogin.Password, Passwordstr.c_str());
	printf("femas-market.Passwor = %s.\n",reqUserLogin.Password);
	
	int ret = m_pUserApi->ReqUserLogin(&reqUserLogin, 0);

	printf("m_pUserApi::ReqUserLogin ret = %d \n",ret);

	return;
}


 void CFemasMarkrtManager::OnFrontDisconnected()
{
	// 当发生这个情况后，API会自动重新连接，客户端可不做处理
	printf("OnFrontDisconnected.\n");
	return;
}
	

 void CFemasMarkrtManager::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	printf("OnRspUserLogin:\n");
	printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		
	if (pRspInfo->ErrorID != 0) 
	{
		// 端登失败，客户端需进行错误处理
		printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		return;
	}
	
      char *contracts[1];
      contracts[0] = new char[100];
      //contracts[1] = new char[100];
     // contracts[2] = new char[100];
      memset(contracts[0],0,100);
      //memset(contracts[1],0,100);
      //memset(contracts[2],0,100);
      strcpy(contracts[0], "*");
      //strcpy(contracts[0], "pb1705");
      //strcpy(contracts[1], "cu1709C45000");
      //strcpy(contracts[2], "rb1710");	  

	m_pUserApi->SubMarketData(contracts, 1);

	delete []contracts[0];
       //delete []contracts[1];
       //delete []contracts[2];

//	char * uncontracts[2]={"",""};
//	uncontracts[0]="IF1211";
//	uncontracts[1]="IF1212";
//	m_pUserApi->UnSubMarketData(uncontracts, 2);

       

       return;
}


void CFemasMarkrtManager::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData) 
{
		// 客户端按需处理返回的数据
	printf("GetDepthMarketData::begin\n");	
	printf("name : %s : \n",pMarketData->InstrumentID);
		
	printf("%s,%d \n",pMarketData->UpdateTime,pMarketData->UpdateMillisec);

//申买一
       if (pMarketData->BidPrice1==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->BidPrice1);

	printf("%d---, ",pMarketData->BidVolume1);

//申买二
	 if (pMarketData->BidPrice2==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->BidPrice2);

	printf("%d---, ",pMarketData->BidVolume2);

//申买三
	 if (pMarketData->BidPrice3==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->BidPrice3);

	printf("%d---, ",pMarketData->BidVolume3);

//申买四
	 if (pMarketData->BidPrice4==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->BidPrice4);

	printf("%d---, ",pMarketData->BidVolume4);

//申买五
	 if (pMarketData->BidPrice5==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->BidPrice5);

	printf("%d---\n",pMarketData->BidVolume5);

//申卖一	
	if (pMarketData->AskPrice1==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->AskPrice1);

	printf("%d---,",pMarketData->AskVolume1);	

//申卖二
	if (pMarketData->AskPrice2==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->AskPrice2);

	printf("%d---,",pMarketData->AskVolume2);

//申卖三
	if (pMarketData->AskPrice3==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->AskPrice3);

	printf("%d---,",pMarketData->AskVolume3);

//申卖四	
	if (pMarketData->AskPrice4==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->AskPrice4);

	printf("%d---,",pMarketData->AskVolume4);

//申卖五	
	if (pMarketData->AskPrice5==DBL_MAX)
		printf("---%s,","NULL");
	else
		printf("---%f,",pMarketData->AskPrice5);

	printf("%d---\n",pMarketData->AskVolume5);	
	
	printf("GetDepthMarketData::end\n");

	m_pFemasBookManager->SendFemasarketData(pMarketData);

	return;
}


void CFemasMarkrtManager::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	printf("OnRspError:\n");
	printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// 客户端需进行错误处理
       return;		
}

void CFemasMarkrtManager::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	printf("Sub 返回订阅合约：%s \n",pSpecificInstrument->InstrumentID);
	return;
}


 void CFemasMarkrtManager::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	printf("UnSub 返回订阅合约：%s \n",pSpecificInstrument->InstrumentID);
	return;
}


 void CFemasMarkrtManager::SetFileData(std::string &FileConfig)
{
      printf("CFemasMarkrtManager::SetData %s\n",FileConfig.c_str());
      m_pFileConfig = new fh::core::assist::Settings(FileConfig);
      if(NULL == m_pFileConfig)
      {
          printf("Error m_pFileConfig is NULL \n");
      }
      return;	  
}


void CFemasMarkrtManager::CreateFemasBookManager(fh::core::market::MarketListenerI *sender)
{
      printf("CFemasMarkrtManager::CreateFemasBookManager \n");
      m_pFemasBookManager = NULL;
      m_pFemasBookManager = new fh::femas::market::CFemasBookManager(sender);
      if(NULL == m_pFemasBookManager)
      {
          printf("Error m_pFemasBookManager is NULL \n");
      }
      return;	  
}


}
}
}


