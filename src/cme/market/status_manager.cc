
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
        LOG_INFO("message securityID=", status.first, ", securityTradingStatus=", (int)status.second);

        std::string contract = "";
        if(status.first == mktdata::SecurityStatus30::securityIDNullValue())
        {
            // 如果没设置 security id
            contract = "";
        }
        else
        {
            contract = m_definition_manager->Get_symbol(status.first);
            // 如果找不到名称，就使用 id
            if(contract == "") contract = std::to_string(status.first);
        }

        StatusManager::Send(m_sender, contract, status.second);
    }

    void StatusManager::Send(fh::core::market::MarketListenerI *sender, const std::string &contract, mktdata::SecurityTradingStatus::Value status)
    {
        auto trading_status = StatusManager::Convert_status(status);
        switch(trading_status)
        {
            case 1: // Auctioning
                sender->OnContractAuctioning(contract);
                return;
            case 2: // Trading
                sender->OnContractTrading(contract);
                return;
            case 3: // NoTrading
                sender->OnContractNoTrading(contract);
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
