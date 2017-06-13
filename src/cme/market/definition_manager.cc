
#include "cme/market/definition_manager.h"
#include "cme/market/message/message_utility.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"
#include "pb/dms/dms.pb.h"

namespace fh
{
namespace cme
{
namespace market
{

    DefinitionManager::DefinitionManager(fh::core::market::MarketListenerI *sender)
    : m_instruments(), m_parser_d(), m_sender(sender)
    {
        // noop
    }

    DefinitionManager::~DefinitionManager()
    {
        // noop
    }

    void DefinitionManager::On_new_definition(
            const fh::cme::market::message::MdpMessage &message,
            std::function<void(const fh::cme::market::message::Instrument &instrument)> callback)
    {
        std::vector<fh::cme::market::message::Instrument> instruments;
        m_parser_d.Parse(message, instruments);
        std::for_each(instruments.begin(), instruments.end(),
                [this, &callback](fh::cme::market::message::Instrument &i){ this->Update_definition(i, callback); });
    }

    std::string DefinitionManager::Get_symbol(std::uint32_t security_id)
    {
        auto definition = m_instruments.find(security_id);
        if(definition == m_instruments.end())
        {
            LOG_WARN("can not find definition: ", security_id);
            return "";
        }
        return definition->second.symbol;
    }

    void DefinitionManager::Update_definition(const fh::cme::market::message::Instrument &instrument,
            std::function<void(const fh::cme::market::message::Instrument &instrument)> callback)
    {
        LOG_DEBUG("new definition: ", instrument.To_string());

        if(instrument.securityUpdateAction == 'D')
        {
            // delete
            std::size_t count = m_instruments.erase(instrument.securityID);
            if(count == 0)
            {
                // 原来就没有对应的信息
                LOG_WARN("can not find origin instrument to delete: ", instrument.securityID);                
            }
            return;
        }
        else
        {
            // modify or add
            m_instruments[instrument.securityID] = instrument;
        }

        // 将新的定义情报发送出去
        fh::cme::market::DefinitionManager::Send(m_sender, instrument);
        // 回调下调用方
        callback(instrument);
    }

    void DefinitionManager::Send(fh::core::market::MarketListenerI *sender, const fh::cme::market::message::Instrument &instrument)
    {
        // 将更新的产品信息发送到策略 TODO 删除的场合如何发送
        pb::dms::Contract contract;
        contract.set_name(instrument.symbol);
        contract.set_tick_size(std::to_string(fh::cme::market::message::utility::Get_price(instrument.minPriceIncrement)));
//        contract.set_tick_value();         TODO
//        contract.set_yesterday_close_price();      TODO
        contract.set_upper_limit(std::to_string(fh::cme::market::message::utility::Get_price(instrument.highLimitPrice)));
        contract.set_lower_limit(std::to_string(fh::cme::market::message::utility::Get_price(instrument.lowLimitPrice)));
        contract.set_contract_type(pb::dms::ContractType::CT_Futures);      // 固定：期货
        // contract.set_lega(); // 暂不使用
        // contract.set_legb(); // 暂不使用

        sender->OnContractDefinition(contract);
    }

} // namespace market
} // namespace cme
} // namespace fh
