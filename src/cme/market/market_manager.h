
#ifndef __FH_CME_MARKET_MANAGER_H__
#define __FH_CME_MARKET_MANAGER_H__

#include <unordered_map> 
#include <map> 
#include <vector>

#include <thread>    // std::thread, std::this_thread::sleep_for

#include "core/global.h"
#include "core/udp/udp_receiver.h"
#include "cme/market/setting/market_settings.h"
#include "cme/market/setting/channel_settings.h"
#include "cme/market/dat_replayer.h"
#include "cme/market/dat_processor.h"
#include "cme/market/dat_saver.h"
#include "cme/market/recovery_saver.h"
#include "cme/market/cme_data.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{
    typedef std::vector<std::pair<const boost::asio::ip::address, const std::uint16_t>> VecUdpRecvIp;
    typedef std::shared_ptr<std::thread> thread_ptr;

    class MarketManager
    {
        public:
            MarketManager(
                    std::function<void(const std::string &)> channel_stop_callback,
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
            
            void  Exit_work_thread(pthread_t tid, std::thread *pcreate_thread);

        private:
            std::function<void(const std::string &)> m_channel_stop_callback;
            std::string m_channel_id;
            fh::core::udp::UDPReceiver *m_udp_incrementals;
            fh::core::udp::UDPReceiver *m_udp_recoveries;
            fh::core::udp::UDPReceiver *m_udp_definitions;

            VecUdpRecvIp  m_incrementals;
            VecUdpRecvIp  m_recoveries;
            VecUdpRecvIp  m_definitions;

            
            fh::cme::market::DatReplayer *m_tcp_replayer;
            fh::cme::market::DatSaver *m_saver;
            fh::cme::market::DatProcessor *m_processor;

            fh::cme::market::CmeData m_data;

            std::thread *m_tid_udp_incrementals;
            std::thread *m_tid_udp_recoveries;
            std::thread *m_tid_udp_definitions;
            std::thread *m_tid_dat_saver;

            pthread_t m_ptid_udp_incrementals;
            pthread_t m_ptid_udp_recoveries;
            pthread_t m_ptid_udp_definitions;
            pthread_t m_ptid_dat_saver;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketManager);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MANAGER_H__
