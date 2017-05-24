#ifndef __TEST_REM_MARKET_CRemMarket_H__
#define __TEST_REM_MARKET_CRemMarket_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "rem/market/rem_market.h"
#include "EESQuoteApi.h"
#include "core/book/book_sender.h"


namespace fh
{
namespace rem
{
namespace market
{

    class CMutRemMarket : public testing::Test
    {

         public:
            explicit CMutRemMarket();
            virtual ~CMutRemMarket();
	
    };

} // namespace market
} // namespace rem
} // namespace fh

#endif