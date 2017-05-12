
#ifndef __FH_TMALPHA_TRADE_TRADE_ALGORITHM_H__
#define __FH_TMALPHA_TRADE_TRADE_ALGORITHM_H__

#include <vector>
#include <list>
#include "core/global.h"
#include "tmalpha/trade/comparable_price.h"


namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 被匹配（成交）的订单信息
    struct MatchedOrder
    {
        pb::ems::Order *match_order;
        OrderPrice match_price;
        OrderSize match_quantity;
    };

    class TradeAlgorithm
    {
        public:
            explicit TradeAlgorithm() {};
            virtual ~TradeAlgorithm() {} ;

        public:
            // 对指定的订单，在当前待成交订单列表中找到能够和其匹配成交的订单类表
            // 返回剩余未成交数量和是否需要马上删除该剩余数量
            virtual std::pair<OrderSize, bool> Matches(std::vector<MatchedOrder> &matched_orders,
                                                                                            const pb::ems::Order &order,
                                                                                            const std::map<ComparablePrice, std::list<pb::ems::Order*>> &opposite_working_orders) = 0;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeAlgorithm);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_ALGORITHM_H__
