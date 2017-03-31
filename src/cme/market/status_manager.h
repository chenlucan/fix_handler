#ifndef __FH_CME_MARKET_STATUS_MANAGER_H__
#define __FH_CME_MARKET_STATUS_MANAGER_H__

#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/message_parser_f.h"
#include "core/market/marketlisteneri.h"
#include "cme/market/definition_manager.h"


namespace fh
{
namespace cme
{
namespace market
{
    class StatusManager
    {
        public:
            StatusManager(fh::core::market::MarketListenerI *sender, fh::cme::market::DefinitionManager *definition_manager);
            virtual ~StatusManager();

        public:
            // 接受到产品状态消息（tag 35-MsgType=f）后发送出去
            void On_new_status(const fh::cme::market::message::MdpMessage &message);

        private:
            void Send(const std::string &contract, std::uint8_t flag);
            // 将收到的 SecurityTradingStatus 转换为： 1（Auctioning） 2（Trading） 3（NoTrading） 4（Other）
            std::uint8_t Convert_status(mktdata::SecurityTradingStatus::Value status);

        private:
            fh::cme::market::message::MessageParserF m_parser_f;
            fh::core::market::MarketListenerI *m_sender;
            fh::cme::market::DefinitionManager *m_definition_manager;

        private:
            DISALLOW_COPY_AND_ASSIGN(StatusManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_STATUS_MANAGER_H__
