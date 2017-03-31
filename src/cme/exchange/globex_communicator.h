
#ifndef __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__
#define __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__

#include <string>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include "core/global.h"
#include "core/exchange/exchangei.h"
#include "cme/exchange/order_manager.h"
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/globex_logger.h"
#include "cme/exchange/strategy_communicator.h"
#include "pb/ems/ems.pb.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class GlobexCommunicator : public core::exchange::ExchangeI
    {
        public:
            GlobexCommunicator(
                    core::exchange::ExchangeListenerI *strategy,
                    const std::string &config_file,
                    const fh::cme::exchange::ExchangeSettings &app_settings);
            virtual ~GlobexCommunicator();

        public:
            // implement of ExchangeI
            virtual bool Start(std::vector<::pb::ems::Order>);
            // implement of ExchangeI
            virtual void Stop();

        public:
            // implement of ExchangeI
            virtual void Initialize(std::vector<::pb::dms::Contract> contracts);
            // implement of ExchangeI
            virtual void Add(const ::pb::ems::Order& order);
            // implement of ExchangeI
            virtual void Change(const ::pb::ems::Order& order);
            // implement of ExchangeI
            virtual void Delete(const ::pb::ems::Order& order);

        public:
            bool Order_request(const char *data, size_t size);
            void Order_response(const fh::cme::exchange::OrderReport& report);

        private:
            static fh::cme::exchange::Order Create_order(const char *data, size_t size);
            static fh::cme::exchange::MassOrder Create_mass_order(const char *data, size_t size);

        private:
            static char Convert_tif(pb::ems::TimeInForce tif);
            static pb::ems::TimeInForce Convert_tif(char tif);
            static char Convert_order_type(pb::ems::OrderType type);
            static pb::ems::OrderType Convert_order_type(char type);
            static char Convert_buy_sell(pb::ems::BuySell bs);
            static pb::ems::BuySell Convert_buy_sell(char bs);
            static pb::ems::OrderStatus Convert_order_status(char status);

        private:
            core::exchange::ExchangeListenerI *m_strategy;
            OrderManager m_order_manager;
            FIX::SessionSettings m_settings;
            FIX::FileStoreFactory m_store;
            GlobexLogFactory m_logger;
            FIX::SocketInitiator m_initiator;

        private:
            DISALLOW_COPY_AND_ASSIGN(GlobexCommunicator);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__
