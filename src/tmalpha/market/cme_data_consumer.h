
#ifndef __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__
#define __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__

#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include "core/global.h"
#include "core/assist/logger.h"
#include "core/market/marketlisteneri.h"
#include "cme/market/playback/book_replayer.h"
#include "cme/market/message/message_utility.h"
#include "tmalpha/market/data_consumer.h"


namespace fh
{
namespace tmalpha
{
namespace market
{
    class CmeDataConsumer : public DataConsumer
    {
        public:
            CmeDataConsumer() : m_replayer()
            {
                // noop
            }

            virtual ~CmeDataConsumer()
            {
                // noop
            }

        public:
            void Add_listener(fh::core::market::MarketListenerI *listener) override
            {
                m_replayer.Add_listener(listener);
            }

            void Consume(const std::string &message) override
            {
                m_replayer.Apply_message(message);
            }

        private:
            fh::cme::market::playback::BookReplayer m_replayer;

        private:
            DISALLOW_COPY_AND_ASSIGN(CmeDataConsumer);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__
