
#ifndef __FH_TMALPHA_TRADE_TRADE_SIMULATER_H__
#define __FH_TMALPHA_TRADE_TRADE_SIMULATER_H__

#include <atomic>
#include <unordered_map>
#include "core/global.h"
#include "core/assist/settings.h"
#include "core/market/marketlisteneri.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/trade_order_manager.h"
#include "tmalpha/trade/trade_market_manager.h"
#include "tmalpha/trade/trade_contract_assist.h"
#include "tmalpha/trade/trade_algorithm.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 用于返回处理结果（response）时的消息文字
    struct Message
    {
        static constexpr const char *CLIENT_ORDER_ID_EXIST = "client order id already exist";
        static constexpr const char *WORKING_ORDER_NOT_FOUND = "working order not found";
        static constexpr const char *ORDER_CANCELLED = "order cancelled";
        static constexpr const char *CONTRACT_NOT_FOUND = "order's contract not found";
        static constexpr const char *ORDER_ALREADY_CANCELLED = "order is already deleted";
        static constexpr const char *ORDER_ALREADY_FILLED = "order is already filled";
        static constexpr const char *ORDER_NOT_FOUND = "order not found";
        static constexpr const char *ORDER_EXPIRED = "order expired";
        static constexpr const char *ORDER_PRICE_TOO_LOW = "order price too low";
        static constexpr const char *ORDER_PRICE_TOO_HIGH = "order price too high";
        static constexpr const char *ORDER_PRICE_INVALID = "order price invalid";
        static constexpr const char *ORDER_TYPE_INVALID = "order type invalid";
        static constexpr const char *ORDER_BS_INVALID = "order must be buy or sell";
        static constexpr const char *LIMIT_ORDER_QUANTITY_INVALID = "limit order must set quantity";
        static constexpr const char *MARKET_ORDER_QUANTITY_INVALID = "market order's quantity must be 0";
    };

    class TradeSimulater : public core::exchange::ExchangeI, public OrderExpiredListener
    {
        public:
            TradeSimulater(
                    fh::core::market::MarketListenerI *market_listener,
                    fh::core::exchange::ExchangeListenerI *exchange_listener);
            virtual ~TradeSimulater();

        public:
            // 加载初期合约定义信息
            void Load_contracts(const std::unordered_map<std::string, std::string> &contracts);
            // 加载订单匹配算法
            void Load_match_algorithm(TradeAlgorithm *ta);

        public:
            // implement of ExchangeI
            bool Start(const std::vector<pb::ems::Order> &init_orders) override;
            // implement of ExchangeI
            void Stop() override;

        public:
            // implement of ExchangeI
            void Initialize(std::vector<::pb::dms::Contract> contracts) override;
            // implement of ExchangeI
            void Add(const pb::ems::Order& order) override;
            // implement of ExchangeI
            void Change(const pb::ems::Order& order) override;
            // implement of ExchangeI
            void Delete(const pb::ems::Order& order) override;
            // implement of ExchangeI
            void Query(const pb::ems::Order& order) override;
            // implement of ExchangeI
            void Query_mass(const char *data, size_t size) override;
            // implement of ExchangeI
            void Delete_mass(const char *data, size_t size) override;

        public:
            // implement of OrderExpiredListener
            void On_order_expired(const pb::ems::Order *order) override;

        private:
            std::string Next_exchange_order_id();
            std::string Next_fill_id();
            std::string Check_order(const pb::ems::Order& order) const;
            void Match_order(pb::ems::Order &order);
            static pb::ems::Order Make_reject_response(const pb::ems::Order& org_order, const std::string &reject_reason);
            static pb::ems::Order Make_cancel_response(const pb::ems::Order& org_order, const std::string &cancel_reason);

        private:
            core::exchange::ExchangeListenerI *m_exchange_listener;
            std::vector<pb::ems::Order> m_init_orders;
            TradeAlgorithm *m_match_algorithm;
            TradeContractAssist m_contract_assist;
            TradeMarketManager m_market_manager;
            TradeOrderManager m_order_manager;
            std::atomic<std::uint32_t> m_exchange_order_id;
            std::atomic<std::uint32_t> m_fill_id;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeSimulater);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_SIMULATER_H__
