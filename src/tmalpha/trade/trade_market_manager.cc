
#include "core/assist/logger.h"
#include "tmalpha/trade/trade_market_manager.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    TradeMarketManager::TradeMarketManager()
    : m_market_listener(nullptr), m_market_states()
    {
        // noop
    }

    TradeMarketManager::~TradeMarketManager()
    {
        for(const auto &m : m_market_states) { delete m.second; }
    }

    void TradeMarketManager::Set_listener(fh::core::market::MarketListenerI *market_listener)
    {
        m_market_listener = market_listener;
    }

    // 添加一个合约定义
    void TradeMarketManager::Add_contract(const std::string &contract_name, std::uint32_t depth)
    {
        // 添加合约
        m_market_states[contract_name] = new TradeMarketState(contract_name, depth);
    }

    // 将合约信息对外通知
    void TradeMarketManager::Send_contract(const pb::dms::Contract &contract)
    {
        if(m_market_listener) m_market_listener->OnContractDefinition(contract);
    }

    // 订单创建时修改行情数据
    void TradeMarketManager::Change_market_on_order_created(const pb::ems::Order *order)
    {
        TradeMarketState *market = m_market_states[order->contract()];
        // 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
        int result = market->On_order_created(order);
        if(result != 0) this->Send_l2(market);
        if(result == 2) this->Send_bbo(market);
    }

    // 订单被删除时修改行情数据
    void TradeMarketManager::Change_market_on_order_deleted(const pb::ems::Order *deleted_order)
    {
        TradeMarketState *market = m_market_states[deleted_order->contract()];
        // 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
        int result = market->On_order_deleted(deleted_order);
        if(result != 0) this->Send_l2(market);
        if(result == 2) this->Send_bbo(market);
    }

    void TradeMarketManager::Change_market_on_order_filled(const pb::ems::Order *order, OrderSize fill_size)
    {
        TradeMarketState *market = m_market_states[order->contract()];
        // 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
        int result = market->On_order_filled(order, fill_size);
        if(result != 0) this->Send_l2(market);
        if(result == 2) this->Send_bbo(market);
    }

    // 发布最新行情数据：L2 行情
    void TradeMarketManager::Send_l2(const TradeMarketState *market)
    {
        // 发送 L2 数据
        if(m_market_listener) m_market_listener->OnL2(market->L2());
    }

    // 发布最新行情数据：BBO 行情
    void TradeMarketManager::Send_bbo(const TradeMarketState *market)
    {
        if(m_market_listener == nullptr) return;

        // 发送 BBO 数据
        pb::dms::BBO bbo = market->BBO();
        if(bbo.has_bid() && bbo.has_offer())
        {
            m_market_listener->OnBBO(bbo);
        }
        else if(!bbo.has_bid() && !bbo.has_offer())
        {
            // 都不存在，就不发送了
        }
        else if(bbo.has_bid())
        {
            pb::dms::Bid bid;
            bid.set_contract(bbo.contract());
            pb::dms::DataPoint *dp = bid.mutable_bid();
            dp->set_price(bbo.bid().price());
            dp->set_size(bbo.bid().size());
            m_market_listener->OnBid(bid);
        }
        else
        {
            pb::dms::Offer offer;
            offer.set_contract(bbo.contract());
            pb::dms::DataPoint *dp = offer.mutable_offer();
            dp->set_price(bbo.offer().price());
            dp->set_size(bbo.offer().size());
            m_market_listener->OnOffer(offer);
        }
    }

    // 发布交易信息
    void TradeMarketManager::Send_trade(const std::string &contract_name, OrderPrice trade_price, OrderSize trade_quantity)
    {
        if(m_market_listener == nullptr) return;

        pb::dms::Trade trade;
        trade.set_contract(contract_name);
        pb::dms::DataPoint *last = trade.mutable_last();
        last->set_price(TO_REAL_PRICE(trade_price));
        last->set_size(trade_quantity);
        m_market_listener->OnTrade(trade);
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh
