
#include "application.h"
#include "settings.h"
#include "logger.h"

namespace rczg
{
    
    Application::Application(const std::string &channel_id, const std::string &setting_file)
    : m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_tcp_replayer(nullptr), m_sender(nullptr), m_arbitrator(nullptr), m_saver(nullptr), m_processor(nullptr)
    {
        Initial_application(channel_id, setting_file);
    }
    
    Application::~Application()
    {
        auto release_udp = [](const rczg::UDPReceiver *r){ delete r;};
        std::for_each(m_udp_incrementals.cbegin(), m_udp_incrementals.cend(), release_udp); 
        std::for_each(m_udp_recoveries.cbegin(), m_udp_recoveries.cend(), release_udp); 
        std::for_each(m_udp_definitions.cbegin(), m_udp_definitions.cend(), release_udp); 
        delete m_tcp_replayer;
        delete m_processor;
        delete m_saver;
        delete m_arbitrator;
        delete m_sender;
    }
    
    void Application::Initial_application(const std::string &channel_id, const std::string &setting_file)
    {
        rczg::Settings settings(setting_file);
        rczg::setting::Channel channel = settings.Get_channel(channel_id);
        std::vector<rczg::setting::Connection> connections = channel.connections;
        
        std::for_each(connections.cbegin(), connections.cend(), [this](const rczg::setting::Connection &c){
            if(c.protocol == rczg::setting::Protocol::TCP)
            {
                // TCP replay
                rczg::Logger::Info("TCP:", c.host_ip, ", ", c.port);
                m_tcp_replayer = new rczg::TCPReceiver(c.host_ip, c.port);
            }
            else if(c.type == rczg::setting::FeedType::I)
            {
                // UDP Incremental
                rczg::Logger::Info("UDP Incremental:", c.ip, ", ", c.port);
                m_udp_incrementals.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
            else if(c.type == rczg::setting::FeedType::N)
            {
                // UDP Instrument Definition
                rczg::Logger::Info("UDP Definition:", c.ip, ", ", c.port);
                m_udp_definitions.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
            else if(c.type == rczg::setting::FeedType::S)
            {
                // UDP Market Recovery
                rczg::Logger::Info("UDP Recovery:", c.ip, ", ", c.port);
                m_udp_recoveries.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
        });
        
        m_sender = new rczg::ZmqSender("tcp://*:5557");
        m_arbitrator = new rczg::DatArbitrator();
        m_saver = new rczg::DatSaver(*m_sender);
        m_processor = new rczg::DatProcessor(*m_arbitrator, *m_saver, *m_tcp_replayer);
    }
    
    void Application::Start()
    {
        rczg::Logger::Info("Start to listen ...");
        
        // must tell them this is weekly pre-opening startup
        m_arbitrator->Set_later_join(false);
        m_saver->Set_later_join(false);
        
        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(), 
                      std::bind(&Application::Start_udp_feed, this, std::placeholders::_1));
        // start message reader
        Start_read();
    }
    
    void Application::Join()
    {
        rczg::Logger::Info("Join to listen ...");
        
        // must tell them this is later joiner startup
        m_arbitrator->Set_later_join(true);
        m_saver->Set_later_join(true);
        
        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(), 
                      std::bind(&Application::Start_udp_feed, this, std::placeholders::_1));
        // start udp recoveries
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), 
                      std::bind(&Application::Start_udp_feed, this, std::placeholders::_1));
        // start udp definitions
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), 
                      std::bind(&Application::Start_udp_feed, this, std::placeholders::_1));
        // start message reader
        Start_read();
    }
    
    void Application::Stop_recoveries()
    {
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), std::mem_fun(&rczg::UDPReceiver::Stop));
    }
    
    void Application::Stop_definitions()
    {
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), std::mem_fun(&rczg::UDPReceiver::Stop)); 
    }

    void Application::Start_udp_feed(rczg::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(&rczg::DatProcessor::Process_feed_data, std::ref(*m_processor), 
                          std::placeholders::_1, std::placeholders::_2)
            ); 
        });  
        t.detach();
    }
    
    void Application::Start_read()
    {
        std::thread t(&DatSaver::Start_serialize, m_saver);              
        t.detach();
    }
    
}

