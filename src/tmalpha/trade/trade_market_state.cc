
#include <sstream>
#include "core/assist/logger.h"
#include "tmalpha/trade/trade_market_state.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    TradeMarketState::TradeMarketState(const std::string &contract_name, std::uint32_t depth)
    : m_contract_name(contract_name), m_depth(depth), m_bid(), m_ask()
    {
        // noop
    }

    TradeMarketState::~TradeMarketState()
    {
        // noop
    }

    // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
    int TradeMarketState::On_order_created(const pb::ems::Order *order)
    {
        // market 订单不影响行情
        if(order->order_type() == pb::ems::OrderType::OT_Market) return 0;

        // 订单价格和数量
        ComparablePrice price(*order);
        OrderSize size = order->quantity();

        std::map<ComparablePrice, OrderSize> &current_states = price.Is_buy() ? m_bid : m_ask;
        // 如果有价位一致的，直接累加数量；否则添加该价位
        if(current_states.find(price) != current_states.end()) current_states[price] += size;
        else current_states[price] = size;

        LOG_INFO("add(", m_contract_name, "): ", price.To_string(), ", size=", size);
        LOG_INFO("current books: ", this->To_string());

        // 如果影响到了最大深度内的数据，返回 1（只影响 L2）或者 2（影响 BBO），否则返回 0
        int level = std::distance(current_states.begin(), current_states.find(price));
        return level >= (int)m_depth ? 0 : (level == 0 ? 2 : 1);
    }

    // 删除一个订单
    // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
    int TradeMarketState::On_order_deleted(const pb::ems::Order *order)
    {
        return this->Delete_order(order, order->quantity());
    }

    // 订单成交指定数量
    // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
    int TradeMarketState::On_order_filled(const pb::ems::Order *order, OrderSize filled_size)
    {
        // 订单成交，就把成交掉的价位上的数量相应的减少
        return this->Delete_order(order, filled_size);
    }

    // 按照本合约的最大深度，提取出 L2 行情数据
    pb::dms::L2 TradeMarketState::L2() const
    {
        pb::dms::L2 l2_info;
        l2_info.set_contract(m_contract_name);
        for(auto p = m_bid.begin(); p != m_bid.end() ; ++p)
        {
            TradeMarketState::Fill_data_point(l2_info.add_bid(), *p);
            if(l2_info.bid_size() >= (int)m_depth) break;
        }
        for(auto p = m_ask.begin(); p != m_ask.end(); ++p)
        {
            TradeMarketState::Fill_data_point(l2_info.add_offer(), *p);
            if(l2_info.offer_size() >= (int)m_depth) break;
        }

        return l2_info;
    }

    // 提取当前 BBO 行情数据
    pb::dms::BBO TradeMarketState::BBO() const
    {
        pb::dms::BBO bbo_info;
        bbo_info.set_contract(m_contract_name);
        if(!m_bid.empty())
        {
            TradeMarketState::Fill_data_point(bbo_info.mutable_bid(), *m_bid.cbegin());
        }
        if(!m_ask.empty())
        {
            TradeMarketState::Fill_data_point(bbo_info.mutable_offer(), *m_ask.cbegin());
        }

        return bbo_info;
    }

    // 删除一个订单的指定数量
    // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
    int TradeMarketState::Delete_order(const pb::ems::Order *order, OrderSize delete_size)
    {
        // market 订单不影响行情
        if(order->order_type() == pb::ems::OrderType::OT_Market) return 0;

        // 订单价格和删除数量
        ComparablePrice price(*order);
        OrderSize size = delete_size;

        std::map<ComparablePrice, OrderSize> &current_states = price.Is_buy() ? m_bid : m_ask;
        auto pos = current_states.find(price);
        if(pos == current_states.end()) throw std::invalid_argument("price not found: " + price.To_string());

        // 直接减掉该价位的数量
        current_states[price] -= size;
        // 记录下该价位的深度
        int level =  std::distance(current_states.begin(), pos);
        // 如果删除过后该价位的数量为 0 了，就删除该价位
        if(current_states[price] == 0) current_states.erase(price);

        LOG_INFO("delete(", m_contract_name, "): ", price, ", size=", size);
        LOG_INFO("current books: ", this->To_string());

        // 如果影响到了最大深度内的数据，返回 1（只影响 L2）或者 2（影响 BBO），否则返回 0
        return level >= (int)m_depth ? 0 : (level == 0 ? 2 : 1);
    }

    // 生成行情数据：价位，数量
    void TradeMarketState::Fill_data_point(pb::dms::DataPoint *dp, const std::pair<ComparablePrice, OrderSize> &kv)
    {
        dp->set_price(TO_REAL_PRICE(kv.first.Price()));
        dp->set_size(kv.second);
    }

    // 显示成字符串
    std::string TradeMarketState::To_string() const
    {
        std::ostringstream os;
        os << m_contract_name << "(" << m_depth <<"): ";
        os << "bid=";
        for(auto &b : m_bid) os << "[" << TO_REAL_PRICE(b.first.Price()) << ", " << b.second << "]";
        os << "offer=";
        for(auto &b : m_ask) os << "[" << TO_REAL_PRICE(b.first.Price()) << ", " << b.second << "]";
        return os.str();
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh
