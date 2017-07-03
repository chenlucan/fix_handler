#ifndef __FH_FEMAS_MARKET_FEMAS_BOOK_MANAGER_H__
#define __FH_FEMAS_MARKET_FEMAS_BOOK_MANAGER_H__

#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "USTPFtdcMduserApi.h"

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
namespace femas
{
namespace market
{
    class CFemasBookManager
    {

         public:
            explicit CFemasBookManager(fh::core::market::MarketListenerI *sender);
            virtual ~CFemasBookManager();

            void SendFemasmarketData(CUstpFtdcDepthMarketDataField *pMarketData);
	     void SendFemasToDB(const std::string &message);	
	     int MakePriceVolume(CUstpFtdcDepthMarketDataField *pMarketData);	
	     //void ClearMap();	
	     void CheckTime(CUstpFtdcDepthMarketDataField *pMarketData);	 

	  private:
            
            fh::core::market::MarketListenerI *m_book_sender;
	     TradeMap m_trademap;	  
		
         private:
            DISALLOW_COPY_AND_ASSIGN(CFemasBookManager);
	
    };
} // namespace market
} // namespace femas
} // namespace fh


#endif