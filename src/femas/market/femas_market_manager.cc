
#include "core/assist/logger.h"
#include "femas_market_manager.h"


namespace fh
{
namespace femas
{
namespace market
{







 void CFemasMarketManager::OnFrontConnected() 
 {
       LOG_INFO("CFemasMarketManager::OnFrontConnected.");

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
	   
	CUstpFtdcReqUserLoginField reqUserLogin;
	strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
	std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
	std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
	std::string Passwordstr = m_pFileConfig->Get("femas-user.Password");
       strcpy(reqUserLogin.BrokerID, BrokerIDstr.c_str());
	LOG_INFO("femas-user.BrokerID = ",reqUserLogin.BrokerID);
	strcpy(reqUserLogin.UserID, UserIDstr.c_str());
	LOG_INFO("femas-user.UserID = ",reqUserLogin.UserID);
	strcpy(reqUserLogin.Password, Passwordstr.c_str());
	LOG_INFO("femas-user.Passwor = ",reqUserLogin.Password);
	
	int ret = m_pUserApi->ReqUserLogin(&reqUserLogin, 0);

	LOG_INFO("m_pUserApi::ReqUserLogin ret = ",ret);

	return;
}


 void CFemasMarketManager::OnFrontDisconnected()
{
	// 当发生这个情况后，API会自动重新连接，客户端可不做处理
	LOG_INFO("OnFrontDisconnected.");
	return;
}
	

 void CFemasMarketManager::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	LOG_INFO("OnRspUserLogin:");
	if(NULL == pRspUserLogin || NULL == pRspInfo)
       {
            LOG_ERROR("CFemasMarketManager::OnRspUserLogin Error");
	     return ;	
       }
	LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]\n" );
	LOG_INFO("RequestID=[",nRequestID,"], Chain=[",bIsLast,"]\n" );
		
	if (pRspInfo->ErrorID != 0) 
	{
		// 端登失败，客户端需进行错误处理
		LOG_ERROR("Failed to login, errorcode=",pRspInfo->ErrorID," errormsg=",pRspInfo->ErrorMsg," requestid=",nRequestID," chain=",bIsLast );
		mIConnet = 1;
		return;
	}
	
	mIConnet = 0;
       return;
}


void CFemasMarketManager::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData) 
{
       if(NULL == pMarketData)
       {
            LOG_ERROR("CFemasMarketManager::OnRtnDepthMarketData Error");
	     return ;	
       }
		// 客户端按需处理返回的数据
	LOG_INFO("GetDepthMarketData::begin");	
	LOG_INFO("name : ",pMarketData->InstrumentID);
		
	LOG_INFO("UpdateTime=",pMarketData->UpdateTime,"--",pMarketData->UpdateMillisec);

//申买一
       if (pMarketData->BidPrice1==DBL_MAX)
		LOG_INFO("BidPrice1:NULL");
	else
		LOG_INFO("BidPrice1:",pMarketData->BidPrice1);

	LOG_INFO("BidVolume1: ",pMarketData->BidVolume1);

//申卖一	
	if (pMarketData->AskPrice1==DBL_MAX)
		LOG_INFO("AskPrice1:NULL");
	else
		LOG_INFO("AskPrice1:",pMarketData->AskPrice1);

	LOG_INFO("AskVolume1:",pMarketData->AskVolume1);	
	
	LOG_INFO("GetDepthMarketData::end");

	m_pFemasBookManager->SendFemasmarketData(pMarketData);
       StructToJSON(pMarketData);

	return;
}


void CFemasMarketManager::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	LOG_INFO("OnRspError:");
	if(NULL == pRspInfo)
       {
            LOG_ERROR("CFemasMarketManager::OnRspError Error");
	     return ;	
       }
	LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );
	LOG_INFO("RequestID=[",nRequestID,"], Chain=[",bIsLast,"]");
		// 客户端需进行错误处理
       return;		
}

void CFemasMarketManager::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
       if(NULL == pSpecificInstrument)
       {
            LOG_ERROR("CFemasMarketManager::OnRspSubMarketData Error");
	     return ;	
       }
	LOG_INFO("Sub  InstrumentID :",pSpecificInstrument->InstrumentID);
	if(mISubSuss > 0)
	{
           mISubSuss--;
	}
	
	return;
}


 void CFemasMarketManager::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
       if(NULL == pSpecificInstrument)
       {
            LOG_ERROR("CFemasMarketManager::OnRspUnSubMarketData Error");
	     return ;	
       }
	LOG_INFO("UnSub InstrumentID :",pSpecificInstrument->InstrumentID);
	return;
}

void CFemasMarketManager::OnHeartBeatWarning(int nTimeLapse)
{
      LOG_INFO("CFemasMarketManager::OnHeartBeatWarning ",nTimeLapse);
      return;	  
}

 void CFemasMarketManager::SetFileData(std::string &FileConfig)
{
      LOG_INFO("CFemasMarketManager::SetData ",FileConfig.c_str());
      m_pFileConfig = new fh::core::assist::Settings(FileConfig);
      if(NULL == m_pFileConfig)
      {
          LOG_ERROR("Error m_pFileConfig is NULL ");
      }
      return;	  
}


void CFemasMarketManager::CreateFemasBookManager(fh::core::market::MarketListenerI *sender)
{
      LOG_INFO("CFemasMarketManager::CreateFemasBookManager ");
      if(NULL != m_pFemasBookManager)
      {
          delete m_pFemasBookManager;
          m_pFemasBookManager = NULL;
      }
      
      m_pFemasBookManager = new fh::femas::market::CFemasBookManager(sender);
      if(NULL == m_pFemasBookManager)
      {
          LOG_ERROR("Error m_pFemasBookManager is NULL ");
      }
      return;	  
}


void CFemasMarketManager::StructToJSON(CUstpFtdcDepthMarketDataField *pMarketData)
{
    if(NULL == pMarketData)
    {
        return;
    }
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("TradingDay", T(pMarketData->TradingDay)));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementGroupID", T(pMarketData->SettlementGroupID)));	
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementID", T(pMarketData->SettlementID)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreSettlementPrice", T(pMarketData->PreSettlementPrice)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreClosePrice", T(pMarketData->PreClosePrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("PreOpenInterest", T(pMarketData->PreOpenInterest)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreDelta", T(pMarketData->PreDelta)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenPrice", T(pMarketData->OpenPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("HighestPrice", T(pMarketData->HighestPrice)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowestPrice", T(pMarketData->LowestPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("ClosePrice", T(pMarketData->ClosePrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("UpperLimitPrice", T(pMarketData->UpperLimitPrice)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowerLimitPrice", T(pMarketData->LowerLimitPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementPrice", T(pMarketData->SettlementPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("CurrDelta", T(pMarketData->CurrDelta)));
    tmjson.append(bsoncxx::builder::basic::kvp("LastPrice", T(pMarketData->LastPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("Volume", T(pMarketData->Volume)));
    tmjson.append(bsoncxx::builder::basic::kvp("Turnover", T(pMarketData->Turnover)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenInterest", T(pMarketData->OpenInterest)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice1", T(pMarketData->BidPrice1)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume1", T(pMarketData->BidVolume1)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice1", T(pMarketData->AskPrice1)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume1", T(pMarketData->AskVolume1)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice2", T(pMarketData->BidPrice2)));	


    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume2", T(pMarketData->BidVolume2)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice3", T(pMarketData->BidPrice3)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume3", T(pMarketData->BidVolume3)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice2", T(pMarketData->AskPrice2)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume2", T(pMarketData->AskVolume2)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice3", T(pMarketData->AskPrice3)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume3", T(pMarketData->AskVolume3)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice4", T(pMarketData->BidPrice4)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume4", T(pMarketData->BidVolume4)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice5", T(pMarketData->BidPrice5)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume5", T(pMarketData->BidVolume5)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice4", T(pMarketData->AskPrice4)));	


    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume4", T(pMarketData->AskVolume4)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice5", T(pMarketData->AskPrice5)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume5", T(pMarketData->AskVolume5)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T(pMarketData->InstrumentID)));
    tmjson.append(bsoncxx::builder::basic::kvp("UpdateTime", T(pMarketData->UpdateTime)));
    tmjson.append(bsoncxx::builder::basic::kvp("UpdateMillisec", T(pMarketData->UpdateMillisec)));
    tmjson.append(bsoncxx::builder::basic::kvp("ActionDay", T(pMarketData->ActionDay)));
    tmjson.append(bsoncxx::builder::basic::kvp("HisHighestPrice", T(pMarketData->HisHighestPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("HisLowestPrice", T(pMarketData->HisLowestPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("LatestVolume", T(pMarketData->LatestVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("InitVolume", T(pMarketData->InitVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("ChangeVolume", T(pMarketData->ChangeVolume)));	


    tmjson.append(bsoncxx::builder::basic::kvp("BidImplyVolume", T(pMarketData->BidImplyVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskImplyVolume", T(pMarketData->AskImplyVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("AvgPrice", T(pMarketData->AvgPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("ArbiType", T(pMarketData->ArbiType)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID_1", T(pMarketData->InstrumentID_1)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID_2", T(pMarketData->InstrumentID_2)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentName", T(pMarketData->InstrumentName)));
    tmjson.append(bsoncxx::builder::basic::kvp("TotalBidVolume", T(pMarketData->TotalBidVolume)));
    tmjson.append(bsoncxx::builder::basic::kvp("TotalAskVolume", T(pMarketData->TotalAskVolume)));
    
	
	
    FemasDateToString(tmjson,pMarketData->InstrumentID);	
    return;	
}
void CFemasMarketManager::FemasDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID)
{
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T(InstrumentID)));		
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    m_pFemasBookManager->SendFemasToDB(bsoncxx::to_json(tmjson.view()));
    return;	

    //OnOrginalMessage	
}


}
}
}


