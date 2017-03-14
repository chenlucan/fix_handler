
#ifndef __FH_CME_MARKET_APPLICATION_H__
#define __FH_CME_MARKET_APPLICATION_H__

#include "core/global.h"
#include "core/udp/udp_receiver.h"
#include "cme/market/dat_replayer.h"
#include "cme/market/dat_processor.h"
#include "cme/market/dat_saver.h"
#include "cme/market/recovery_saver.h"

namespace fh
{
namespace cme
{
namespace market
{

    class Application
    {
        public:
            explicit Application(
                    const std::string &channel_id,
                    const std::string &channel_setting_file = "market_config.xml",
                    const std::string &app_setting_file = "market_settings.ini");
            virtual ~Application();

        public:
            void Start();
            void Join();

        private:
            void Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file);
            void Start_increment_feed(fh::core::udp::UDPReceiver *udp);
            void Start_definition_feed(fh::core::udp::UDPReceiver *udp);
            void Start_recovery_feed(fh::core::udp::UDPReceiver *udp);
            void On_definition_end();
            void On_recovery_end();
            void Start_save();
            void Stop_recoveries();
            void Stop_definitions();

        private:
            std::vector<fh::core::udp::UDPReceiver *> m_udp_incrementals;
            std::vector<fh::core::udp::UDPReceiver *> m_udp_recoveries;
            std::vector<fh::core::udp::UDPReceiver *> m_udp_definitions;
            fh::cme::market::DatReplayer *m_tcp_replayer;
            fh::cme::market::DatSaver *m_saver;
            fh::cme::market::DatProcessor *m_processor;
            fh::cme::market::RecoverySaver *m_definition_saver;
            fh::cme::market::RecoverySaver *m_recovery_saver;

        private:
            DISALLOW_COPY_AND_ASSIGN(Application);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_APPLICATION_H__