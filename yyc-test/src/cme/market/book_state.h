#ifndef __FH_CME_MARKET_BOOK_STATE_H__
#define __FH_CME_MARKET_BOOK_STATE_H__

#include <deque>
#include <sstream>
#include <algorithm>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace market
{
    // 具体一个层级的价格情报
    struct BookPrice
    {
        std::uint32_t numberOfOrders;    // order count
        std::uint32_t mDEntrySize;    // quantity
        std::uint64_t mDEntryPx;    // price, exponent is -7

        // TODO serialize to send
        std::string Serialize() const
        {
            std::ostringstream os;
            os << "price:";
            os << " count=" << numberOfOrders;
            os << " quantity=" << mDEntrySize;
            os << " price=" << mDEntryPx;

            return os.str();
        }
    };

    // 根据 book 情报实时更新后的行情实时状态（每个产品（SecurityID）的）
    struct BookState
    {
        std::uint8_t bookType;    // 1: Multiple Depth Book 2: Implied Book
        std::uint32_t securityID;
        std::uint8_t marketDepth;
        std::deque<BookPrice> bid;
        std::deque<BookPrice> ask;

        // TODO serialize to send
        std::string Serialize() const
        {
            std::ostringstream os;
            os << "book state:";
            os << " bookType=" << (int)bookType;
            os << " securityID=" << securityID;
            os << " marketDepth=" << (int)marketDepth;
            os << std::endl;

            os << "  bid:" << bid.size() << std::endl;
            std::for_each(bid.cbegin(), bid.cend(), [&os](const BookPrice &p){
                os << "     [" << p.Serialize() << "]" << std::endl;
            });

            os << "  ask:" << ask.size() << std::endl;
            std::for_each(ask.cbegin(), ask.cend(), [&os](const BookPrice &a){
                os << "     [" << a.Serialize() << "]" << std::endl;
            });

            return os.str();
        }
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_STATE_H__
