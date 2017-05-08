


#ifndef __FH_FEMAS_MARKET_CFemasMarkrtManager_H__
#define __FH_FEMAS_MARKET_CFemasMarkrtManager_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "USTPFtdcMduserApi.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "femas/market/femas_book_manager.h"
#include "core/assist/utility.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/global.h"

namespace fh
{
namespace femas
{
namespace market
{



template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

class CFemasMarketManager : public CUstpFtdcMduserSpi
{
    public:
		CFemasMarketManager(CUstpFtdcMduserApi *pUserApi) : m_pUserApi(pUserApi) 
		{
		    m_pFileConfig = NULL;
		    m_pFemasBookManager = NULL;	
		    mIConnet = -1;	
		    mISubSuss = -1;	
		}
		virtual ~CFemasMarketManager()
		{
                  if(NULL != m_pFileConfig)
                  {
                        delete m_pFileConfig;
			   m_pFileConfig = NULL;
		    }
	           delete m_pFemasBookManager;			  
		}

		virtual void OnFrontConnected();
		virtual void OnFrontDisconnected();
		virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData) ;
		virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnHeartBeatWarning(int nTimeLapse);

		//增加初始化接口
		void SetFileData(std::string &FileConfig);

		void CreateFemasBookManager(fh::core::market::MarketListenerI *sender);

		void StructToJSON(CUstpFtdcDepthMarketDataField *pMarketData);
		void FemasDateToString(bsoncxx::builder::basic::document& json);

		int mIConnet;
		int mISubSuss;

		
    private:
		CUstpFtdcMduserApi *m_pUserApi;
		fh::core::assist::Settings *m_pFileConfig;

		fh::femas::market::CFemasBookManager* m_pFemasBookManager;
};












}
}
}

#endif