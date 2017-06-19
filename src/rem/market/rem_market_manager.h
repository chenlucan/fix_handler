#ifndef __FH_REM_MARKET_REM_MARKET_MANAGER_H__
#define __FH_REM_MARKET_REM_MARKET_MANAGER_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "EESQuoteApi.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "rem/market/rem_book_manager.h"
#include "core/assist/utility.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/global.h"
#include <atomic>
#include <time.h>
#include <ctime>
#include <stdlib.h>

typedef unsigned long long  ullong;

namespace fh
{
namespace rem
{
namespace market
{



template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}




class CRemMarkrtManager : public EESQuoteEvent
{
    public:
		CRemMarkrtManager(EESQuoteApi *pUserApi) : m_pUserApi(pUserApi) 
		{
		    m_pFileConfig = NULL;
		    m_pRemBookManager = NULL;	
		    mIConnet = -1;	
		    mISubSuss = -1;
		    m_SubSymbol.clear();	
		    m_restart = false;	
		}
		virtual ~CRemMarkrtManager()
		{
                  if(NULL != m_pFileConfig)
                  {
                        delete m_pFileConfig;
			   m_pFileConfig = NULL;
		    }
	           delete m_pRemBookManager;			  
		}

		virtual void OnEqsConnected();
		virtual void OnEqsDisconnected();
		virtual void OnLoginResponse(bool bSuccess, const char* pReason);
		virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);
		virtual void OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen);
		virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
		virtual void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
		virtual void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast);

		//增加初始化接口
		void SetFileData(std::string &FileConfig);

		void CreateRemBookManager(fh::core::market::MarketListenerI *sender);

		void StructToJSON(EESMarketDepthQuoteData *pMarketData);
		void RemDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,std::string updatetime,ullong tmp_time);
		std::string GetUpdateTimeStr(EESMarketDepthQuoteData *pMarketData);
		ullong str2stmp(const char *strTime);
		ullong GetUpdateTimeInt(EESMarketDepthQuoteData *pMarketData);

		bool GetReatart()
		{
                   return m_restart;
		}

		std::atomic_int mIConnet;
		std::atomic_int mISubSuss;
		std::vector<std::string> m_SubSymbol;

              bool m_restart;
		
    private:
		EESQuoteApi *m_pUserApi;
		fh::core::assist::Settings *m_pFileConfig;

		fh::rem::market::CRemBookManager* m_pRemBookManager;
};







}
}
}

#endif