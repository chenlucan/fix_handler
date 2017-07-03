
#ifndef __FH_CME_EXCHANGE_MUT_GLOBEX_COMMUNICATOR_H__
#define __FH_CME_EXCHANGE_MUT_GLOBEX_COMMUNICATOR_H__

#include "gtest/gtest.h"

#include <string>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include "core/global.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "cme/exchange/order_manager.h"
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/globex_logger.h"
#include "pb/ems/ems.pb.h"

#include "core/strategy/invalid_order.h"
#include "../../core/strategy/mock_strategy_communicator.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class MutGlobexCommunicator : public testing::Test
    {
        public:
            MutGlobexCommunicator();
            virtual ~MutGlobexCommunicator();
            
                        
            virtual void SetUp();
            virtual void TearDown();

        private:
            DISALLOW_COPY_AND_ASSIGN(MutGlobexCommunicator);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_MUT_GLOBEX_COMMUNICATOR_H__
