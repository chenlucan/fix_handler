
#ifndef __FH_TMALPHA_TRADE_TRADE_TYPES_H__
#define __FH_TMALPHA_TRADE_TRADE_TYPES_H__

#include <cstdint>

namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 内部的价位都使用分单位价格
    typedef  std::uint32_t OrderPrice;
    // 手数
    typedef std::uint64_t OrderSize;

    // 内部价格和实际价格的转换
    #define TO_ORDER_PRICE(price) ((OrderPrice)(price*100))
    #define TO_REAL_PRICE(order_price) (order_price/100.00)
    //OrderPrice TO_ORDER_PRICE(double price) { return (OrderPrice)(price * 100); }
    //double TO_REAL_PRICE(OrderPrice order_price) { return order_price/100.00; }
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_TYPES_H__
