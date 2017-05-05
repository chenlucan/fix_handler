
#ifndef __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__
#define __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__

#include <cstdint>
#include <vector>
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "pb/dms/dms.pb.h"
#include "core/market/marketlisteneri.h"


namespace fh
{
namespace tmalpha
{
namespace market
{
    class MockMarketReplayListener : public fh::core::market::MarketListenerI
    {
        public:
            MockMarketReplayListener() : m_contracts(), m_bbos(), m_bids(), m_offers(), m_l2s(), m_trades(), m_status() {}
            virtual ~MockMarketReplayListener() {}

        public:
            // implement of MarketListenerI
            void OnMarketDisconnect(core::market::MarketI* market)
            {
                // noop
            }

            // implement of MarketListenerI
            void OnMarketReconnect(core::market::MarketI* market)
            {
                // noop
            }

            // implement of MarketListenerI
            void OnContractDefinition(const pb::dms::Contract &contract)
            {
                LOG_INFO("OnContractDefinition");
                m_contracts.push_back(contract);
            }

            // implement of MarketListenerI
            void OnBBO(const pb::dms::BBO &bbo)
            {
                LOG_INFO("OnBBO");
                m_bbos.push_back(bbo);
            }

            // implement of MarketListenerI
            void OnBid(const pb::dms::Bid &bid)
            {
                LOG_INFO("OnBid");
                m_bids.push_back(bid);
            }

            // implement of MarketListenerI
            void OnOffer(const pb::dms::Offer &offer)
            {
                LOG_INFO("OnOffer");
                m_offers.push_back(offer);
            }

            // implement of MarketListenerI
            void OnL2(const pb::dms::L2 &l2)
            {
                LOG_INFO("OnL2");
                m_l2s.push_back(l2);
            }

            // implement of MarketListenerI
            void OnL3()
            {
                // noop
            }

            // implement of MarketListenerI
            void OnTrade(const pb::dms::Trade &trade)
            {
                LOG_INFO("OnTrade");
                m_trades.push_back(trade);
            }

            // implement of MarketListenerI
            void OnContractAuctioning(const std::string &contract)
            {
                LOG_INFO("OnContractAuctioning");
                m_status.push_back({contract, 1});
            }

            // implement of MarketListenerI
            void OnContractNoTrading(const std::string &contract)
            {
                LOG_INFO("OnContractNoTrading");
                m_status.push_back({contract, 2});
            }

            // implement of MarketListenerI
            void OnContractTrading(const std::string &contract)
            {
                LOG_INFO("OnContractTrading");
                m_status.push_back({contract, 3});
            }

            // implement of MarketListenerI
            void OnOrginalMessage(const std::string &message)
            {
                LOG_INFO("OnOrginalMessage");
            }

        public:
            std::vector<pb::dms::Contract> &Contracts() { return m_contracts; }
            std::vector<pb::dms::BBO> &BBOs() { return m_bbos; }
            std::vector<pb::dms::Bid> &Bids() { return m_bids; }
            std::vector<pb::dms::Offer> &Offers() { return m_offers; }
            std::vector<pb::dms::L2> &L2s() { return m_l2s; }
            std::vector<pb::dms::Trade> &Trades() { return m_trades; }
            std::vector<std::pair<std::string, int>> &Status() { return m_status; }

        private:
            std::vector<pb::dms::Contract> m_contracts;
            std::vector<pb::dms::BBO> m_bbos;
            std::vector<pb::dms::Bid> m_bids;
            std::vector<pb::dms::Offer> m_offers;
            std::vector<pb::dms::L2> m_l2s;
            std::vector<pb::dms::Trade> m_trades;
            std::vector<std::pair<std::string, int>> m_status;

    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__
