#ifndef __FH_CME_MARKET_MESSAGE_INSTRUMENT_H__
#define __FH_CME_MARKET_MESSAGE_INSTRUMENT_H__

#include <sstream>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    // 产品的定义情报
    struct Instrument
    {
        std::uint32_t packet_seq_num;
        std::uint16_t template_id;
        std::uint16_t appId;
        char securityUpdateAction;      // A: add  D: delete  M:modify
        std::uint32_t securityID;
        std::string symbol;
        std::pair<std::int64_t, std::int8_t> minPriceIncrement;
        std::pair<std::int64_t, std::int8_t> highLimitPrice;
        std::pair<std::int64_t, std::int8_t> lowLimitPrice;
        std::uint8_t depthGBX;    // GBX- Real Book depth
        std::uint8_t depthGBI;        // GBI-Implied Book depth

        std::string To_string() const
        {
            std::ostringstream os;
            os << "instrument={";
            os << "packet_seq_num=" << packet_seq_num << " ";
            os << "template_id=" << template_id << " ";
            os << "securityID=" << securityID << " ";
            os << "symbol=" << symbol << " ";
            os << "securityUpdateAction=" << securityUpdateAction << " ";
            os << "minPriceIncrement={" << minPriceIncrement.first << "," << (int)minPriceIncrement.second << "} ";
            os << "highLimitPrice={" << highLimitPrice.first << "," << (int)highLimitPrice.second << "} ";
            os << "lowLimitPrice={" << lowLimitPrice.first << "," << (int)lowLimitPrice.second << "} ";
            os << "depthGBX=" << (int)depthGBX << " ";
            os << "depthGBI=" << (int)depthGBI << "}";

            return os.str();
        }
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_INSTRUMENT_H__
