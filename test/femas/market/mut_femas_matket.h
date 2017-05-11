#ifndef __TEST_FEMA_MARKET_CFemasMarket_H__
#define __TEST_FEMA_MARKET_CFemasMarket_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "USTPFtdcMduserApi.h"
#include "femas/market/femas_market.h"


namespace fh
{
namespace femas
{
namespace market
{

    class CMutFemasMarket : public testing::Test
    {

         public:
            explicit CMutFemasMarket();
            virtual ~CMutFemasMarket();
	
    };


} // namespace market
} // namespace femas
} // namespace fh


#endif