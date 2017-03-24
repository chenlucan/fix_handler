
#ifndef __FH_CME_MARKET_PERSIST_MDP_SAVER_H__
#define __FH_CME_MARKET_PERSIST_MDP_SAVER_H__

#include "core/global.h"
#include "cme/market/persist/mdp_receiver.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace persist
{
    class MdpSaver
    {
        public:
            explicit MdpSaver(const std::string &app_setting_file = "market_settings.ini");
            virtual ~MdpSaver();

        public:
            void Start_save();

        private:
            void Init(const std::string &app_setting_file);

        private:
            fh::cme::market::persist::MdpReceiver *m_receiver;

        private:
            DISALLOW_COPY_AND_ASSIGN(MdpSaver);
    };
} // namespace persist
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_PERSIST_MDP_SAVER_H__
