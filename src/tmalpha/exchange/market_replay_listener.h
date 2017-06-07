
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
            : m_book_sender(org_url, book_url), m_on_l2_changed(), m_last_turnover({0, 0}) {}
            virtual ~MarketReplayListener() {}

        public:
            // L2 行情数据发生变化时通过这个 callback 通知外部（L2 行情，bid 成交数量，ask成交数量）
            void Add_l2_changed_callback(std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> on_l2_changed)
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
                if(m_on_l2_changed) m_on_l2_changed(l2, m_last_turnover.first, m_last_turnover.second);
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
                // 数据回放时，不需要处理原始行情数据
                // 所以各个交易所的回放模块使用本接口来传递行情发生变化时计算出的买/卖成交量
                // 内部保存这些数量后，在接下来的 onL2 时通知外部
                // 参数的字符串格式为："bid_volumn(bid 成交量),ask_volumn(ask 成交量)"
                auto pos = message.find(",");
                if(pos == std::string::npos) throw std::invalid_argument("orginal message invalid: " + message);
                m_last_turnover = {std::stoi(message.substr(0, pos)), std::stoi(message.substr(pos + 1))};
            }

        private:
            fh::core::book::BookSender m_book_sender;
            std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> m_on_l2_changed;
            std::pair<std::uint32_t, std::uint32_t> m_last_turnover;
    };
}   // namespace exchange
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_EXCHANGE_MARKET_REPLAY_LISTENER_H__
