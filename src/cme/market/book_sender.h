#ifndef __FH_CME_MARKET_BOOK_SENDER_H__
#define __FH_CME_MARKET_BOOK_SENDER_H__

#include <string>
#include "core/global.h"
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
            // implement of MarketListenerI
            virtual void OnMarketDisconnect(core::market::MarketI* market);
            // implement of MarketListenerI
            virtual void OnMarketReconnect(core::market::MarketI* market);
            // implement of MarketListenerI
            virtual void OnContractDefinition(const pb::dms::Contract &contract);
            // implement of MarketListenerI
            virtual void OnBBO(const pb::dms::BBO &bbo);
            // implement of MarketListenerI
            virtual void OnBid(const pb::dms::Bid &bid);
            // implement of MarketListenerI
            virtual void OnOffer(const pb::dms::Offer &offer);
            // implement of MarketListenerI
            virtual void OnL2(const pb::dms::L2 &l2);
            // implement of MarketListenerI
            virtual void OnL3();
            // implement of MarketListenerI
            virtual void OnTrade(const pb::dms::Trade &trade);

        private:
            fh::core::zmq::ZmqSender m_sender;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookSender);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_SENDER_H__
