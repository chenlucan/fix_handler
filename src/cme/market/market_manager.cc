
#include "cme/market/market_manager.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    MarketManager::MarketManager(
            fh::core::market::MarketListenerI *listener,
            const fh::cme::market::setting::Channel &channel,
            const fh::cme::market::setting::MarketSettings &settings)
    : fh::core::market::MarketI(listener),
      m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
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
                LOG_INFO("TCP:", c.host_ip, ", ", c.port, ", {", auth.first, ", ", auth.second, "}");
                m_tcp_replayer = new fh::cme::market::DatReplayer(c.host_ip, c.port, auth.first, auth.second, channel.id);
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

        m_saver = new  fh::cme::market::DatSaver(listener);
        m_processor = new  fh::cme::market::DatProcessor(m_saver, m_tcp_replayer);
        m_definition_saver = new  fh::cme::market::RecoverySaver(true);
        m_recovery_saver = new  fh::cme::market::RecoverySaver(false);
    }

    // implement of MarketI
    bool MarketManager::Start()
    {
        // must tell processer this is weekly pre-opening startup
        m_processor->Set_later_join(false);

        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(),
                      std::bind(&MarketManager::Start_increment_feed, this, std::placeholders::_1));
        // start message saver
        Start_save();

        return true;
    }

    // implement of MarketI
    bool MarketManager::Join()
    {
        // must tell processer this is weekly pre-opening startup
        m_processor->Set_later_join(true);

        // start udp definitions
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(),
                      std::bind(&MarketManager::Start_definition_feed, this, std::placeholders::_1));

        return true;
    }

    void MarketManager::Stop_recoveries()
    {
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("recovery udp listener stopped.");
    }

    void MarketManager::Stop_definitions()
    {
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), std::mem_fun(&fh::core::udp::UDPReceiver::Stop));
        LOG_INFO("definition udp listener stopped.");
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
        LOG_INFO("definition all received.");
        this->Stop_definitions();

        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(),
                      std::bind(&MarketManager::Start_increment_feed, this, std::placeholders::_1));
        // start udp recoveries
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(),
                      std::bind(&MarketManager::Start_recovery_feed, this, std::placeholders::_1));
    }

    void MarketManager::On_recovery_end()
    {
        LOG_INFO("recovery all received.");
        this->Stop_recoveries();

        // start message saver
        Start_save();
    }

    void MarketManager::Start_save()
    {
        m_saver->Set_definition_data(m_definition_saver->Get_data());
        m_saver->Set_recovery_data(m_recovery_saver->Get_data());

        std::thread t(&DatSaver::Start_save, m_saver);
        t.detach();
    }

    // implement of MarketI
    void MarketManager::Initialize(std::vector<std::string> insts)
    {
        // noop
    }

    // implement of MarketI
    void MarketManager::Stop()
    {
        // noop
    }

    // implement of MarketI
    void MarketManager::Subscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void MarketManager::UnSubscribe(std::vector<std::string> instruments)
    {
        // noop
    }

    // implement of MarketI
    void MarketManager::ReqDefinitions(std::vector<std::string> instruments)
    {
        // noop
    }

} // namespace market
} // namespace cme
} // namespace fh
