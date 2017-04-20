


#ifndef __FH_FEMA_CFemasMarket_H__
#define __FH_FEMA_CFemasMarket_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "femas_market_manager.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"

namespace fh
{
namespace femas
{
namespace market
{



class CFemasMarket : public fh::core::market::MarketI
{
       public:
             CFemasMarket(fh::core::market::MarketListenerI *listener);
            virtual ~CFemasMarket();

        public:
            // implement of MarketI
            virtual bool Start();
            // implement of MarketI
            virtual void Initialize(std::vector<std::string> insts);
            // implement of MarketI
            virtual void Stop();
            // implement of MarketI
            virtual void Subscribe(std::vector<std::string> instruments);
            // implement of MarketI
            virtual void UnSubscribe(std::vector<std::string> instruments);
            // implement of MarketI
            virtual void ReqDefinitions(std::vector<std::string> instruments);   

	     void SetFileConfigData(std::string &FileConfig);		
        public:
	     fh::core::assist::Settings *m_pFileConfig;	
	     CUstpFtdcMduserApi *m_pUstpFtdcMduserApi;	 
	     fh::femas::market::CFemasMarkrtManager *m_FemasMarkrtManager;
};


}
}
}

#endif