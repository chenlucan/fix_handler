#ifndef __TEST_FEMA_MARKET_CFemasMarketManager_H__
#define __TEST_FEMA_MARKET_CFemasMarketManager_H__

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

    class CMutFemasMarketManager : public testing::Test
    {

         public:
            explicit CMutFemasMarketManager();
            virtual ~CMutFemasMarketManager();
	
    };


} // namespace market
} // namespace femas
} // namespace fh


#endif