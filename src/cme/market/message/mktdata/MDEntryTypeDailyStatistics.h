/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _MKTDATA_MDENTRYTYPEDAILYSTATISTICS_H_
#define _MKTDATA_MDENTRYTYPEDAILYSTATISTICS_H_

#if defined(SBE_HAVE_CMATH)
/* cmath needed for std::numeric_limits<double>::quiet_NaN() */
#  include <cmath>
#  define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#  define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#else
/* math.h needed for NAN */
#  include <math.h>
#  define SBE_FLOAT_NAN NAN
#  define SBE_DOUBLE_NAN NAN
#endif

#if __cplusplus >= 201103L
#  include <cstdint>
#  include <string>
#  include <cstring>
#endif

#if __cplusplus >= 201103L
#  define SBE_CONSTEXPR constexpr
#else
#  define SBE_CONSTEXPR
#endif

#include <sbe/sbe.h>

using namespace sbe;

namespace mktdata {

class MDEntryTypeDailyStatistics
{
public:

    enum Value 
    {
        SettlementPrice = (char)54,
        ClearedVolume = (char)66,
        OpenInterest = (char)67,
        FixingPrice = (char)87,
        NULL_VALUE = (char)0
    };

    static MDEntryTypeDailyStatistics::Value get(const char value)
    {
        switch (value)
        {
            case 54: return SettlementPrice;
            case 66: return ClearedVolume;
            case 67: return OpenInterest;
            case 87: return FixingPrice;
            case 0: return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum MDEntryTypeDailyStatistics [E103]");
    }
};
}
#endif
