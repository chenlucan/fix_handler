#ifndef __TEST_REM_MARKET_CRemExchange_H__
#define __TEST_REM_MARKET_CRemExchange_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include <map>
#include "EesTraderApi.h"
#include "core/book/book_sender.h"
#include "rem/exchange/rem_communicator.h"
#include "core/strategy/strategy_communicator.h"


namespace fh
{
namespace rem
{
namespace exchange
{

    class CMutRemExchange : public testing::Test
    {

         public:
            explicit CMutRemExchange();
            virtual ~CMutRemExchange();
	
    };

} // namespace exchange
} // namespace rem
} // namespace fh

#endif