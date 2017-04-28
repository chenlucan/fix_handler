
#ifndef __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__
#define __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__

#include <cstdint>
#include <unordered_map>
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "pb/dms/dms.pb.h"
#include "tmalpha/market/market_replay_listener.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class MockMarketReplayListener : public DefaultMarketReplayListener
    {
        public:
            MockMarketReplayListener() : m_states() {}
            virtual ~MockMarketReplayListener() {}

        public:
            void On_state_changed(const std::unordered_map<std::string , pb::dms::L2> &states) override
            {
                m_states = states;
                DefaultMarketReplayListener::On_state_changed(states);
            }

            std::unordered_map<std::string , pb::dms::L2> &Get_states()
            {
                return m_states;
            }

        private:
            std::unordered_map<std::string , pb::dms::L2> m_states;

    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MOCK_MARKET_REPLAY_LISTENER_H__
