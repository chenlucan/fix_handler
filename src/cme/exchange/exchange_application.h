
#ifndef __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__
#define __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"
#include "core/strategy/strategy_communicator.h"
#include "cme/exchange/globex_communicator.h"


namespace fh
{
namespace cme
{
namespace exchange
{
    class ExchangeApplication
    {
        public:
            explicit ExchangeApplication(
                const std::string &fix_setting_file = CME_EXCHANGE_CLIENT_CFG,
                const std::string &app_setting_file = CME_EXCHANGE_SETTINGS_INI);
            virtual ~ExchangeApplication();

        public:
            bool Start();
            void Stop();

        private:
            void Initial_application(const std::string &fix_setting_file, const std::string &app_setting_file);
            std::vector<::pb::ems::Order> Get_init_orders();

        private:
            fh::core::strategy::StrategyCommunicator *m_strategy;
            GlobexCommunicator *m_globex;

        private:
            DISALLOW_COPY_AND_ASSIGN(ExchangeApplication);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__
