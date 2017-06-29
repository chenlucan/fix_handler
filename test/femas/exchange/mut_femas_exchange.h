
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
#include <map>
#include "USTPFtdcTraderApi.h"
#include "core/book/book_sender.h"
#include "femas/exchange/communicator.h"
#include "core/strategy/strategy_communicator.h"


namespace fh
{
namespace femas
{
namespace exchange
{

    class CMutExchangeFemas : public testing::Test
    {

         public:
            explicit CMutExchangeFemas();
            virtual ~CMutExchangeFemas();
	
    };


} // namespace exchange
} // namespace femas
} // namespace fh


#endif