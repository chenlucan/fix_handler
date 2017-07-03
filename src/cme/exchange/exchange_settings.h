
#ifndef __FH_CME_EXCHANGE_EXCHANGE_SETTINGS_H__
#define __FH_CME_EXCHANGE_EXCHANGE_SETTINGS_H__

#include <string>
#include "core/global.h"
#include "core/assist/settings.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class ExchangeSettings
    {
        public:
            explicit ExchangeSettings(const std::string &app_setting_file);
            virtual ~ExchangeSettings();

        public:
            const std::pair<std::string, std::string> Get_strategy_url() const;
            const std::pair<std::string, std::string> Get_target_id() const;
            const std::tuple<std::string, std::string, std::string> Get_sender_id() const;
            const std::pair<std::string, std::string> Get_password() const;
            const std::tuple<std::string, std::string, std::string> Get_app_info() const;
            const std::string Get_account() const;
            const std::string Get_manual_flag() const;
            const std::string Get_security_type() const;
            const std::string Get_customer_flag() const;
            const std::string Get_cti_code() const;

        private:
            fh::core::assist::Settings m_settings;

        private:
            DISALLOW_COPY_AND_ASSIGN(ExchangeSettings);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif // __FH_CME_EXCHANGE_EXCHANGE_SETTINGS_H__
