
#include "application.h"

namespace rczg
{
    
    Application::Application()
    : m_udp_incrementals(), m_udp_recoveries(), m_udp_definitions(),
      m_tcp_replayer(nullptr), m_sender(nullptr), m_arbitrator(nullptr), m_saver(nullptr), m_processor(nullptr)
    {
        Initial_application();
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
    
    void Application::Initial_application()
    {
        // TODO should initialized by config file (channel id="360")
        rczg::UDPReceiver *incrementalA = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30001, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::UDPReceiver *incrementalB = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30002, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::UDPReceiver *recoveryA = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30003, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::UDPReceiver *recoveryB = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30004, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::UDPReceiver *definitionA = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30005, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::UDPReceiver *definitionB = new rczg::UDPReceiver(
            boost::asio::ip::address::from_string("0.0.0.0"), 
            30006, 
            boost::asio::ip::address::from_string("239.255.0.1"));
        rczg::TCPReceiver *tcp = new rczg::TCPReceiver(
            boost::asio::ip::address::from_string("192.168.1.185"), 
            30007);
            
        m_udp_incrementals.push_back(incrementalA);
        m_udp_incrementals.push_back(incrementalB);
        m_udp_recoveries.push_back(recoveryA);
        m_udp_recoveries.push_back(recoveryB);
        m_udp_definitions.push_back(definitionA);
        m_udp_definitions.push_back(definitionB);
        m_tcp_replayer = tcp;

        m_sender = new rczg::ZmqSender("tcp://*:5557");
        m_arbitrator = new rczg::DatArbitrator(0);
        m_saver = new rczg::DatSaver(*m_sender);
        m_processor = new rczg::DatProcessor(*m_arbitrator, *m_saver, *m_tcp_replayer);
    }
    
    void Application::Start()
    {
        // start udp incrementals
        std::for_each(m_udp_incrementals.begin(), m_udp_incrementals.end(), 
                      std::bind(&Application::Start_udp_feed, this, std::placeholders::_1));
        // start message reader
        Start_read();
    }
    
    void Application::Join()
    {
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
                        
        std::cin.get();
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

