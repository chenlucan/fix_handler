
#include "cme/market/setting/market_settings.h"
#include "cme/market/application.h"
#include "cme/market/setting/channel_settings.h"
#include "core/assist/logger.h"


namespace fh
{
namespace cme
{
namespace market
{

    Application::Application(
            core::market::MarketListenerI *listener,
            const std::string &channel_id,
            const std::string &channel_setting_file, const std::string &app_setting_file)
    : fh::core::market::MarketI(listener),
      m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_tcp_replayer(nullptr), m_book_sender(nullptr),m_saver(nullptr),
      m_processor(nullptr), m_definition_saver(nullptr), m_recovery_saver(nullptr)
    {
        Initial_application(channel_id, channel_setting_file, app_setting_file);
    }

    Application::~Application()
    {
        auto release_udp = [](const fh::core::udp::UDPReceiver *r){ delete r;};
        std::for_each(m_udp_incrementals.cbegin(), m_udp_incrementals.cend(), release_udp);
        std::for_each(m_udp_recoveries.cbegin(), m_udp_recoveries.cend(), release_udp);
        std::for_each(m_udp_definitions.cbegin(), m_udp_definitions.cend(), release_udp);
        delete m_tcp_replayer;
        delete m_processor;
        delete m_book_sender;
        delete m_saver;
        delete m_definition_saver;
        delete m_recovery_saver;
    }

    void Application::Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file)
    {
        fh::cme::market::setting::ChannelSettings channel_settings(channel_setting_file);
        fh::cme::market::setting::Channel channel = channel_settings.Get_channel(channel_id);
        std::vector<fh::cme::market::setting::Connection> connections = channel.connections;

        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> auth = app_settings.Get_auth();
        std::pair<std::string, std::string> save_url = app_settings.Get_data_save_url();

        std::for_each(connections.cbegin(), connections.cend(), [this, &channel_id, &auth](const fh::cme::market::setting::Connection &c){
            if(c.protocol == fh::cme::market::setting::Protocol::TCP)
            {
                // TCP replay
                LOG_INFO("TCP:", c.host_ip, ", ", c.port, ", {", auth.first, ", ", auth.second, "}");
                m_tcp_replayer = new fh::cme::market::DatReplayer(c.host_ip, c.port, auth.first, auth.second, channel_id);
            }
            else if(c.type == fh::cme::market::setting::FeedType::I)
            {
                // UDP Incremental
                LOG_INFO("UDP Incremental:", c.ip, ", ", c.port);
                m_udp_incrementals.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
            else if(c.type ==  fh::cme::market::setting::FeedType::N)
            {
                // UDP Instrument Definition
                LOG_INFO("UDP Definition:", c.ip, ", ", c.port);
                m_udp_definitions.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
            else if(c.type ==  fh::cme::market::setting::FeedType::S)
            {
                // UDP Market Recovery
                LOG_INFO("UDP Recovery:", c.ip, ", ", c.port);
                m_udp_recoveries.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
        });

        m_book_sender = new fh::cme::market::BookSender(save_url.second);
        m_saver = new  fh::cme::market::DatSaver(save_url.first, m_book_sender);
        m_processor = new  fh::cme::market::DatProcessor(*m_saver, *m_tcp_replayer);
        m_definition_saver = new  fh::cme::market::RecoverySaver(true);
        m_recovery_saver = new  fh::cme::market::RecoverySaver(false);
    }

    // implement of MarketI
    bool Application::Start()
    {
        LOG_INFO("Start to listen ...");

        // must tell processer this is weekly pre-opening startup
        m_processor->Set_later_join(false);

        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(),
                      std::bind(&Application::Start_increment_feed, this, std::placeholders::_1));
        // start message saver
        Start_save();

        return true;
    }

    void Application::Join()
    {
        LOG_INFO("Join to listen ...");

        // must tell processer this is weekly pre-opening startup
        m_processor->Set_later_join(true);

        // start udp definitions
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(),
                      std::bind(&Application::Start_definition_feed, this, std::placeholders::_1));
    }

    void Application::Stop_recoveries()
    {
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("recovery udp listener stopped.");
    }

    void Application::Stop_definitions()
    {
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("definition udp listener stopped.");
    }

    void Application::Start_increment_feed(fh::core::udp::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::DatProcessor::Process_feed_data, std::ref(*m_processor),
                          std::placeholders::_1, std::placeholders::_2)
            );
        });
        t.detach();
    }

    void Application::Start_definition_feed(fh::core::udp::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::RecoverySaver::Process_recovery_packet,
                          std::ref(*m_definition_saver),
                          [this]{this->On_definition_end();},
                          std::placeholders::_1, std::placeholders::_2)
            );
        });
        t.detach();
    }

    void Application::Start_recovery_feed(fh::core::udp::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::RecoverySaver::Process_recovery_packet,
                          std::ref(*m_recovery_saver),
                          [this]{this->On_recovery_end();},
                          std::placeholders::_1, std::placeholders::_2)
            );
        });
        t.detach();
    }

    void Application::On_definition_end()
    {
        LOG_INFO("definition all received.");
        this->Stop_definitions();

        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(),
                      std::bind(&Application::Start_increment_feed, this, std::placeholders::_1));
        // start udp recoveries
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(),
                      std::bind(&Application::Start_recovery_feed, this, std::placeholders::_1));
    }

    void Application::On_recovery_end()
    {
        LOG_INFO("recovery all received.");
        this->Stop_recoveries();

        // start message saver
        Start_save();
    }

    void Application::Start_save()
    {
        m_saver->Set_definition_data(m_definition_saver->Get_data());
        m_saver->Set_recovery_data(m_recovery_saver->Get_data());

        std::thread t(&DatSaver::Start_save, m_saver);
        t.detach();
    }

    // implement of MarketI
    void Application::Initialize(std::vector<std::string> insts)
    {

    }

    // implement of MarketI
    void Application::Stop()
    {

    }

    // implement of MarketI
    void Application::Subscribe(std::vector<std::string> instruments)
    {

    }

    // implement of MarketI
    void Application::UnSubscribe(std::vector<std::string> instruments)
    {

    }

    // implement of MarketI
    void Application::ReqDefinitions(std::vector<std::string> instruments)
    {

    }

} // namespace market
} // namespace cme
} // namespace fh
