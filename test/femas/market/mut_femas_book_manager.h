#ifndef __TEST_FEMA_MARKET_CFemasBookManger_H__
#define __TEST_FEMA_MARKET_CFemasBookManger_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
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
    class CMutFemasBookManager : public testing::Test
    {

         public:
            explicit CMutFemasBookManager(fh::core::market::MarketListenerI *sender);
            virtual ~CMutFemasBookManager();

            virtual void SendMutFemasmarketData(CUstpFtdcDepthMarketDataField *pMarketData);
	     virtual void SendMutFemasToDB(const std::string &message);		

	  private:

            fh::core::market::MarketListenerI *m_book_sender;
		
         private:
            DISALLOW_COPY_AND_ASSIGN(CMutFemasBookManager);
	
    };
} // namespace market
} // namespace femas
} // namespace fh


#endif

