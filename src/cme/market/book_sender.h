#ifndef __FH_CME_MARKET_BOOK_SENDER_H__
#define __FH_CME_MARKET_BOOK_SENDER_H__

#include <string>
#include "core/market/marketlisteneri.h"
#include "core/zmq/zmq_sender.h"

namespace fh
{
namespace cme
{
namespace market
{
    class BookSender : public fh::core::market::MarketListenerI
    {
        public:
            explicit BookSender(const std::string &url);
            virtual ~BookSender();

        public:
            void Send(const std::string &message);

        public:
            // implement of MarketListenerI
            virtual void OnMarketDisconnect(core::market::MarketI* market);
            // implement of MarketListenerI
            virtual void OnMarketReconnect(core::market::MarketI* market);
            // implement of MarketListenerI
            virtual void OnContractDefinition();
            // implement of MarketListenerI
            virtual void OnBBO();
            // implement of MarketListenerI
            virtual void OnBid();
            // implement of MarketListenerI
            virtual void OnOffer();
            // implement of MarketListenerI
            virtual void OnL2();
            // implement of MarketListenerI
            virtual void OnL3();
            // implement of MarketListenerI
            virtual void OnTrade();

        private:
            fh::core::zmq::ZmqSender sender;
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_SENDER_H__
