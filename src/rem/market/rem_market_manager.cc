#include "core/assist/logger.h"
#include "rem_market_manager.h"


namespace fh
{
namespace rem
{
namespace market
{


void CRemMarkrtManager::OnEqsConnected()
{
    LOG_INFO("CRemMarkrtManager::OnEqsConnected.");
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
    EqsLoginParam temp;
    std::string LogIDstr = m_pFileConfig->Get("rem-user.LogID");
    std::string Passwordstr = m_pFileConfig->Get("rem-user.Password");
    LOG_INFO("LogIDstr = ",LogIDstr);
    LOG_INFO("Passwordstr = ",Passwordstr);	
    strcpy(temp.m_loginId, LogIDstr.c_str());
    strcpy(temp.m_password, Passwordstr.c_str());	
    m_pUserApi->LoginToEqs(temp);
    return;	
}
void CRemMarkrtManager::OnEqsDisconnected()
{
    LOG_INFO("OnEqsDisconnected.");
    m_restart = true;	
    return;
}
void CRemMarkrtManager::OnLoginResponse(bool bSuccess, const char* pReason)
{
    LOG_INFO("OnLoginResponse");
    if(!bSuccess)
    {
        LOG_ERROR("Failed to login, errormsg=",pReason);
	 mIConnet = 1;	
	 return;	
    }
    mIConnet = 0;
    return;	
}
void CRemMarkrtManager::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData)
{
    LOG_INFO("OnQuoteUpdated begin");
    LOG_INFO("EESMarketDepthQuoteData:");
    LOG_INFO("name : ",pDepthQuoteData->InstrumentID);	
    LOG_INFO("TradingDay : ",pDepthQuoteData->TradingDay);	
//申买一
       if (pDepthQuoteData->BidPrice1==DBL_MAX)
		LOG_INFO("BidPrice1:NULL");
	else
		LOG_INFO("BidPrice1:",pDepthQuoteData->BidPrice1);

	LOG_INFO("BidVolume1: ",pDepthQuoteData->BidVolume1);

//申买二
	 if (pDepthQuoteData->BidPrice2==DBL_MAX)
		LOG_INFO("BidPrice2:NULL");
	else
		LOG_INFO("BidPrice2:",pDepthQuoteData->BidPrice2);

	LOG_INFO("BidVolume2:",pDepthQuoteData->BidVolume2);

//申买三
	 if (pDepthQuoteData->BidPrice3==DBL_MAX)
		LOG_INFO("BidPrice3:NULL");
	else
		LOG_INFO("BidPrice3:",pDepthQuoteData->BidPrice3);

	LOG_INFO("BidVolume3:",pDepthQuoteData->BidVolume3);

//申买四
	 if (pDepthQuoteData->BidPrice4==DBL_MAX)
		LOG_INFO("BidPrice4:NULL");
	else
		LOG_INFO("BidPrice4:",pDepthQuoteData->BidPrice4);

	LOG_INFO("BidVolume4:",pDepthQuoteData->BidVolume4);

//申买五
	 if (pDepthQuoteData->BidPrice5==DBL_MAX)
		LOG_INFO("BidPrice5:NULL");
	else
		LOG_INFO("BidPrice5:",pDepthQuoteData->BidPrice5);

	LOG_INFO("BidVolume5:",pDepthQuoteData->BidVolume5);

//申卖一	
	if (pDepthQuoteData->AskPrice1==DBL_MAX)
		LOG_INFO("AskPrice1:NULL");
	else
		LOG_INFO("AskPrice1:",pDepthQuoteData->AskPrice1);

	LOG_INFO("AskVolume1:",pDepthQuoteData->AskVolume1);	

//申卖二
	if (pDepthQuoteData->AskPrice2==DBL_MAX)
		LOG_INFO("AskPrice2:NULL");
	else
		LOG_INFO("AskPrice2:",pDepthQuoteData->AskPrice2);

	LOG_INFO("AskVolume2:",pDepthQuoteData->AskVolume2);

//申卖三
	if (pDepthQuoteData->AskPrice3==DBL_MAX)
		LOG_INFO("AskPrice3:NULL");
	else
		LOG_INFO("AskPrice3:",pDepthQuoteData->AskPrice3);

	LOG_INFO("AskVolume3:",pDepthQuoteData->AskVolume3);

//申卖四	
	if (pDepthQuoteData->AskPrice4==DBL_MAX)
		LOG_INFO("AskPrice4:NULL");
	else
		LOG_INFO("AskPrice4:",pDepthQuoteData->AskPrice4);

	LOG_INFO("AskVolume4:",pDepthQuoteData->AskVolume4);

//申卖五	
	if (pDepthQuoteData->AskPrice5==DBL_MAX)
		LOG_INFO("AskPrice5:NULL");
	else
		LOG_INFO("AskPrice5:",pDepthQuoteData->AskPrice5);

	LOG_INFO("AskVolume5:",pDepthQuoteData->AskVolume5);	
	
	LOG_INFO("OnQuoteUpdated::end");	

       m_pRemBookManager->SendRemmarketData(pDepthQuoteData);
       StructToJSON(pDepthQuoteData);
	
    return;	
}
void CRemMarkrtManager::OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen)
{
    LOG_INFO("OnWriteTextLog:",pLogText);
}
void CRemMarkrtManager::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
    if (bSuccess)
    {
        LOG_INFO("SubSymbol :", pSymbol);
	 if(mISubSuss > 0)
	 {
           mISubSuss--;
	 }	
    }
    else
    {
	 LOG_ERROR("SubSymbol :", pSymbol,"fail");
    }
}
void CRemMarkrtManager::OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
    if (bSuccess)
    {
        LOG_INFO("UnsubSymbol :", pSymbol,"Success");
    }
    else
    {
	 LOG_ERROR("UnsubSymbol :", pSymbol,"fail");
    }
}
void CRemMarkrtManager::OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast)
{

    	std::string strSymbol = pSymbol;
	if (!strSymbol.empty())
	{
	       LOG_INFO("subSymbol : ",pSymbol);
	       if(m_SubSymbol.size()>0)
		{
		    if(std::strcmp(m_SubSymbol[0].c_str(),"*") == 0)
		    {
                      m_pUserApi->RegisterSymbol(chInstrumentType, pSymbol);
		    }
		    else
		    {
                      std::vector<std::string>::iterator result = std::find( m_SubSymbol.begin( ), m_SubSymbol.end( ),pSymbol);  
		        if(result != m_SubSymbol.end( ))
		        {
		            LOG_INFO("My SubSymbol :", pSymbol);
                          m_pUserApi->RegisterSymbol(chInstrumentType, pSymbol);
		        }
		    }
                  
		}
		else
	       {
                  
                  m_pUserApi->RegisterSymbol(chInstrumentType, pSymbol);
		}
		
	}

	if (bLast)
	{
		LOG_INFO("OnSymbolListResponse is over");
	}
 
}

//增加初始化接口
void CRemMarkrtManager::SetFileData(std::string &FileConfig)
{
    LOG_INFO("CRemMarkrtManager::SetData ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig);
    if(NULL == m_pFileConfig)
    {
        LOG_ERROR("Error m_pFileConfig is NULL ");
    }
    return;
}

void CRemMarkrtManager::CreateRemBookManager(fh::core::market::MarketListenerI *sender)
{
      LOG_INFO("CRemMarkrtManager::CreateRemBookManager ");
      if(NULL != m_pRemBookManager)
      {
          delete m_pRemBookManager;
          m_pRemBookManager = NULL;
      }
      
      m_pRemBookManager = new fh::rem::market::CRemBookManager(sender);
      if(NULL == m_pRemBookManager)
      {
          LOG_ERROR("Error m_pRemBookManager is NULL ");
      }
      return;
}

void CRemMarkrtManager::StructToJSON(EESMarketDepthQuoteData *pMarketData)
{
    if(NULL == pMarketData)
    {
        return;
    }
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("TradingDay", T(pMarketData->TradingDay)));
    tmjson.append(bsoncxx::builder::basic::kvp("ExchangeID", T(pMarketData->ExchangeID)));
    tmjson.append(bsoncxx::builder::basic::kvp("ExchangeInstID", T(pMarketData->ExchangeInstID)));	
    
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
    
    tmjson.append(bsoncxx::builder::basic::kvp("AveragePrice", T(pMarketData->AveragePrice==DBL_MAX ? 0.0 : pMarketData->AveragePrice)));	
    	
    RemDateToString(tmjson);	
    return;
}
void CRemMarkrtManager::RemDateToString(bsoncxx::builder::basic::document& json)
{
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("REM")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    m_pRemBookManager->SendRemToDB(bsoncxx::to_json(tmjson.view()));
    return;
}






}
}
}