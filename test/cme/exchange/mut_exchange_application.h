
#ifndef __FH_CME_EXCHANGE_MUT_EXCHANGE_APPLICATION_H__
#define __FH_CME_EXCHANGE_MUT_EXCHANGE_APPLICATION_H__

#include "gtest/gtest.h"

#include <string>
#include "core/global.h"
#include "core/strategy/strategy_communicator.h"
#include "cme/exchange/globex_communicator.h"


namespace fh
{
namespace cme
{
namespace exchange
{
    class MutExchangeApplication : public testing::Test
    {
        public:
            explicit MutExchangeApplication();
            virtual ~MutExchangeApplication();
            
            virtual void SetUp();
            virtual void TearDown();

        
        private:
            DISALLOW_COPY_AND_ASSIGN(MutExchangeApplication);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_MUT_EXCHANGE_APPLICATION_H__
