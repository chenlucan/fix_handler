#ifndef __FH_CME_MARKET_MUT_BOOK_SENDER_H__
#define __FH_CME_MARKET_MUT_BOOK_SENDER_H__

#include "gtest/gtest.h"

#include <string>
#include "core/global.h"
#include "../../core/market/mock_marketi.h"
#include "core/zmq/zmq_sender.h"

namespace fh
{
namespace cme
{
namespace market
{
    class MutBookSender : public testing::Test
    {
        public:
            MutBookSender();
            virtual ~MutBookSender();

        public:            
            void OnMarketDisconnect_Test001();
        private:
            DISALLOW_COPY_AND_ASSIGN(MutBookSender);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MUT_BOOK_SENDER_H__
