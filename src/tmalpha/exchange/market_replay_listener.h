
#ifndef __FH_TMALPHA_EXCHANGE_MARKET_REPLAY_LISTENER_H__
#define __FH_TMALPHA_EXCHANGE_MARKET_REPLAY_LISTENER_H__

#include <cstdint>
#include <vector>
#include <unordered_map>
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "pb/dms/dms.pb.h"
#include "core/book/book_sender.h"
#include "core/market/marketi.h"


namespace fh
{
namespace tmalpha
{
namespace exchange
{
    class MarketReplayListener : public fh::core::market::MarketListenerI
    {
        public:
            MarketReplayListener(const std::string &org_url, const std::string &book_url)
            : m_book_sender(org_url, book_url), m_on_l2_changed() {}
            virtual ~MarketReplayListener() {}

        public:
            void Add_l2_changed_callback(std::function<void(const pb::dms::L2 &)> on_l2_changed)
            {
                m_on_l2_changed = on_l2_changed;
            }

        public:
            // implement of MarketListenerI
            void OnMarketDisconnect(fh::core::market::MarketI* market) override
            {
                m_book_sender.OnMarketDisconnect(market);
            }

            // implement of MarketListenerI
            void OnMarketReconnect(fh::core::market::MarketI* market) override
            {
                m_book_sender.OnMarketReconnect(market);
            }

            // implement of MarketListenerI
            void OnContractDefinition(const pb::dms::Contract &contract) override
            {
                m_book_sender.OnContractDefinition(contract);
            }

            // implement of MarketListenerI
            void OnBBO(const pb::dms::BBO &bbo) override
            {
                m_book_sender.OnBBO(bbo);
            }

            // implement of MarketListenerI
            void OnBid(const pb::dms::Bid &bid) override
            {
                m_book_sender.OnBid(bid);
            }

            // implement of MarketListenerI
            void OnOffer(const pb::dms::Offer &offer) override
            {
                m_book_sender.OnOffer(offer);
            }

            // implement of MarketListenerI
            void OnL2(const pb::dms::L2 &l2) override
            {
                m_book_sender.OnL2(l2);
                if(m_on_l2_changed) m_on_l2_changed(l2);
            }

            // implement of MarketListenerI
            void OnL3() override
            {
                m_book_sender.OnL3();
            }

            // implement of MarketListenerI
            void OnTrade(const pb::dms::Trade &trade) override
            {
                m_book_sender.OnTrade(trade);
            }

            // implement of MarketListenerI
            void OnContractAuctioning(const std::string &contract) override
            {
                m_book_sender.OnContractAuctioning(contract);
            }

            // implement of MarketListenerI
            void OnContractNoTrading(const std::string &contract) override
            {
                m_book_sender.OnContractNoTrading(contract);
            }

            // implement of MarketListenerI
            void OnContractTrading(const std::string &contract) override
            {
                m_book_sender.OnContractTrading(contract);
            }

            // implement of MarketListenerI
            void OnOrginalMessage(const std::string &message) override
            {
                m_book_sender.OnOrginalMessage(message);
            }

        private:
            fh::core::book::BookSender m_book_sender;
            std::function<void(const pb::dms::L2 &)> m_on_l2_changed;
    };
}   // namespace exchange
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_EXCHANGE_MARKET_REPLAY_LISTENER_H__
