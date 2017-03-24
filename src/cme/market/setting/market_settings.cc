
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

    const std::string MarketSettings::Get_persist_receive_url() const
    {
        return m_settings.Get("persist.receive_url");
    }

    const std::string MarketSettings::Get_db_url() const
    {
        return m_settings.Get("persist.db_url");
    }

    const std::pair<std::string, std::string> MarketSettings::Get_save_collection() const
    {
        return {m_settings.Get("persist.db_name"), m_settings.Get("persist.collection_name")};
    }

} // namespace setting
} // namespace market
} // namespace cme
} // namespace fh
