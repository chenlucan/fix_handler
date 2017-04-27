
#ifndef __FH_CME_MARKET_MUT_DAT_SAVER_H__
#define __FH_CME_MARKET_MUT_DAT_SAVER_H__

#include "gtest/gtest.h"

#include <set>
#include <mutex>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{
    class MutDatSaver : public testing::Test
    {
        public:
            explicit MutDatSaver();
            virtual ~MutDatSaver();
            
            virtual void SetUp();
            virtual void TearDown();

            std::vector<std::string> m_vecRevPacket;
        private:
            DISALLOW_COPY_AND_ASSIGN(MutDatSaver);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_SAVER_H__
