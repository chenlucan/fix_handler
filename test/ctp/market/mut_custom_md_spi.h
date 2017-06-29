#ifndef __TEST_CTP_MARKET_CCTPMarketManager_H__
#define __TEST_CTP_MARKET_CCTPMarketManager_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "ThostFtdcMdApi.h"


namespace fh
{
namespace ctp
{
namespace market
{

    class CMutCustomMdSpi : public testing::Test
    {

         public:
            explicit CMutCustomMdSpi();
            virtual ~CMutCustomMdSpi();
	
    };


} // namespace market
} // namespace femas
} // namespace fh


#endif