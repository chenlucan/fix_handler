#ifndef __FH_REM_CRemMarket_H__
#define __FH_REM_CRemMarket_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "rem_market_manager.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"

namespace fh
{
namespace rem
{
namespace market
{



class CRemMarket : public fh::core::market::MarketI
{
       public:
             CRemMarket(fh::core::market::MarketListenerI *listener);
            virtual ~CRemMarket();

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

	     bool GetReatart()
	     {
                return m_RemMarkrtManager->GetReatart();
	     }
		 
        public:
	     fh::core::assist::Settings *m_pFileConfig;	
	     EESQuoteApi *m_pEESQuoteApi;	 
	     fh::rem::market::CRemMarkrtManager *m_RemMarkrtManager; 

	     int m_itimeout;	 
		 
};



}
}
}

#endif