
#ifndef __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__
#define __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"
#include "core/exchange/exchangei.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class StrategyCommunicator;
    class GlobexCommunicator;

    class ExchangeApplication : public core::exchange::ExchangeI
    {
        public:
            explicit ExchangeApplication(
                core::exchange::ExchangeListenerI* listener,
                bool is_week_begin,
                const std::string &fix_setting_file = "exchange_client.cfg",
                const std::string &app_setting_file = "exchange_settings.ini");
            virtual ~ExchangeApplication();

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

        private:
            void Initial_application(const std::string &fix_setting_file, const std::string &app_setting_file, bool is_week_begin);
            void On_from_strategy(char *data, size_t size);
            void On_from_globex(char *data, size_t size);

        private:
            StrategyCommunicator *m_strategy;
            GlobexCommunicator *m_globex;

        private:
            DISALLOW_COPY_AND_ASSIGN(ExchangeApplication);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__
