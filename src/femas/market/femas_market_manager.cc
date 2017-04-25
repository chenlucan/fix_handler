
#include "core/assist/logger.h"
#include "femas_market_manager.h"


namespace fh
{
namespace femas
{
namespace market
{







 void CFemasMarkrtManager::OnFrontConnected() 
 {
       LOG_INFO("CFemasMarkrtManager::OnFrontConnected.");

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


 void CFemasMarkrtManager::OnFrontDisconnected()
{
	// 当发生这个情况后，API会自动重新连接，客户端可不做处理
	LOG_INFO("OnFrontDisconnected.");
	return;
}
	

 void CFemasMarkrtManager::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	LOG_INFO("OnRspUserLogin:");
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


void CFemasMarkrtManager::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData) 
{
		// 客户端按需处理返回的数据
	LOG_INFO("GetDepthMarketData::begin\n");	
	LOG_INFO("name : ",pMarketData->InstrumentID);
		
	LOG_INFO("UpdateTime=",pMarketData->UpdateTime,"--",pMarketData->UpdateMillisec);

//申买一
       if (pMarketData->BidPrice1==DBL_MAX)
		LOG_INFO("BidPrice1:NULL");
	else
		LOG_INFO("BidPrice1:",pMarketData->BidPrice1);

	LOG_INFO("BidVolume1: ",pMarketData->BidVolume1);

//申买二
	 if (pMarketData->BidPrice2==DBL_MAX)
		LOG_INFO("BidPrice2:NULL");
	else
		LOG_INFO("BidPrice2:",pMarketData->BidPrice2);

	LOG_INFO("BidVolume2:",pMarketData->BidVolume2);

//申买三
	 if (pMarketData->BidPrice3==DBL_MAX)
		LOG_INFO("BidPrice3:NULL");
	else
		LOG_INFO("BidPrice3:",pMarketData->BidPrice3);

	LOG_INFO("BidVolume3:",pMarketData->BidVolume3);

//申买四
	 if (pMarketData->BidPrice4==DBL_MAX)
		LOG_INFO("BidPrice4:NULL");
	else
		LOG_INFO("BidPrice4:",pMarketData->BidPrice4);

	LOG_INFO("BidVolume4:",pMarketData->BidVolume4);

//申买五
	 if (pMarketData->BidPrice5==DBL_MAX)
		LOG_INFO("BidPrice5:NULL");
	else
		LOG_INFO("BidPrice5:",pMarketData->BidPrice5);

	LOG_INFO("BidVolume5:",pMarketData->BidVolume5);

//申卖一	
	if (pMarketData->AskPrice1==DBL_MAX)
		LOG_INFO("AskPrice1:NULL");
	else
		LOG_INFO("AskPrice1:",pMarketData->AskPrice1);

	LOG_INFO("AskVolume1:",pMarketData->AskVolume1);	

//申卖二
	if (pMarketData->AskPrice2==DBL_MAX)
		LOG_INFO("AskPrice2:NULL");
	else
		LOG_INFO("AskPrice2:",pMarketData->AskPrice2);

	LOG_INFO("AskVolume2:",pMarketData->AskVolume2);

//申卖三
	if (pMarketData->AskPrice3==DBL_MAX)
		LOG_INFO("AskPrice3:NULL");
	else
		LOG_INFO("AskPrice3:",pMarketData->AskPrice3);

	LOG_INFO("AskVolume3:",pMarketData->AskVolume3);

//申卖四	
	if (pMarketData->AskPrice4==DBL_MAX)
		LOG_INFO("AskPrice4:NULL");
	else
		LOG_INFO("AskPrice4:",pMarketData->AskPrice4);

	LOG_INFO("AskVolume4:",pMarketData->AskVolume4);

//申卖五	
	if (pMarketData->AskPrice5==DBL_MAX)
		LOG_INFO("AskPrice5:NULL");
	else
		LOG_INFO("AskPrice5:",pMarketData->AskPrice5);

	LOG_INFO("AskVolume5:",pMarketData->AskVolume5);	
	
	LOG_INFO("GetDepthMarketData::end");

	m_pFemasBookManager->SendFemasarketData(pMarketData);
       StructToJSON(pMarketData);

	return;
}


void CFemasMarkrtManager::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	LOG_INFO("OnRspError:");
	LOG_INFO("ErrorCode=[",pRspInfo->ErrorID,"], ErrorMsg=[",pRspInfo->ErrorMsg,"]" );
	LOG_INFO("RequestID=[",nRequestID,"], Chain=[",bIsLast,"]");
		// 客户端需进行错误处理
       return;		
}

void CFemasMarkrtManager::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG_INFO("Sub 返回订阅合约：",pSpecificInstrument->InstrumentID);
	if(mISubSuss > 0)
	{
           mISubSuss--;
	}
	
	return;
}


 void CFemasMarkrtManager::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG_INFO("UnSub 返回订阅合约：",pSpecificInstrument->InstrumentID);
	return;
}

void CFemasMarkrtManager::OnHeartBeatWarning(int nTimeLapse)
{
      LOG_INFO("CFemasMarkrtManager::OnHeartBeatWarning ",nTimeLapse);
      return;	  
}

 void CFemasMarkrtManager::SetFileData(std::string &FileConfig)
{
      LOG_INFO("CFemasMarkrtManager::SetData ",FileConfig.c_str());
      m_pFileConfig = new fh::core::assist::Settings(FileConfig);
      if(NULL == m_pFileConfig)
      {
          LOG_ERROR("Error m_pFileConfig is NULL ");
      }
      return;	  
}


void CFemasMarkrtManager::CreateFemasBookManager(fh::core::market::MarketListenerI *sender)
{
      LOG_INFO("CFemasMarkrtManager::CreateFemasBookManager ");
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


void CFemasMarkrtManager::StructToJSON(CUstpFtdcDepthMarketDataField *pMarketData)
{
    if(NULL == pMarketData)
    {
        return;
    }
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("TradingDay", T(pMarketData->TradingDay)));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementGroupID", T(pMarketData->SettlementGroupID)));	
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementID", T(pMarketData->SettlementID)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreSettlementPrice", T(pMarketData->PreSettlementPrice==DBL_MAX ? 0.0 : pMarketData->PreSettlementPrice)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreClosePrice", T(pMarketData->PreClosePrice==DBL_MAX ? 0.0 : pMarketData->PreClosePrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("PreOpenInterest", T(pMarketData->PreOpenInterest)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreDelta", T(pMarketData->PreDelta==DBL_MAX ? 0.0 : pMarketData->PreDelta)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenPrice", T(pMarketData->OpenPrice==DBL_MAX ? 0.0 : pMarketData->OpenPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("HighestPrice", T(pMarketData->HighestPrice==DBL_MAX ? 0.0 : pMarketData->HighestPrice)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowestPrice", T(pMarketData->LowestPrice==DBL_MAX ? 0.0 : pMarketData->LowestPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("ClosePrice", T(pMarketData->ClosePrice==DBL_MAX ? 0.0 : pMarketData->ClosePrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("UpperLimitPrice", T(pMarketData->UpperLimitPrice==DBL_MAX ? 0.0 : pMarketData->UpperLimitPrice)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowerLimitPrice", T(pMarketData->LowerLimitPrice==DBL_MAX ? 0.0 : pMarketData->LowerLimitPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementPrice", T(pMarketData->SettlementPrice==DBL_MAX ? 0.0 : pMarketData->SettlementPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("CurrDelta", T(pMarketData->CurrDelta==DBL_MAX ? 0.0 : pMarketData->CurrDelta)));
    tmjson.append(bsoncxx::builder::basic::kvp("LastPrice", T(pMarketData->LastPrice==DBL_MAX ? 0.0 : pMarketData->LastPrice)));
    tmjson.append(bsoncxx::builder::basic::kvp("Volume", T(pMarketData->Volume)));
    tmjson.append(bsoncxx::builder::basic::kvp("Turnover", T(pMarketData->Turnover)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenInterest", T(pMarketData->OpenInterest)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice1", T(pMarketData->BidPrice1==DBL_MAX ? 0.0 : pMarketData->BidPrice1)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume1", T(pMarketData->BidVolume1)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice1", T(pMarketData->AskPrice1==DBL_MAX ? 0.0 : pMarketData->AskPrice1)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume1", T(pMarketData->AskVolume1)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice2", T(pMarketData->BidPrice2==DBL_MAX ? 0.0 : pMarketData->BidPrice2)));	


    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume2", T(pMarketData->BidVolume2)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice3", T(pMarketData->BidPrice3==DBL_MAX ? 0.0 : pMarketData->BidPrice3)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume3", T(pMarketData->BidVolume3)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice2", T(pMarketData->AskPrice2==DBL_MAX ? 0.0 : pMarketData->AskPrice2)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume2", T(pMarketData->AskVolume2)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice3", T(pMarketData->AskPrice3==DBL_MAX ? 0.0 : pMarketData->AskPrice3)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume3", T(pMarketData->AskVolume3)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice4", T(pMarketData->BidPrice4==DBL_MAX ? 0.0 : pMarketData->BidPrice4)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume4", T(pMarketData->BidVolume4)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice5", T(pMarketData->BidPrice5==DBL_MAX ? 0.0 : pMarketData->BidPrice5)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume5", T(pMarketData->BidVolume5)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice4", T(pMarketData->AskPrice4==DBL_MAX ? 0.0 : pMarketData->AskPrice4)));	


    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume4", T(pMarketData->AskVolume4)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice5", T(pMarketData->AskPrice5==DBL_MAX ? 0.0 : pMarketData->AskPrice5)));
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
    
	
	
    FemasDateToString(tmjson);	
    return;	
}
void CFemasMarkrtManager::FemasDateToString(bsoncxx::builder::basic::document& json)
{
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    m_pFemasBookManager->SendFemasToDB(bsoncxx::to_json(tmjson.view()));
    return;	

    //OnOrginalMessage	
}


}
}
}


