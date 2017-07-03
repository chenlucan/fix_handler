#ifndef __TEST_CTP_MARKET_CMutCtpMarket_H__
#define __TEST_CTP_MARKET_CMutCtpMarket_H__

#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "ThostFtdcTraderApi.h"
#include "ctp/market/custom_manager.h"
#include "core/book/book_sender.h"
#include "ctp/market/MDAccountID.h"

namespace fh
{
namespace ctp
{
namespace market
{

    class CMutCustomManager : public testing::Test
    {

         public:
            explicit CMutCustomManager();
            virtual ~CMutCustomManager();
	
    };


} // namespace market
} // namespace femas
} // namespace fh


#endif