
#ifndef __FH_TMALPHA_REPLAY_MARKET_REPLAY_EXCHANGE_H__
#define __FH_TMALPHA_REPLAY_MARKET_REPLAY_EXCHANGE_H__

#include "core/global.h"
#include "core/assist/settings.h"
#include "core/market/marketlisteneri.h"
#include "core/exchange/exchangei.h"
#include "core/exchange/exchangelisteneri.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/replay/replay_simulater.h"


namespace fh
{
namespace tmalpha
{
namespace replay
{
namespace exchange
{
    class ReplayExchange : public core::exchange::ExchangeI
    {
        public:
            ReplayExchange(core::exchange::ExchangeListenerI *exchange_listener);
            virtual ~ReplayExchange();

        public:
            void Set_simulater(fh::tmalpha::replay::ReplaySimulater *simulater);

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
            fh::tmalpha::replay::ReplaySimulater *m_simulater;

        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayExchange);
    };
} // namespace exchange
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_REPLAY_MARKET_REPLAY_EXCHANGE_H__
