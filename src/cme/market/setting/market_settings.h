
#ifndef __FH_CME_MARKET_SETTING_MARKET_SETTINGS_H__
#define __FH_CME_MARKET_SETTING_MARKET_SETTINGS_H__

#include "core/global.h"
#include "core/assist/settings.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace setting
{
    class MarketSettings
    {
        public:
            explicit MarketSettings(const std::string &app_setting_file);
            virtual ~MarketSettings();

        public:
            const std::pair<std::string, std::string> Get_auth() const;
            const std::pair<std::string, std::string> Get_data_save_url() const;
            const std::string Get_persist_receive_url() const;
            const std::string Get_db_url() const;
            const std::pair<std::string, std::string> Get_save_collection() const;

        private:
            fh::core::assist::Settings m_settings;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketSettings);
    };
} // namespace setting
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_SETTING_MARKET_SETTINGS_H__
