#ifndef __FH_FEMAS_MARKET_CFemasBookManager_H__
#define __FH_FEMAS_MARKET_CFemasBookManager_H__

#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "USTPFtdcMduserApi.h"


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

            void SendFemasarketData(CUstpFtdcDepthMarketDataField *pMarketData);

	  private:

            fh::core::market::MarketListenerI *m_book_sender;
		
         private:
            DISALLOW_COPY_AND_ASSIGN(CFemasBookManager);
	
    };
} // namespace market
} // namespace femas
} // namespace fh


#endif