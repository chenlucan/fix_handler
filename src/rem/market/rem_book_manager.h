#ifndef __FH_REM_MARKET_REM_BOOK_MANAGER_H__
#define __FH_REM_MARKET_REM_BOOK_MANAGER_H__

#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "EESQuoteApi.h"

#include <stdio.h> 
#include <unistd.h>     
#include <signal.h>               
#include <sys/time.h> 

typedef struct strade
{
    int mvolume;
    std::string mtime;
    strade()
    {
        mvolume = 0;
	 mtime = "";	
    }
} mstrade;

typedef std::map <std::string,mstrade*> TradeMap;

namespace fh
{
namespace rem
{
namespace market
{

    class CRemBookManager
    {

         public:
            explicit CRemBookManager(fh::core::market::MarketListenerI *sender);
            virtual ~CRemBookManager();

            void SendRemmarketData(EESMarketDepthQuoteData *pMarketData);
	     void SendRemToDB(const std::string &message);
	     int MakePriceVolume(EESMarketDepthQuoteData *pMarketData);	 
	     void CheckTime(EESMarketDepthQuoteData *pMarketData);	 

	  private:

            fh::core::market::MarketListenerI *m_book_sender;
	     TradeMap m_trademap;	 		
		
         private:
            DISALLOW_COPY_AND_ASSIGN(CRemBookManager);
	
    };



} // namespace market
} // namespace rem
} // namespace fh


#endif 