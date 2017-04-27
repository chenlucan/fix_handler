
#ifndef __FH_TMALPHA_MARKET_MARKET_REPLAY_LISTENER_H__
#define __FH_TMALPHA_MARKET_MARKET_REPLAY_LISTENER_H__

#include <cstdint>
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "pb/dms/dms.pb.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class MarketReplayListener
    {
        public:
            MarketReplayListener() {}
            virtual ~MarketReplayListener() {}

        public:
            virtual void On_state_changed(const std::unordered_map<std::string , pb::dms::L2> &) = 0;
    };

    class DefaultMarketReplayListener : public MarketReplayListener
    {
        public:
            DefaultMarketReplayListener() {}
            virtual ~DefaultMarketReplayListener() {}

        public:
            void On_state_changed(const std::unordered_map<std::string , pb::dms::L2> &states) override
            {
                for(const auto &l2 : states)
                {
                    LOG_INFO(fh::core::assist::utility::Format_pb_message(l2.second));
                }
            }
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MARKET_REPLAY_LISTENER_H__
