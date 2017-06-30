#ifndef __FH_CTP_MARKET_H__
#define __FH_CTP_MARKET_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "custom_md_spi.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "core/book/book_sender.h"
#include "MDWrapper.h"
#include "MDAccountID.h"

namespace fh
{
namespace ctp
{
namespace market
{

class CustomManager : public fh::core::market::MarketI
{
	public:
		CustomManager(std::shared_ptr<fh::core::market::MarketListenerI> listener,  std::shared_ptr<fh::ctp::market::MDAccountID> id);
		virtual ~CustomManager();

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
		
	public:	
		int m_itimeout;
	   	std::vector<std::string> m_insts;		
		std::shared_ptr<fh::ctp::market::MDWrapper> m_ctpmaket;
		
    private:
        bool subscribeOK;	
		std::shared_ptr<fh::ctp::market::MDAccountID> id;
		
	private:
		DISALLOW_COPY_AND_ASSIGN(CustomManager);		 
		 
};


}
}
}

#endif