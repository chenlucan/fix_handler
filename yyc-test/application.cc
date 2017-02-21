
#include "application.h"
#include "app_settings.h"
#include "channel_settings.h"
#include "logger.h"

namespace rczg
{
    
    Application::Application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file)
    : m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_tcp_replayer(nullptr),
	  m_saver(nullptr), m_processor(nullptr), m_definition_saver(nullptr), m_recovery_saver(nullptr)
    {
        Initial_application(channel_id, channel_setting_file, app_setting_file);
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
        delete m_definition_saver;
        delete m_recovery_saver;
    }
    
    void Application::Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file)
    {
        rczg::ChannelSettings channel_settings(channel_setting_file);
        rczg::setting::Channel channel = channel_settings.Get_channel(channel_id);
        std::vector<rczg::setting::Connection> connections = channel.connections;

        rczg::AppSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> auth = app_settings.Get_auth();
        std::pair<std::string, std::string> save_url = app_settings.Get_data_save_url();
        
        std::for_each(connections.cbegin(), connections.cend(), [this, &channel_id, &auth](const rczg::setting::Connection &c){
            if(c.protocol == rczg::setting::Protocol::TCP)
            {
                // TCP replay
                LOG_INFO("TCP:", c.host_ip, ", ", c.port, ", {", auth.first, ", ", auth.second, "}");
                m_tcp_replayer = new rczg::DatReplayer(c.host_ip, c.port, auth.first, auth.second, channel_id);
            }
            else if(c.type == rczg::setting::FeedType::I)
            {
                // UDP Incremental
                LOG_INFO("UDP Incremental:", c.ip, ", ", c.port);
                m_udp_incrementals.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
            else if(c.type == rczg::setting::FeedType::N)
            {
                // UDP Instrument Definition
                LOG_INFO("UDP Definition:", c.ip, ", ", c.port);
                m_udp_definitions.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
            else if(c.type == rczg::setting::FeedType::S)
            {
                // UDP Market Recovery
                LOG_INFO("UDP Recovery:", c.ip, ", ", c.port);
                m_udp_recoveries.push_back(new rczg::UDPReceiver(c.ip, c.port));
            }
        });
        
        m_saver = new rczg::DatSaver(save_url.first, save_url.second);
        m_processor = new rczg::DatProcessor(*m_saver, *m_tcp_replayer);
        m_definition_saver = new rczg::RecoverySaver(true);
        m_recovery_saver = new rczg::RecoverySaver(false);
    }
    
    void Application::Start()
    {
        LOG_INFO("Start to listen ...");
        
        // must tell processer this is weekly pre-opening startup
        m_processor->Set_later_join(false);
        
        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(), 
                      std::bind(&Application::Start_increment_feed, this, std::placeholders::_1));
        // start message saver
        Start_save();
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
        std::for_each(m_udp_recoveries.begin(), m_udp_recoveries.end(), std::mem_fun(&rczg::UDPReceiver::Stop));
        LOG_INFO("recovery udp listener stopped.");
    }
    
    void Application::Stop_definitions()
    {
        std::for_each(m_udp_definitions.begin(), m_udp_definitions.end(), std::mem_fun(&rczg::UDPReceiver::Stop));
        LOG_INFO("definition udp listener stopped.");
    }

    void Application::Start_increment_feed(rczg::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(&rczg::DatProcessor::Process_feed_data, std::ref(*m_processor), 
                          std::placeholders::_1, std::placeholders::_2)
            ); 
        });  
        t.detach();
    }
    
    void Application::Start_definition_feed(rczg::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(&rczg::RecoverySaver::Process_recovery_packet,
                		  std::ref(*m_definition_saver),
						  [this]{this->On_definition_end();},
                          std::placeholders::_1, std::placeholders::_2)
            );
        });
        t.detach();
    }

    void Application::Start_recovery_feed(rczg::UDPReceiver *udp)
    {
        std::thread t([udp, this]{
            udp->Start_receive(
                std::bind(&rczg::RecoverySaver::Process_recovery_packet,
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
    
}

