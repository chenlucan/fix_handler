
#ifndef __FH_TMALPHA_TRADE_TRADE_MARKET_MANAGER_H__
#define __FH_TMALPHA_TRADE_TRADE_MARKET_MANAGER_H__

#include <unordered_map>
#include "pb/ems/ems.pb.h"
#include "core/global.h"
#include "core/market/marketlisteneri.h"
#include "tmalpha/trade/trade_market_state.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 管理所有合约的行情数据，并且在行情发生变化时需要向外发送
    class TradeMarketManager
    {
        public:
            explicit TradeMarketManager(fh::core::market::MarketListenerI *market_listener);
            virtual ~TradeMarketManager();

        public:
            void Add_contract(const std::string &contract_name, std::uint32_t depth);
            void Send_contract(const pb::dms::Contract &contract);
            void Send_trade(const std::string &contract_name, OrderPrice trade_price, OrderSize trade_quantity);

        public:
            void Change_market_on_order_created(const pb::ems::Order *order);
            void Change_market_on_order_deleted(const pb::ems::Order *deleted_order);
            void Change_market_on_order_filled(const pb::ems::Order *order, OrderSize fill_size);

        private:
            void Send_l2(const TradeMarketState *market);
            void Send_bbo(const TradeMarketState *market);

        private:
            fh::core::market::MarketListenerI *m_market_listener;
            std::unordered_map<std::string, TradeMarketState*> m_market_states;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeMarketManager);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_MARKET_MANAGER_H__
