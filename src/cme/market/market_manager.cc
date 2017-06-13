
#include "cme/market/market_manager.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{
    MarketManager::MarketManager(
            std::function<void(const std::string &)> channel_stop_callback,
            fh::core::market::MarketListenerI *listener,
            const fh::cme::market::setting::Channel &channel,
            const fh::cme::market::setting::MarketSettings &settings)
    : m_channel_stop_callback(channel_stop_callback), m_channel_id(channel.id),
      m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_incrementals(), m_recoveries(), m_definitions(),
      m_tcp_replayer(nullptr), m_saver(nullptr), m_processor(nullptr), 
      m_data(), 
      m_tid_udp_incrementals(nullptr), m_tid_udp_recoveries(nullptr), m_tid_udp_definitions(nullptr), 
      m_tid_dat_saver(nullptr)
    {
        LOG_DEBUG("===== MarketManager::MarketManager()  =====");
        Initial_application(listener, channel, settings);
    }

    MarketManager::~MarketManager()
    {
        LOG_DEBUG("===== MarketManager::~MarketManager()  =====");

        delete m_tcp_replayer;
        delete m_processor;
        delete m_saver;

        if(m_tid_dat_saver)
        {
            delete m_tid_dat_saver;
            m_tid_dat_saver = nullptr;
        }

        if(m_tid_udp_definitions)
        {
            delete m_tid_udp_definitions;
            m_tid_udp_definitions = nullptr;
        }

        if(m_tid_udp_recoveries)
        {
            delete m_tid_udp_recoveries;
            m_tid_udp_recoveries = nullptr;
        }
                
        if(m_tid_udp_incrementals)
        {
            delete m_tid_udp_incrementals;
            m_tid_udp_incrementals = nullptr;
        }

        if(m_udp_definitions!=nullptr)
        {
            delete m_udp_definitions;
            m_udp_definitions = nullptr;
        }
        
        if(m_udp_recoveries!=nullptr)
        {
            delete m_udp_recoveries;
            m_udp_recoveries = nullptr;
        }
        
        if(m_udp_incrementals!=nullptr)
        {
            delete m_udp_incrementals;
            m_udp_incrementals = nullptr;
        }
    }

    void MarketManager::Initial_application(
            fh::core::market::MarketListenerI *listener,
            const fh::cme::market::setting::Channel &channel,
            const fh::cme::market::setting::MarketSettings &settings)
    {
        try
        {
            std::vector<fh::cme::market::setting::Connection> connections = channel.connections;
            std::pair<std::string, std::string> auth = settings.Get_auth();

            if(connections.empty())
            {
                return;
            }

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
                    m_incrementals.push_back({c.ip, c.port});
                }
                else if(c.type ==  fh::cme::market::setting::FeedType::N)
                {
                    // UDP Instrument Definition
                    LOG_INFO("[", m_channel_id, "]UDP Definition:", c.ip, ", ", c.port);
                    m_definitions.push_back({c.ip, c.port});
                }
                else if(c.type ==  fh::cme::market::setting::FeedType::S)
                {
                    // UDP Market Recovery
                    LOG_INFO("[", m_channel_id, "]UDP Recovery:", c.ip, ", ", c.port);
                    m_recoveries.push_back({c.ip, c.port});
                }
            });

            if(!m_incrementals.empty())
            {
                m_udp_incrementals = new fh::core::udp::UDPReceiver(m_incrementals);
            }

            if(!m_recoveries.empty())
            {
                m_udp_recoveries = new fh::core::udp::UDPReceiver(m_recoveries);
            }

            if(!m_definitions.empty())
            {
                m_udp_definitions = new fh::core::udp::UDPReceiver(m_definitions);
            }

            m_saver = new  fh::cme::market::DatSaver(&m_data, listener);
            m_processor = new  fh::cme::market::DatProcessor(&m_data, m_tcp_replayer, std::bind(&MarketManager::Stop, this));
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }

    void MarketManager::Start()
    {
        try
        {
            if(m_udp_incrementals!=nullptr)
            {
                Start_increment_feed(m_udp_incrementals);
            }
            
            if(m_udp_definitions!=nullptr)
            {
                Start_definition_feed(m_udp_definitions);
            }
            // start message saver
            Start_save();
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }

    void MarketManager::Stop_recoveries()
    {
        LOG_DEBUG("===== MarketManager::Stop_recoveries =====");

        if(m_udp_recoveries!=nullptr)
        {
            m_udp_recoveries->Stop();
        }
        
        if(m_tid_udp_recoveries)
        {
            // wait m_ptid_udp_recoveries worker thread exit
            Exit_work_thread(m_ptid_udp_recoveries, m_tid_udp_recoveries);  
        }
        
        LOG_INFO("[", m_channel_id, "]recovery udp listener stopped.");
    }

    void MarketManager::Stop_definitions()
    {
        LOG_DEBUG("===== MarketManager::Stop_definitions =====");

        if(m_udp_definitions!=nullptr)
        {
            m_udp_definitions->Stop();
        }

        if(m_tid_udp_definitions)
        {
            // wait m_ptid_udp_definitions worker thread exit
            Exit_work_thread(m_ptid_udp_definitions, m_tid_udp_definitions); 
        }
        
        LOG_INFO("[", m_channel_id, "]definition udp listener stopped.");
    }

    void MarketManager::Stop_increments()
    {
        LOG_DEBUG("===== MarketManager::Stop_increments =====");

        if(m_udp_incrementals!=nullptr)
        {
            m_udp_incrementals->Stop();
        }

        if(m_tid_udp_incrementals)
        {
            // wait m_ptid_udp_incrementals worker thread exit
            Exit_work_thread(m_ptid_udp_incrementals, m_tid_udp_incrementals);
        }

        LOG_INFO("[", m_channel_id, "]increment udp listener stopped.");
    }

    void MarketManager::Stop_saver()
    {
        LOG_DEBUG("===== MarketManager::Stop_saver =====");
        if(m_saver!=nullptr)
        {
            m_saver->Stop();
        }

        if(m_tid_dat_saver)
        {             
             // wait m_ptid_dat_saver worker thread exit
            Exit_work_thread(m_ptid_dat_saver, m_tid_dat_saver);
        }       
        
        LOG_INFO("[", m_channel_id, "]saver stopped.");
    }

    void MarketManager::Start_increment_feed(fh::core::udp::UDPReceiver *udp)
    {
        if(m_processor!=nullptr)
        {
            m_tid_udp_incrementals = new std::thread([udp, this]{
                udp->Start_receive(
                    std::bind(& fh::cme::market::DatProcessor::Process_feed_data, std::ref(*m_processor),
                              std::placeholders::_1, std::placeholders::_2)
                );
            });

            if(m_tid_udp_incrementals)
            {
                m_ptid_udp_incrementals = m_tid_udp_incrementals->native_handle();
                LOG_DEBUG("===== m_ptid_udp_incrementals = [", (void *)m_ptid_udp_incrementals, "] ======\n");
                //m_tid_udp_incrementals->detach();
            }
        }    
    }

    void MarketManager::Start_definition_feed(fh::core::udp::UDPReceiver *udp)
    {
        m_tid_udp_definitions = new std::thread([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::RecoverySaver::Process_recovery_packet,
                          std::ref(*(m_data.Get_definition_saver())),
                          [this]{this->On_definition_end();},
                          std::placeholders::_1, std::placeholders::_2)
            );
        });

        if(m_tid_udp_definitions)
        {
            m_ptid_udp_definitions = m_tid_udp_definitions->native_handle();
            LOG_DEBUG("===== m_ptid_udp_definitions = [", (void *)m_ptid_udp_definitions, "] ======\n");
        }
    }

    void MarketManager::Start_recovery_feed(fh::core::udp::UDPReceiver *udp)
    {
        m_tid_udp_recoveries = new std::thread([udp, this]{
            udp->Start_receive(
                std::bind(& fh::cme::market::RecoverySaver::Process_recovery_packet,
                          std::ref(*(m_data.Get_recovery_saver())),
                          [this]{this->On_recovery_end();},
                          std::placeholders::_1, std::placeholders::_2)
            );
        });

        if(m_tid_udp_recoveries)
        {
            m_ptid_udp_recoveries = m_tid_udp_recoveries->native_handle();
            LOG_DEBUG("===== m_ptid_udp_recoveries = [", (void *)m_ptid_udp_recoveries, "] ======\n");
        }
    }

    void MarketManager::On_definition_end()
    {
        LOG_INFO("[", m_channel_id, "]definition all received.");
        this->Stop_definitions();

        if(m_udp_recoveries!=nullptr)
        {
            Start_recovery_feed(m_udp_recoveries);
        }
    }

    void MarketManager::On_recovery_end()
    {
        LOG_INFO("[", m_channel_id, "]recovery all received.");
        this->Stop_recoveries();
    }

    void MarketManager::Start_save()
    {
        LOG_INFO("[", m_channel_id, "]start data saver.");
        if(m_saver!=nullptr)
        {
            //std::thread t(&DatSaver::Start_save, m_saver);
            //t.detach();

            m_tid_dat_saver = new std::thread(&DatSaver::Start_save, m_saver);
            if(m_tid_dat_saver)
            {
                m_ptid_dat_saver = m_tid_dat_saver->native_handle();
                LOG_DEBUG("===== m_ptid_dat_saver = [", (void *)m_ptid_dat_saver, "] ======\n");
            }
        }
    }

    void MarketManager::Stop()
    {
        this->Stop_saver();

        this->Stop_recoveries();
        
        this->Stop_definitions();

        this->Stop_increments();

        LOG_INFO("[", m_channel_id, "]stopped.");
        m_channel_stop_callback(m_channel_id);
    }

    void  MarketManager::Exit_work_thread(pthread_t tid, std::thread *pcreate_thread)
    {
        LOG_DEBUG("===== Exit_work_thread, tid= [", (void *)tid, "] =====");

        int rc = pthread_kill(tid, 0);
        if(rc == ESRCH)
        {
            LOG_DEBUG("===== [1] The specified thread does not exist or is terminated! =====");
        } 
        else if(rc == EINVAL)
        {  
            LOG_DEBUG("===== [2] Call a useless signal! =====");  
        }
        else  
        {
            LOG_DEBUG("===== [3] The thread exist! ====="); 
        }
        
        std::this_thread::sleep_for( std::chrono::seconds(1) );  // wait work thread exit to prevent deadlock[data_saver]

        rc = pthread_kill(tid, SIGUSR1);
        LOG_DEBUG("===== pthread_kill() rc= [", rc, "] =====");
        //rc = pthread_join(tid, NULL);  // problem: free thread object , need to use std::thread::join
        //LOG_DEBUG("===== pthread_join() rc= [", rc, "] ====="); 

        if(pcreate_thread)
        {
            pcreate_thread->join();
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
