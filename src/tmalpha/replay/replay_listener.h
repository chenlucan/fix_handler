
#ifndef __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__
#define __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__

#include <cstdint>
#include <vector>
#include <algorithm>
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
            ReplayListener() : m_market_listener(nullptr), m_on_l2_changed(), m_last_total_turnover(), m_current_turnover() {}
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

            void OnL2(const pb::dms::L2 &l2, int volume_multiple)
            {
                if(m_market_listener) m_market_listener->OnL2(l2);
                if(m_on_l2_changed)
                {
                    std::pair<std::uint64_t, std::uint64_t> volumn = this->Calculate_current_trade_volumn(l2, volume_multiple);
                    m_on_l2_changed(l2, volumn.first, volumn.second);
                }
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

            void OnTurnover(const pb::dms::Turnover &turnover)
            {
                // 每次来新的 turnover 时，都要计算下和上次 turnover 的差值，就是本次行情间隔内的成交量和成交金额
                m_current_turnover = {
                        (std::uint64_t)std::max(0, (int)turnover.total_volume()  - (int)m_last_total_turnover.first),
                        std::max(0.0, turnover.turnover() - m_last_total_turnover.second)
                };
                // 同时将这次的总成交量，总成交金额保存下，供下次行情来的时候计算用
                m_last_total_turnover = {turnover.total_volume() , turnover.turnover()};

                LOG_INFO("current turnover increase: volumn=", turnover.total_volume() , ", turnover=", turnover.turnover());
            }

        private:
            // 根据本次行情间隔内的总成交量和总成交金额，结合最优买卖价格，计算出买卖当前各自的成交量
            // 计算公式：
            //       当前买成交量(x) + 当前卖成交量(y) =  当前时刻成交量(volumn)
            //       当前买成交量(x) * 当前买最优价 * 合约数量乘数 + 当前卖成交量(y) * 当前卖最优价 * 合约数量乘数 = 当前时刻成交金额(turnover)
            std::pair<std::uint32_t, std::uint32_t> Calculate_current_trade_volumn(const pb::dms::L2 &l2, int volume_multiple) const
            {
                std::uint64_t volumn = m_current_turnover.first;    // 本次行情间隔内的总成交量
                double turnover = m_current_turnover.second;     // 本次行情间隔内的总成交金额
                double bid_best_price = l2.bid_size() == 0 ? 0 : l2.bid(0).price();     // 最优买价
                double ask_best_price = l2.offer_size() == 0 ? 0 : l2.offer(0).price();     // 最优卖价

                // 以下任意一条满足的场合无法计算，直接返回 {0, 0}：
                //      合约数量乘数无效（数据问题）
                //      本次行情间隔内的总成交量为 0
                //      本次行情间隔内的总成交金额为 0
                //      最优买价和最优卖价都不存在
                //      最优买价等于最优卖价（不可能出现）
                if(volume_multiple <= 0 || volumn == 0 || turnover <= 0 || (bid_best_price <=0 && ask_best_price <= 0) || bid_best_price == ask_best_price) return {0, 0};

                // 根据计算公式求出 x，y：
                //      x = ( turnover / volume_multiple - volumn * ask_best_price ) / ( bid_best_price - ask_best_price )
                //      y = volumn - x
                int bid_v = (int)(( turnover / volume_multiple - (int)volumn * ask_best_price ) / ( bid_best_price - ask_best_price ));
                int ask_v = volumn - bid_v;

                LOG_INFO("calculate bid volumn(x) and ask volumn(y): ");
                LOG_INFO("        x + y =  ", volumn);
                LOG_INFO("        x * ", bid_best_price, " *  ", volume_multiple , " + y * ", ask_best_price, " *  ", volume_multiple , " = ", turnover);
                LOG_INFO("==>  x = ", bid_v, ", y = ", ask_v);

                if(bid_v <= 0) return {0, volumn};
                if(ask_v <= 0) return {volumn, 0};
                return {bid_v, ask_v};
            }

        private:
            fh::core::market::MarketListenerI *m_market_listener;
            std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> m_on_l2_changed;
            std::pair<std::uint64_t, double> m_last_total_turnover;       // 上次行情数据的总成交量，总成交金额
            std::pair<std::uint64_t, double> m_current_turnover;       // 当前时刻成交量，成交金额（本次的总成交 - 上次的总成交）

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayListener);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_REPLAY_LISTENER_H__
