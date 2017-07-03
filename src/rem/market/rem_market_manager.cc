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

//申卖一	
	if (pDepthQuoteData->AskPrice1==DBL_MAX)
		LOG_INFO("AskPrice1:NULL");
	else
		LOG_INFO("AskPrice1:",pDepthQuoteData->AskPrice1);

	LOG_INFO("AskVolume1:",pDepthQuoteData->AskVolume1);	
	
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
    
    tmjson.append(bsoncxx::builder::basic::kvp("AveragePrice", T(pMarketData->AveragePrice)));	
    	
    RemDateToString(tmjson,pMarketData->InstrumentID,GetUpdateTimeStr(pMarketData),GetUpdateTimeInt(pMarketData));	
    return;
}
void CRemMarkrtManager::RemDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,std::string updatetime,ullong tmp_time)
{
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("REM")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(tmp_time))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(updatetime)));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T(InstrumentID)));		
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    m_pRemBookManager->SendRemToDB(bsoncxx::to_json(tmjson.view()));
    return;
}

ullong CRemMarkrtManager::str2stmp(const char *strTime)
{
     if (strTime != NULL)
     {
         struct tm sTime;
 #ifdef __GNUC__
         strptime(strTime, "%Y-%m-%d %H:%M:%S", &sTime);
 #else
         sscanf(strTime, "%d-%d-%d %d:%d:%d", &sTime.tm_year, &sTime.tm_mon, &sTime.tm_mday, &sTime.tm_hour, &sTime.tm_min, &sTime.tm_sec);
         sTime.tm_year -= 1900;
         sTime.tm_mon -= 1;
 #endif
         ullong ft = mktime(&sTime);
         return ft;
     }
     else {
         return time(0);
     }
}

ullong CRemMarkrtManager::GetUpdateTimeInt(EESMarketDepthQuoteData *pMarketData)
{
    std::string timestr="";
    char ctmp[20]={0};	
    strncpy(ctmp,pMarketData->TradingDay,4);
    ctmp[4] = '-';
    strncpy(ctmp+5,pMarketData->TradingDay+4,2);	
    ctmp[7] = '-';
    strncpy(ctmp+8,pMarketData->TradingDay+6,2);
    ctmp[10] = ' ';
    timestr = ctmp;	
    timestr+=pMarketData->UpdateTime;
    ullong tmp_time = 0;
    tmp_time = str2stmp(timestr.c_str());	
    tmp_time *= 1000;
    tmp_time += pMarketData->UpdateMillisec;
    tmp_time *= 1000;
    tmp_time *= 1000;	
    return tmp_time;	
}

std::string CRemMarkrtManager::GetUpdateTimeStr(EESMarketDepthQuoteData *pMarketData)
{
    std::string timestr="";
    char ctmp[20]={0};	
    strncpy(ctmp,pMarketData->TradingDay,4);
    ctmp[4] = '-';
    strncpy(ctmp+5,pMarketData->TradingDay+4,2);	
    ctmp[7] = '-';
    strncpy(ctmp+8,pMarketData->TradingDay+6,2);	
    ctmp[10] = ' ';
    timestr = ctmp;	
    timestr+=pMarketData->UpdateTime;
    timestr+=".";	 
    std::string tmp = std::to_string(pMarketData->UpdateMillisec);
    if(pMarketData->UpdateMillisec != 500)
    {
        tmp = "000";
    }		
    tmp += "000";
    timestr += tmp;	
    	
    return timestr;
}




}
}
}