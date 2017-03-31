
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
    : m_book_sender(nullptr), m_market_manager(nullptr)
    {
        Initial_application(channel_id, channel_setting_file, app_setting_file);
    }

    MarketApplication::~MarketApplication()
    {
        delete m_market_manager;
        delete m_book_sender;
    }

    void MarketApplication::Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file)
    {
        fh::cme::market::setting::ChannelSettings channel_settings(channel_setting_file);
        fh::cme::market::setting::Channel channel = channel_settings.Get_channel(channel_id);
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> save_url = app_settings.Get_data_save_url();

        m_book_sender = new BookSender(save_url.first, save_url.second);
        m_market_manager = new MarketManager(m_book_sender, channel, app_settings);
    }

    void MarketApplication::Start()
    {
        LOG_INFO("Start to listen ...");
        m_market_manager->Start();
    }


    void MarketApplication::Join()
    {
        LOG_INFO("Join to listen ...");
        m_market_manager->Join();
    }

    void MarketApplication::Stop()
    {
        LOG_INFO("Stop listen ...");
        m_market_manager->Stop();
    }

} // namespace market
} // namespace cme
} // namespace fh
