
#ifndef __FH_TMALPHA_TRADE_TRADE_CONTRACT_ASSIST_H__
#define __FH_TMALPHA_TRADE_TRADE_CONTRACT_ASSIST_H__

#include <string>
#include <sstream>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "core/global.h"
#include "core/assist/logger.h"
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"
#include "tmalpha/trade/trade_types.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    // 合约属性
    struct TradeContract    // price exponent is -2
    {
        std::string name;       // 名称
        std::uint32_t depth;            // 行情深度
        OrderPrice tick;                    // 步长
        OrderPrice min;                   // 最低价格
        OrderPrice max;                   // 最高价格

        std::string To_string() const
        {
            std::ostringstream os;
            os << "name=" << name << ", ";
            os << "depth=" << depth << ", ";
            os << "tick=" << tick << ", ";
            os << "min=" << min << ", ";
            os << "max=" << max;
            return os.str();
        }

        pb::dms::Contract To_dms() const
        {
            pb::dms::Contract contract;
            contract.set_name(name);
            contract.set_tick_size(std::to_string(TO_REAL_PRICE(tick)));
            contract.set_lower_limit(std::to_string(TO_REAL_PRICE(min)));
            contract.set_upper_limit(std::to_string(TO_REAL_PRICE(max)));
            return contract;
        }
    };

    enum class TradeOrderPreCheckStatus
    {
            NORMAL,                             // 正常
            CONTRACT_NOT_EXIST,   // 合约不存在
            PRICE_TOO_LOW,              // 订单的价格低于合约最低价
            PRICE_TOO_HIGH,             // 订单的价格高于合约最高价
            INVALID_PRICE                  // 订单的价格和 tick 不匹配
    };

    class TradeContractAssist
    {
        public:
            explicit TradeContractAssist() : m_contracts() {}
            virtual ~TradeContractAssist() {}

        public:
            // 添加合约
            void Add(const TradeContract &contract)
            {
                m_contracts[contract.name] = contract;
                LOG_INFO("add new contract: ", contract.To_string());
            }

            // 检查一个订单的价格是否合法
            TradeOrderPreCheckStatus Check_order(const pb::ems::Order &order) const
            {
                auto pos = m_contracts.find(order.contract());
                if(pos == m_contracts.end()) return TradeOrderPreCheckStatus::CONTRACT_NOT_EXIST;

                const TradeContract &tc = pos->second;
                double price = std::stod(order.price());
                OrderPrice nprice = TO_ORDER_PRICE(price);

                if(price == 0) return TradeOrderPreCheckStatus::NORMAL;        // 价格有些场合下可以不设置
                if(price != TO_REAL_PRICE(nprice)) return TradeOrderPreCheckStatus::INVALID_PRICE;     // 出现不足 1 分的价格
                if(nprice > tc.max) return TradeOrderPreCheckStatus::PRICE_TOO_HIGH;
                if(nprice < tc.min) return TradeOrderPreCheckStatus::PRICE_TOO_LOW;
                if((nprice - tc.min) % tc.tick != 0) return TradeOrderPreCheckStatus::INVALID_PRICE;

                return TradeOrderPreCheckStatus::NORMAL;
            }

            // 返回所有合约信息
            const std::unordered_map<std::string, TradeContract> &Contracts() const
            {
                return m_contracts;
            }

            // 查看指定合约是否存在
            bool Is_contract_exist(const std::string &name) const
            {
                return m_contracts.find(name) != m_contracts.end();
            }

        private:
            std::unordered_map<std::string, TradeContract> m_contracts;

        private:
            DISALLOW_COPY_AND_ASSIGN(TradeContractAssist);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_TRADE_CONTRACT_ASSIST_H__
