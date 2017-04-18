
#include "cme/market/status_manager.h"
#include "cme/market/message/message_utility.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    StatusManager::StatusManager(fh::core::market::MarketListenerI *sender, fh::cme::market::DefinitionManager *definition_manager)
    : m_parser_f(), m_sender(sender), m_definition_manager(definition_manager)
    {
        // noop
    }

    StatusManager::~StatusManager()
    {
        // noop
    }

    void StatusManager::On_new_status(const fh::cme::market::message::MdpMessage &message)
    {
        auto status = m_parser_f.Parse(message);
        auto security_id = status.first;
        auto trading_status = this->Convert_status(status.second);

        LOG_INFO("message securityID=", security_id, ", securityTradingStatus=", (int)status.second);

        if(security_id == mktdata::SecurityStatus30::securityIDNullValue())
        {
            // 如果没设置 security id
            this->Send("", trading_status);
            return;
        }

        std::string contract = m_definition_manager->Get_symbol(security_id);
        if(contract == "")
        {
            // 如果找不到名称，就使用 id
            this->Send(std::to_string(security_id), trading_status);
            return;
        }

        this->Send(contract, trading_status);
    }

    void StatusManager::Send(const std::string &contract, std::uint8_t flag)
    {
        switch(flag)
        {
            case 1: // Auctioning
                m_sender->OnContractAuctioning(contract);
                return;
            case 2: // Trading
                m_sender->OnContractTrading(contract);
                return;
            case 3: // NoTrading
                m_sender->OnContractNoTrading(contract);
                return;
            default: // other
                return;
        }
    }

    // 将收到的 SecurityTradingStatus 转换为： 1（Auctioning） 2（Trading） 3（NoTrading） 4（Other）
    std::uint8_t StatusManager::Convert_status(mktdata::SecurityTradingStatus::Value status)
    {
        switch(status)
        {
            case mktdata::SecurityTradingStatus::PreOpen:
                return 1;
            case mktdata::SecurityTradingStatus::ReadyToTrade:
                return 2;
            case mktdata::SecurityTradingStatus::TradingHalt:
            case mktdata::SecurityTradingStatus::NotAvailableForTrading:
            case mktdata::SecurityTradingStatus::Close:
                return 3;
            default:
                return 4;
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
