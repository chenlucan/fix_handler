
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "global.h"
#include "udp_receiver.h"
#include "tcp_receiver.h"
#include "dat_processor.h"
#include "dat_arbitrator.h"
#include "dat_saver.h"

namespace rczg
{
    class Application
    {
        public:
            explicit Application(const std::string &channel_id, const std::string &setting_file = "config.xml");
            virtual ~Application();
            
        public:
            void Start();
            void Join();
            
        private:
            void Initial_application(const std::string &channel_id, const std::string &setting_file);
            void Start_udp_feed(rczg::UDPReceiver *udp);
            void Start_read();
            void Stop_recoveries();
            void Stop_definitions();
            
        private:
            std::vector<rczg::UDPReceiver *> m_udp_incrementals;
            std::vector<rczg::UDPReceiver *> m_udp_recoveries;
            std::vector<rczg::UDPReceiver *> m_udp_definitions;
            rczg::TCPReceiver *m_tcp_replayer;
            rczg::ZmqSender *m_sender;
            rczg::DatArbitrator *m_arbitrator;
            rczg::DatSaver *m_saver;
            rczg::DatProcessor *m_processor;

        private:
            DISALLOW_COPY_AND_ASSIGN(Application);
    };   
}

#endif // __APPLICATION_H__

    

