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
            void OnMarketDisconnect(core::market::MarketI* market) override;
            // implement of MarketListenerI
            void OnMarketReconnect(core::market::MarketI* market) override;
            // implement of MarketListenerI
            void OnContractDefinition(const pb::dms::Contract &contract) override;
            // implement of MarketListenerI
            void OnBBO(const pb::dms::BBO &bbo) override;
            // implement of MarketListenerI
            void OnBid(const pb::dms::Bid &bid) override;
            // implement of MarketListenerI
            void OnOffer(const pb::dms::Offer &offer) override;
            // implement of MarketListenerI
            void OnL2(const pb::dms::L2 &l2) override;
            // implement of MarketListenerI
            void OnL3() override;
            // implement of MarketListenerI
            void OnTrade(const pb::dms::Trade &trade) override;
            void OnContractAuctioning(std::string contract) override;
            void OnContractNoTrading(std::string contract)  override;
            void OnContractTrading(std::string contract)    override;

        private:
            fh::core::zmq::ZmqSender m_sender;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookSender);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_SENDER_H__
