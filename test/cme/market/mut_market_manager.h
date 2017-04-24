
#ifndef __FH_CME_MUT_MARKET_MANAGER_H__
#define __FH_CME_MUT_MARKET_MANAGER_H__

#include "gtest/gtest.h"

#include "core/global.h"
#include "core/udp/udp_receiver.h"
#include "cme/market/setting/market_settings.h"
#include "cme/market/setting/channel_settings.h"
#include "cme/market/dat_replayer.h"
#include "cme/market/dat_processor.h"
#include "cme/market/dat_saver.h"
#include "cme/market/recovery_saver.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{
    class MutMarketManager : public testing::Test
    {
        public:
            MutMarketManager();
            virtual ~MutMarketManager();
        
            virtual void SetUp();
            virtual void TearDown();

        private:
            DISALLOW_COPY_AND_ASSIGN(MutMarketManager);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MUT_MARKET_MANAGER_H__
