
#ifndef __FH_TMALPHA_MARKET_DATA_PROVIDER_H__
#define __FH_TMALPHA_MARKET_DATA_PROVIDER_H__

#include <cstdint>
#include <vector>

namespace fh
{
namespace tmalpha
{
namespace market
{
    class DataProvider
    {
        public:
            DataProvider() {}
            virtual ~DataProvider() {}

        public:
            virtual void Range(const std::string &start_include, const std::string &end_exclude) = 0;
            virtual std::uint64_t Total_count() = 0;
            virtual std::uint64_t Query(std::vector<std::string> &result, std::uint64_t prev_last_record) = 0;
            virtual std::uint64_t Message_identify(const std::string &message) = 0;
            virtual std::uint64_t Message_send_time(const std::string &message) = 0;
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_DATA_PROVIDER_H__
