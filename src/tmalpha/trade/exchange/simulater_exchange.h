
#ifndef __FH_TMALPHA_TRADE_EXCHANGE_SIMULATER_EXCHANGE_H__
#define __FH_TMALPHA_TRADE_EXCHANGE_SIMULATER_EXCHANGE_H__

#include "core/global.h"
#include "core/assist/settings.h"
#include "core/market/marketlisteneri.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/trade_simulater.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
namespace exchange
{
    class SimulaterExchange : public core::exchange::ExchangeI
    {
        public:
            SimulaterExchange(core::exchange::ExchangeListenerI *exchange_listener);
            virtual ~SimulaterExchange();

        public:
            void Set_simulater(fh::tmalpha::trade::TradeSimulater *simulater);

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

        private:
            core::exchange::ExchangeListenerI *m_exchange_listener;
            std::vector<pb::ems::Order> m_init_orders;
            fh::tmalpha::trade::TradeSimulater *m_simulater;

        private:
            DISALLOW_COPY_AND_ASSIGN(SimulaterExchange);
    };
} // namespace exchange
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_EXCHANGE_SIMULATER_EXCHANGE_H__
