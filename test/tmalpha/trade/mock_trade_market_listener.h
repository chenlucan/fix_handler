
#ifndef __FH_TMALPHA_TRADE_MOCK_TRADE_MARKET_LISTENER_H__
#define __FH_TMALPHA_TRADE_MOCK_TRADE_MARKET_LISTENER_H__

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
namespace trade
{
    class MockTradeMarketListener : public fh::core::market::MarketListenerI
    {
        public:
            MockTradeMarketListener() : m_contracts(), m_bbos(), m_bids(), m_offers(), m_l2s(), m_trades() {}
            virtual ~MockTradeMarketListener() {}

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
                LOG_INFO("OnContractDefinition: ", fh::core::assist::utility::Format_pb_message(contract));
                m_contracts.push_back(contract);
            }

            // implement of MarketListenerI
            void OnBBO(const pb::dms::BBO &bbo)
            {
                LOG_INFO("OnBBO: ", fh::core::assist::utility::Format_pb_message(bbo));
                m_bbos.push_back(bbo);
            }

            // implement of MarketListenerI
            void OnBid(const pb::dms::Bid &bid)
            {
                LOG_INFO("OnBid: ", fh::core::assist::utility::Format_pb_message(bid));
                m_bids.push_back(bid);
            }

            // implement of MarketListenerI
            void OnOffer(const pb::dms::Offer &offer)
            {
                LOG_INFO("OnOffer: ", fh::core::assist::utility::Format_pb_message(offer));
                m_offers.push_back(offer);
            }

            // implement of MarketListenerI
            void OnL2(const pb::dms::L2 &l2)
            {
                LOG_INFO("OnL2: ", fh::core::assist::utility::Format_pb_message(l2));
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
                LOG_INFO("OnTrade: ", fh::core::assist::utility::Format_pb_message(trade));
                m_trades.push_back(trade);
            }

            // implement of MarketListenerI
            void OnContractAuctioning(const std::string &contract)
            {
                // noop
            }

            // implement of MarketListenerI
            void OnContractNoTrading(const std::string &contract)
            {
                // noop
            }

            // implement of MarketListenerI
            void OnContractTrading(const std::string &contract)
            {
                // noop
            }

            // implement of MarketListenerI
            void OnOrginalMessage(const std::string &message)
            {
                // noop
            }

        public:
            std::vector<pb::dms::Contract> &Contracts() { return m_contracts; }
            std::vector<pb::dms::BBO> &BBOs() { return m_bbos; }
            std::vector<pb::dms::Bid> &Bids() { return m_bids; }
            std::vector<pb::dms::Offer> &Offers() { return m_offers; }
            std::vector<pb::dms::L2> &L2s() { return m_l2s; }
            std::vector<pb::dms::Trade> &Trades() { return m_trades; }

        private:
            std::vector<pb::dms::Contract> m_contracts;
            std::vector<pb::dms::BBO> m_bbos;
            std::vector<pb::dms::Bid> m_bids;
            std::vector<pb::dms::Offer> m_offers;
            std::vector<pb::dms::L2> m_l2s;
            std::vector<pb::dms::Trade> m_trades;

    };
}   // namespace trade
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_TRADE_MOCK_TRADE_MARKET_LISTENER_H__
