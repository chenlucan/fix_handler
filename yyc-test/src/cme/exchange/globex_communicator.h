
#ifndef __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__
#define __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__

#include <string>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include "core/global.h"
#include "cme/exchange/order_manager.h"
#include "cme/exchange/exchange_settings.h"
#include "cme/exchange/globex_logger.h"
#include "pb/ems/ems.pb.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class GlobexCommunicator
    {
        public:
            GlobexCommunicator(
                    const std::string &config_file,
                    const fh::cme::exchange::ExchangeSettings &app_settings,
                    bool is_week_begin);
            virtual ~GlobexCommunicator();

        public:
            void Start(std::function<void(char *data, size_t size)> processor);
            void Stop();
            bool Order_request(const char *data, size_t size);
            void Order_response(const fh::cme::exchange::OrderReport& report);

        private:
            fh::cme::exchange::Order Create_order(const char *data, size_t size);
            fh::cme::exchange::MassOrder Create_mass_order(const char *data, size_t size);
            std::string Create_order_result(const fh::cme::exchange::OrderReport& report);
            char Convert_tif(pb::ems::TimeInForce tif) const;
            char Convert_order_type(pb::ems::OrderType type) const;
            char Convert_buy_sell(pb::ems::BuySell bs) const;
            pb::ems::BuySell Convert_buy_sell(char bs) const;
            pb::ems::OrderStatus Convert_order_status(char status) const;

        private:
            OrderManager m_order_manager;
            FIX::SessionSettings m_settings;
            FIX::FileStoreFactory m_store;
            GlobexLogFactory m_logger;
            FIX::SocketInitiator m_initiator;
            std::function<void(char *data, size_t size)> m_processor;

        private:
            DISALLOW_COPY_AND_ASSIGN(GlobexCommunicator);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_GLOBEX_COMMUNICATOR_H__
