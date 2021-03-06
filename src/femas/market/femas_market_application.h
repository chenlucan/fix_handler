

#ifndef __FH_FEMA_MARKET_FEMAS_MARKET_APPLICATION_H__
#define __FH_FEMA_MARKET_FEMAS_MARKET_APPLICATION_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "femas_market.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "core/book/book_sender.h"


namespace fh
{
namespace femas
{
namespace market
{


class CFemasMarketApp
{

    public:
		CFemasMarketApp(CFemasMarket *pFemasMarket);
		CFemasMarketApp(std::string &save_url_f,std::string &save_url_s);
		~CFemasMarketApp();

              void Initialize(std::vector<std::string> insts);
			  
              bool Start();

              void Stop();

	       void Subscribe(std::vector<std::string> instruments);

              void UnSubscribe(std::vector<std::string> instruments);

              void ReqDefinitions(std::vector<std::string> instruments); 	  
		
		void SetFileConfigData(std::string &FileConfig);
    public:
	       CFemasMarket *m_pFemasMarket;	
		fh::core::assist::Settings *m_pFileConfig;
		fh::core::book::BookSender *m_book_sender;
    private:
            DISALLOW_COPY_AND_ASSIGN(CFemasMarketApp);		

};







}
}
}


#endif