
#ifndef __FH_CME_MARKET_MANAGER_H__
#define __FH_CME_MARKET_MANAGER_H__

#include "core/global.h"
#include "core/udp/udp_receiver.h"
#include "cme/market/setting/market_settings.h"
#include "cme/market/setting/channel_settings.h"
#include "cme/market/dat_replayer.h"
#include "cme/market/dat_processor.h"
#include "cme/market/dat_saver.h"
#include "cme/market/recovery_saver.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{

    class MarketManager
    {
        public:
            MarketManager(
                    fh::core::market::MarketListenerI *listener,
                    const fh::cme::market::setting::Channel &channel,
                    const fh::cme::market::setting::MarketSettings &settings);
            virtual ~MarketManager();

        public:
            void Start();
            void Stop();

        private:
            void Initial_application(
                    fh::core::market::MarketListenerI *listener,
                    const fh::cme::market::setting::Channel &channel,
                    const fh::cme::market::setting::MarketSettings &settings);
            void Start_increment_feed(fh::core::udp::UDPReceiver *udp);
            void Start_definition_feed(fh::core::udp::UDPReceiver *udp);
            void Start_recovery_feed(fh::core::udp::UDPReceiver *udp);
            void On_definition_end();
            void On_recovery_end();
            void Start_save();
            void Stop_recoveries();
            void Stop_definitions();
            void Stop_increments();
            void Stop_saver();

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
            DISALLOW_COPY_AND_ASSIGN(MarketManager);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MANAGER_H__
