
#include <boost/algorithm/string.hpp>
#include "cme/market/market_application.h"
#include "cme/market/setting/market_settings.h"
#include "cme/market/setting/channel_settings.h"
#include "core/assist/logger.h"


namespace fh
{
namespace cme
{
namespace market
{

    MarketApplication::MarketApplication(
            const std::string &channel_id,
            const std::string &channel_setting_file,
            const std::string &app_setting_file)
    : m_book_sender(nullptr), m_market(nullptr)
    {
        Initial_application(channel_id, channel_setting_file, app_setting_file);
    }

    MarketApplication::~MarketApplication()
    {
        delete m_market;
        delete m_book_sender;
    }

    void MarketApplication::Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file)
    {
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> save_url = app_settings.Get_data_save_url();
        m_book_sender = new fh::core::book::BookSender(save_url.first, save_url.second);

        std::vector<fh::cme::market::setting::Channel> target_channels = this->Get_target_channels(channel_id, channel_setting_file);
        m_market = new CmeMarket(m_book_sender, target_channels, app_settings);
    }

    void MarketApplication::Start()
    {
        LOG_INFO("Start to listen ...");
        m_market->Start();
    }

    void MarketApplication::Stop()
    {
        LOG_INFO("Stop listen ...");
        m_market->Stop();
    }

    std::vector<fh::cme::market::setting::Channel> MarketApplication::Get_target_channels(
            const std::string &channel_ids, const std::string &channel_setting_file)
    {
        fh::cme::market::setting::ChannelSettings channel_settings(channel_setting_file);
        if(channel_ids.empty())
        {
            return channel_settings.All_channels();
        }
        else
        {
            std::vector<std::string> channel_id_list;
            boost::split(channel_id_list, channel_ids, boost::is_any_of(","));
            std::vector<fh::cme::market::setting::Channel> channels;
            for (const auto &c : channel_id_list)
            {
                channels.push_back(channel_settings.Get_channel(c));
            }
            return channels;
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
