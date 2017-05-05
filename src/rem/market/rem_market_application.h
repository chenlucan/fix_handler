#ifndef __FH_REM_MARKET_CRemMarketApp_H__
#define __FH_REM_MARKET_CRemMarketApp_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "rem_market.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "core/book/book_sender.h"

namespace fh
{
namespace rem
{
namespace market
{

class CRemMarketApp
{

    public:
		CRemMarketApp(CRemMarket *pRemMarket);
		CRemMarketApp(std::string &save_url_f,std::string &save_url_s);
		~CRemMarketApp();

              void Initialize(std::vector<std::string> insts);
			  
              bool Start();

              void Stop();

	       void Subscribe(std::vector<std::string> instruments);

              void UnSubscribe(std::vector<std::string> instruments);

              void ReqDefinitions(std::vector<std::string> instruments); 	  
		
		void SetFileConfigData(std::string &FileConfig);

		bool GetReatart()
		{
                   return m_pRemMarket->GetReatart();
		}
    public:
	       CRemMarket *m_pRemMarket;	
		fh::core::assist::Settings *m_pFileConfig;
		fh::core::book::BookSender *m_book_sender;

};



}
}
}


#endif