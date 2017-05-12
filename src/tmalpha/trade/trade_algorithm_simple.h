
#ifndef __FH_TMALPHA_TRADE_TRADE_ALGORITHM_SIMPLE_H__
#define __FH_TMALPHA_TRADE_TRADE_ALGORITHM_SIMPLE_H__

#include "tmalpha/trade/trade_algorithm.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    class TradeAlgorithmSimple : public TradeAlgorithm
    {
        public:
            TradeAlgorithmSimple() {};
            virtual ~TradeAlgorithmSimple() {};

        public:
            // 对指定的订单，在当前待成交订单列表中找到能够和其匹配成交的订单类表；
            // 返回剩余未成交数量和是否需要马上删除该剩余数量
            std::pair<OrderSize, bool> Matches(std::vector<MatchedOrder> &matched_orders,
                                                                               const pb::ems::Order &order,
                                                                               const std::map<ComparablePrice, std::list<pb::ems::Order*>> &opposite_working_orders) override
            {
                // 将订单的价格转换成可比较的价格
                ComparablePrice order_price(order);
                // 待匹配手数
                OrderSize order_size = order.quantity();

                // 对每一个待匹配订单顺序检查能否成交（由于最优价排在最前面，所以可以按顺序匹配）
                for(const auto &p : opposite_working_orders)
                {
                    const ComparablePrice &working_price = p.first;
                    const std::list<pb::ems::Order*> &working_order = p.second;

                    // 如果双方都是 market price，跳过去
                    if(order_price.Is_market() && working_price.Is_market()) continue;

                    // 如果无法和当前价位匹配，那说明和以后的价位也不能匹配了（因为后面的价位都不优于当前价位），退出
                    // （如果是请求买，那么说明 出价 < 当前卖方订单价格；如果是请求卖，那么说明 出价 > 当前买方订单价格）
                    if(!ComparablePrice::Is_match(order_price, working_price)) goto END;

                    // 价位可以匹配时，将该价位的待匹配订单按（时间）顺序一一匹配
                    for(pb::ems::Order *w : working_order)
                    {
                        // 匹配价格
                        OrderPrice matched_price = working_price.Is_market() ? order_price.Price() : working_price.Price();
                        // 匹配手数
                        OrderSize matched_size = std::min(order_size, w->quantity());
                        // 记录匹配订单
                        matched_orders.push_back(MatchedOrder{w, matched_price, matched_size});
                        // 剩余手数
                        order_size = order_size - matched_size;
                        // 全部匹配掉了
                        if(order_size == 0) goto END;
                    }
                }

                END:
                if(order_size == 0)
                {
                    // 全部匹配掉了，直接返回 false（没有剩余未匹配数量可删除）
                    return {0, false};
                }
                else if(order.tif() == pb::ems::TimeInForce::TIF_FAK)
                {
                    // 如果是 FAK，说明允许部分匹配，但是剩余数量的需要删除
                    return {order_size, true};
                }
                else if(order.tif() == pb::ems::TimeInForce::TIF_FOK)
                {
                    // 如果是 FOK，说明不允许部分匹配，需要清除匹配结果，将原订单的数量全部删除
                    matched_orders.clear();
                    return {order.quantity(), true};
                }
                else
                {
                    // 其他场合无需删除剩余数量
                    return {order_size, false};
                }
            }

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeAlgorithmSimple);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_ALGORITHM_SIMPLE_H__
