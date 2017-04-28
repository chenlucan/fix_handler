
#ifndef __FH_TMALPHA_MARKET_DATA_CONSUMER_H__
#define __FH_TMALPHA_MARKET_DATA_CONSUMER_H__

#include <cstdint>
#include <unordered_map>
#include "pb/dms/dms.pb.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class DataConsumer
    {
        public:
            DataConsumer() {}
            virtual ~DataConsumer() {}

        public:
            virtual void Consume(const std::string &message) = 0;
            virtual std::unordered_map<std::string , pb::dms::L2> Get_state() = 0;
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_DATA_CONSUMER_H__
