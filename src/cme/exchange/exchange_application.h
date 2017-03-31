
#ifndef __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__
#define __FH_CME_EXCHANGE_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class StrategyCommunicator;
    class GlobexCommunicator;

    class ExchangeApplication
    {
        public:
            explicit ExchangeApplication(
                const std::string &fix_setting_file = "exchange_client.cfg",
                const std::string &app_setting_file = "exchange_settings.ini");
            virtual ~ExchangeApplication();

        public:
            void Start();
            void Stop();

        private:
            void Initial_application(const std::string &fix_setting_file, const std::string &app_setting_file);
            void On_from_strategy(char *data, size_t size);

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
