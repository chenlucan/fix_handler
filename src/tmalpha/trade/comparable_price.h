#ifndef __FH_TMALPHA_TRADE_COMPARABLE_PRICE_H__
#define __FH_TMALPHA_TRADE_COMPARABLE_PRICE_H__

#include <string>
#include <ostream>
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/trade_types.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    class ComparablePrice
    {
        private:
            // 是 market order 的场合，其价位定义为 0
            static const std::uint32_t MARKET_ORDER_PRICE = 0;

        public:
            ComparablePrice(bool is_buy, OrderPrice price) : m_is_buy(is_buy), m_price(price) {}

            ComparablePrice(const pb::ems::Order &order)
            : ComparablePrice(order.buy_sell() == pb::ems::BuySell::BS_Buy, TO_ORDER_PRICE(std::stod(order.price()))) {}

            virtual ~ComparablePrice() {}

            // 查看一个价格和当前对手方的价位能否匹配，匹配即成交
            static bool Is_match(const ComparablePrice &price, const ComparablePrice &opposite_price)
            {
                // 不能匹配同一方（买和买，或者卖和卖）
                if(price.Is_buy() == opposite_price.Is_buy()) throw std::invalid_argument("cannot match same side");

                // 都是 market 订单，不能匹配
                if(price.Is_market() && opposite_price.Is_market()) return false;

                // 有一方是 market 订单，成交
                if(price.Is_market() || opposite_price.Is_market()) return true;

                // 价格相同，匹配成功
                if (price.Price() == opposite_price.Price()) return true;

                // 请求买入的场合，如果买价优于（高于）对手方价格，匹配成功
                if (price.Is_buy()) return price.Price() > opposite_price.Price();
                // 请求卖出的场合，如果卖价优于（低于）对手方价格，匹配成功
                else return price.Price() < opposite_price.Price();
            }

            // 比较两个价格，a < b 说明 a 的价格比 b 更优
            // 用于在 map 中排序：最优的排在最前面
            bool operator <(const ComparablePrice &p) const
            {
                // 不能比较非同一方（买和卖）
                if(this->Is_buy() != p.Is_buy()) throw std::invalid_argument("cannot compare buy to sell");

                // market 订单始终排在最前面（都是 market 的话，返回 false）
                if(this->Is_market()) return !p.Is_market();
                if(p.Is_market()) return false;

                // 买的场合价格高的排前面，卖的场合价格低的排前面
                return this->Is_buy() ? this->Price() > p.Price() : this->Price() < p.Price();
            }

            // 比较两个价格是否相等
            bool operator ==(const ComparablePrice &p) const
            {
                // 不能比较非同一方（买和卖）
                if(this->Is_buy() != p.Is_buy()) throw std::invalid_argument("cannot compare buy to sell");
                return this->Price() == p.Price();
            }

            // 比较两个价格是否小于等于
            bool operator <=(const ComparablePrice &p) const { return this->operator <(p) || this->operator ==(p); }

            // 比较两个价格是否大于
            bool operator >(const ComparablePrice &p) const { return !this->operator <=(p); }

            // 比较两个价格是否大于等于
            bool operator >=(const ComparablePrice &p) const { return !this->operator <(p); }

            // 比较两个价格是否不相等
            bool operator !=(const ComparablePrice &p) const { return !this->operator ==(p); }

        public:
            OrderPrice Price() const { return m_price; }
            bool Is_buy() const { return m_is_buy; }
            bool Is_market() const { return m_price == MARKET_ORDER_PRICE; }
            std::string To_string() const
            {
                return (this->Is_buy() ? "Buy at " : "Sell at ") + (this->Is_market() ? "Market" : std::to_string(TO_REAL_PRICE(this->Price())));
            }

        private:
            bool m_is_buy;
            OrderPrice m_price;
    };

    inline std::ostream &operator <<(std::ostream & out, const ComparablePrice &p)
    {
        out << p.To_string();
        return out;
    }
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_COMPARABLE_PRICE_H__
