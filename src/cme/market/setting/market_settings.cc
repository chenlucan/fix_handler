
#include "cme/market/setting/market_settings.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace setting
{

    MarketSettings::MarketSettings(const std::string &app_setting_file) : m_settings(app_setting_file)
    {
        // noop
    }

    MarketSettings::~MarketSettings()
    {
        // noop
    }

    const std::pair<std::string, std::string> MarketSettings::Get_auth() const
    {
        return {m_settings.Get("replayer.username"), m_settings.Get("replayer.password")};
    }

    const std::pair<std::string, std::string> MarketSettings::Get_data_save_url() const
    {
        return {m_settings.Get("zeromq.org_url"), m_settings.Get("zeromq.book_url")};
    }

} // namespace setting
} // namespace market
} // namespace cme
} // namespace fh
