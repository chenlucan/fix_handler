
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "global.h"
#include "udp_receiver.h"
#include "dat_replayer.h"
#include "dat_processor.h"
#include "dat_saver.h"
#include "recovery_saver.h"

namespace rczg
{
    class Application
    {
        public:
            explicit Application(const std::string &channel_id, const std::string &channel_setting_file = "config.xml", const std::string &app_setting_file = "settings.ini");
            virtual ~Application();
            
        public:
            void Start();
            void Join();
            
        private:
            void Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file);
            void Start_increment_feed(rczg::UDPReceiver *udp);
            void Start_definition_feed(rczg::UDPReceiver *udp);
            void Start_recovery_feed(rczg::UDPReceiver *udp);
            void On_definition_end();
            void On_recovery_end();
            void Start_save();
            void Stop_recoveries();
            void Stop_definitions();
            
        private:
            std::vector<rczg::UDPReceiver *> m_udp_incrementals;
            std::vector<rczg::UDPReceiver *> m_udp_recoveries;
            std::vector<rczg::UDPReceiver *> m_udp_definitions;
            rczg::DatReplayer *m_tcp_replayer;
            rczg::DatSaver *m_saver;
            rczg::DatProcessor *m_processor;
            rczg::RecoverySaver *m_definition_saver;
            rczg::RecoverySaver *m_recovery_saver;

        private:
            DISALLOW_COPY_AND_ASSIGN(Application);
    };   
}

#endif // __APPLICATION_H__

    

