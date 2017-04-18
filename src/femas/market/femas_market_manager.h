


#ifndef __FH_FEMAS_MARKET_CFemasMarkrtManager_H__
#define __FH_FEMAS_MARKET_CFemasMarkrtManager_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "USTPFtdcMduserApi.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "femas/market/femas_book_manager.h"

namespace fh
{
namespace femas
{
namespace market
{

class CFemasMarkrtManager : public CUstpFtdcMduserSpi
{
    public:
		CFemasMarkrtManager(CUstpFtdcMduserApi *pUserApi) : m_pUserApi(pUserApi) {m_pFileConfig = NULL;}
		~CFemasMarkrtManager()
		{
                  if(NULL != m_pFileConfig)
                  {
                        delete m_pFileConfig;
			   m_pFileConfig = NULL;
		    }
		}

		void OnFrontConnected();
		void OnFrontDisconnected();
		void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData) ;
		void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		//增加初始化接口
		void SetFileData(std::string &FileConfig);

		void CreateFemasBookManager(fh::core::market::MarketListenerI *sender);

		
    private:
		CUstpFtdcMduserApi *m_pUserApi;
		fh::core::assist::Settings *m_pFileConfig;

		fh::femas::market::CFemasBookManager* m_pFemasBookManager;
};












}
}
}

#endif