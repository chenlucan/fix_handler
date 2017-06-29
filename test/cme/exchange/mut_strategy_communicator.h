
#ifndef __FH_CORE_STRATEGY_MUT_STRATEGY_COMMUNICATOR_H__
#define __FH_CORE_STRATEGY_MUT_STRATEGY_COMMUNICATOR_H__

#include "gtest/gtest.h"

#include <string>
#include "core/global.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "cme/exchange/order_manager.h"
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/globex_logger.h"
#include "pb/ems/ems.pb.h"

#include "core/strategy/invalid_order.h"

namespace fh
{
namespace core
{
namespace strategy
{
    class MutStrategyCommunicator : public testing::Test
    {
        public:
            MutStrategyCommunicator();
            virtual ~MutStrategyCommunicator();
            
                        
            virtual void SetUp();
            virtual void TearDown();

        private:
            DISALLOW_COPY_AND_ASSIGN(MutStrategyCommunicator);
    };
} // namespace strategy
} // namespace core
} // namespace fh

#endif     // __FH_CORE_STRATEGY_MUT_STRATEGY_COMMUNICATOR_H__
