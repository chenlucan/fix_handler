
#include "cme/market/market_manager.h"
#include "cme/market/cme_market.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    MarketManager::MarketManager(
            CmeMarket *cme,
            fh::core::market::MarketListenerI *listener,
            const fh::cme::market::setting::Channel &channel,
            const fh::cme::market::setting::MarketSettings &settings)
    : m_cme(cme), m_channel_id(channel.id), m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_tcp_replayer(nullptr), m_saver(nullptr),
      m_processor(nullptr), m_definition_saver(nullptr), m_recovery_saver(nullptr)
    {
        Initial_application(listener, channel, settings);
    }

    MarketManager::~MarketManager()
    {
        auto release_udp = [](const fh::core::udp::UDPReceiver *r){ delete r;};
        std::for_each(m_udp_incrementals.cbegin(), m_udp_incrementals.cend(), release_udp);
        std::for_each(m_udp_recoveries.cbegin(), m_udp_recoveries.cend(), release_udp);
        std::for_each(m_udp_definitions.cbegin(), m_udp_definitions.cend(), release_udp);
        delete m_tcp_replayer;
        delete m_processor;
        delete m_saver;
        delete m_definition_saver;
        delete m_recovery_saver;
    }

    void MarketManager::Initial_application(
            fh::core::market::MarketListenerI *listener,
            const fh::cme::market::setting::Channel &channel,
            const fh::cme::market::setting::MarketSettings &settings)
    {
        std::vector<fh::cme::market::setting::Connection> connections = channel.connections;
        std::pair<std::string, std::string> auth = settings.Get_auth();

        std::for_each(connections.cbegin(), connections.cend(), [this, &channel, &auth](const fh::cme::market::setting::Connection &c){
            if(c.protocol == fh::cme::market::setting::Protocol::TCP)
            {
                // TCP replay
                LOG_INFO("[", m_channel_id, "]TCP:", c.host_ip, ", ", c.port, ", {", auth.first, ", ", auth.second, "}");
                m_tcp_replayer = new fh::cme::market::DatReplayer(c.host_ip, c.port, auth.first, auth.second, channel.id);
            }
            else if(c.type == fh::cme::market::setting::FeedType::I)
            {
                // UDP Incremental
                LOG_INFO("[", m_channel_id, "]UDP Incremental:", c.ip, ", ", c.port);
                m_udp_incrementals.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
            else if(c.type ==  fh::cme::market::setting::FeedType::N)
            {
                // UDP Instrument Definition
                LOG_INFO("[", m_channel_id, "]UDP Definition:", c.ip, ", ", c.port);
                m_udp_definitions.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
            else if(c.type ==  fh::cme::market::setting::FeedType::S)
            {
                // UDP Market Recovery
                LOG_INFO("[", m_channel_id, "]UDP Recovery:", c.ip, ", ", c.port);
                m_udp_recoveries.push_back(new fh::core::udp::UDPReceiver(c.ip, c.port));
            }
        });

        m_saver = new  fh::cme::market::DatSaver(listener);
        m_processor = new  fh::cme::market::DatProcessor(m_saver, m_tcp_replayer, std::bind(&MarketManager::Stop, this));
        m_definition_saver = new  fh::cme::market::RecoverySaver(true);
        m_recovery_saver = new  fh::cme::market::RecoverySaver(false);
    }

    void MarketManager::Start()
    {
        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(),
                      std::bind(&MarketManager::Start_increment_feed, this, std::placeholders::_1));
        // start udp definitions
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(),
                      std::bind(&MarketManager::Start_definition_feed, this, std::placeholders::_1));
        // start message saver
        Start_save();
    }

    void MarketManager::Stop_recoveries()
    {
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("[", m_channel_id, "]recovery udp listener stopped.");
    }

    void MarketManager::Stop_definitions()
    {
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("[", m_channel_id, "]definition udp listener stopped.");
    }

    void MarketManager::Stop_increments()
    {
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("[", m_channel_id, "]increment udp listener stopped.");
    }

    void MarketManager::Stop_saver()
    {
        m_saver->Stop();
        LOG_INFO("[", m_channel_id, "]saver stopped.");
    }

    void MarketManager::Start_increment_feed(fh::core::udp::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::DatProcessor::Process_feed_data, std::ref(*m_processor),
                          std::placeholders::_1, std::placeholders::_2)
            );
        });
        t.detach();
    }

    void MarketManager::Start_definition_feed(fh::core::udp::UDPReceiver *udp)
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

    void MarketManager::Start_recovery_feed(fh::core::udp::UDPReceiver *udp)
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

    void MarketManager::On_definition_end()
    {
        LOG_INFO("[", m_channel_id, "]definition all received.");
        this->Stop_definitions();

        // start udp recoveries
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(),
                      std::bind(&MarketManager::Start_recovery_feed, this, std::placeholders::_1));
    }

    void MarketManager::On_recovery_end()
    {
        LOG_INFO("[", m_channel_id, "]recovery all received.");
        this->Stop_recoveries();

        // 此时将接受到的恢复数据设置到 saver
        m_saver->Set_recovery_data(m_definition_saver->Get_data(), m_recovery_saver->Get_data());
    }

    void MarketManager::Start_save()
    {
        LOG_INFO("[", m_channel_id, "]start data saver.");
        std::thread t(&DatSaver::Start_save, m_saver);
        t.detach();
    }

    void MarketManager::Stop()
    {
        this->Stop_increments();
        this->Stop_saver();

        LOG_INFO("[", m_channel_id, "]stopped.");
        m_cme->Remove_market(m_channel_id);
    }

} // namespace market
} // namespace cme
} // namespace fh
