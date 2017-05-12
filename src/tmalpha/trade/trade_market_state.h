
#ifndef __FH_TMALPHA_TRADE_TRADE_MARKET_STATE_H__
#define __FH_TMALPHA_TRADE_TRADE_MARKET_STATE_H__

#include <map>
#include "core/global.h"
#include "pb/ems/ems.pb.h"
#include "tmalpha/trade/comparable_price.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 每个合约的深度行情数据，用于向外广播
    // 每个价位的行情都需要保存，即使超出了最大深度；否则某个价位被删除后就没有补充了
    class TradeMarketState
    {
        public:
            TradeMarketState(const std::string &contract_name, std::uint32_t depth);
            virtual ~TradeMarketState();

        public:
            // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
            int On_order_created(const pb::ems::Order *order);
            // 删除一个订单
            // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
            int On_order_deleted(const pb::ems::Order *order);
            // 订单成交指定数量
            // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
            int On_order_filled(const pb::ems::Order *order, OrderSize filled_size);

        public:
            // 按照本合约的最大深度，提取出 L2 行情数据
            pb::dms::L2 L2() const;
            // 提取当前 BBO 行情数据
            pb::dms::BBO BBO() const;

        private:
            // 删除一个订单的指定数量
            // 返回 0：不影响最大深度内的数据；1：影响 L2，不影响 BBO；2：影响 BBO
            int On_order_deleted(const pb::ems::Order *order, OrderSize delete_size);
            // 生成行情数据：价位，数量
            static void Fill_data_point(pb::dms::DataPoint *dp, const std::pair<ComparablePrice, OrderSize> &kv);

        private:
            // 合约名称
            std::string m_contract_name;
            // 深度
            std::uint32_t m_depth;
            // bid 行情数据（价位由高到低）
            std::map<ComparablePrice, OrderSize> m_bid;
            // ask 行情数据（价位由低到高）
            std::map<ComparablePrice, OrderSize> m_ask;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeMarketState);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_MARKET_STATE_H__
