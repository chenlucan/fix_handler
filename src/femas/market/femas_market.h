


#ifndef __FH_FEMA_FEMAS_MARKET_H__
#define __FH_FEMA_FEMAS_MARKET_H__

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
            bool Start() override;
            // implement of MarketI
            void Initialize(std::vector<std::string> insts) override;
            // implement of MarketI
            void Stop() override;
            // implement of MarketI
            void Subscribe(std::vector<std::string> instruments) override;
            // implement of MarketI
            void UnSubscribe(std::vector<std::string> instruments) override;
            // implement of MarketI
            void ReqDefinitions(std::vector<std::string> instruments) override; 

	     void SetFileConfigData(std::string &FileConfig);	
        public:
	     fh::core::assist::Settings *m_pFileConfig;	
	     CUstpFtdcMduserApi *m_pUstpFtdcMduserApi;	 
	     fh::femas::market::CFemasMarketManager *m_FemasMarketManager; 

	     int m_itimeout;
	     std::vector<std::string> m_insts;	
		 
	private:
            DISALLOW_COPY_AND_ASSIGN(CFemasMarket);		 
		 
};


}
}
}

#endif