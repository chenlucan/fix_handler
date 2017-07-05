
#ifndef __TEST_CTP_EXCHANGE_CTP_H__
#define __TEST_CTP_EXCHANGE_CTP_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include <map>
#include "ThostFtdcTraderApi.h"
#include "core/book/book_sender.h"
#include "ctp/exchange/ctpcommunicator.h"
#include "core/strategy/strategy_communicator.h"


namespace fh
{
namespace ctp
{
namespace exchange
{

    class CMutExchangeCtp : public testing::Test
    {

         public:
            explicit CMutExchangeCtp();
            virtual ~CMutExchangeCtp();
	
    };


} // namespace exchange
} // namespace femas
} // namespace fh


#endif
