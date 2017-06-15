
#ifndef __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__
#define __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__

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
namespace replay
{
    class ReplayListener
    {
        public:
            ReplayListener() : m_market_listener(nullptr), m_on_l2_changed() {}
            virtual ~ReplayListener() {}

        public:
            // L2 行情数据发生变化时通过这个 callback 通知外部（L2 行情，bid 成交数量，ask成交数量）：交易时需要使用
            void Add_l2_changed_callback(std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> on_l2_changed)
            {
                m_on_l2_changed = on_l2_changed;
            }

            // 行情数据通过这个通知外部
            void Add_market_listener(fh::core::market::MarketListenerI *listener)
            {
                m_market_listener = listener;
            }

        public:
            void OnMarketDisconnect(fh::core::market::MarketI* market)
            {
                if(m_market_listener) m_market_listener->OnMarketDisconnect(market);
            }

            void OnMarketReconnect(fh::core::market::MarketI* market)
            {
                if(m_market_listener) m_market_listener->OnMarketReconnect(market);
            }

            void OnContractDefinition(const pb::dms::Contract &contract)
            {
                if(m_market_listener) m_market_listener->OnContractDefinition(contract);
            }

            void OnBBO(const pb::dms::BBO &bbo)
            {
                if(m_market_listener) m_market_listener->OnBBO(bbo);
            }

            void OnBid(const pb::dms::Bid &bid)
            {
                if(m_market_listener) m_market_listener->OnBid(bid);
            }

            void OnOffer(const pb::dms::Offer &offer)
            {
                if(m_market_listener) m_market_listener->OnOffer(offer);
            }

            void OnL2(const pb::dms::L2 &l2, std::uint32_t bid_turnover, std::uint32_t offer_turnover)
            {
                if(m_market_listener) m_market_listener->OnL2(l2);
                if(m_on_l2_changed) m_on_l2_changed(l2, bid_turnover, offer_turnover);
            }

            void OnL3()
            {
                if(m_market_listener) m_market_listener->OnL3();
            }

            void OnTrade(const pb::dms::Trade &trade)
            {
                if(m_market_listener) m_market_listener->OnTrade(trade);
            }

            void OnContractAuctioning(const std::string &contract)
            {
                if(m_market_listener) m_market_listener->OnContractAuctioning(contract);
            }

            void OnContractNoTrading(const std::string &contract)
            {
                if(m_market_listener) m_market_listener->OnContractNoTrading(contract);
            }

            void OnContractTrading(const std::string &contract)
            {
                if(m_market_listener) m_market_listener->OnContractTrading(contract);
            }

        private:
            fh::core::market::MarketListenerI *m_market_listener;
            std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> m_on_l2_changed;

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayListener);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__
