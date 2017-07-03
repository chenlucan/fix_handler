

#ifndef __FH_CTP_MARKET_CTP_MARKET_APPLICATION_H__
#define __FH_CTP_MARKET_CTP_MARKET_APPLICATION_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "custom_manager.h"

#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "core/book/book_sender.h"
#include <fstream>
#include "MDAccountID.h"

namespace fh
{
namespace ctp
{
namespace market
{


class CCtpMarketApp
{

	public:
		CCtpMarketApp(std::string &FileConfig);

		void Initialize(std::vector<std::string> insts);

		bool Start();

		void Stop();

		void Subscribe(std::vector<std::string> instruments);

		void UnSubscribe(std::vector<std::string> instruments);

		void ReqDefinitions(std::vector<std::string> instruments); 	  
		
	public:
	    std::shared_ptr<fh::ctp::market::CustomManager> m_pCtpManager;
        std::shared_ptr<fh::core::book::BookSender> m_book_sender;		

	private:
	    std::string m_instrumentFileDir;
		std::vector<std::string> m_insts;
		
	private:
	    DISALLOW_COPY_AND_ASSIGN(CCtpMarketApp);		

};







}
}
}


#endif