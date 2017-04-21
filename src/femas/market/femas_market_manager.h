


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
		CFemasMarkrtManager(CUstpFtdcMduserApi *pUserApi) : m_pUserApi(pUserApi) 
		{
		    m_pFileConfig = NULL;
		    mIConnet = -1;	
		    mISubSuss = -1;	
		}
		virtual ~CFemasMarkrtManager()
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