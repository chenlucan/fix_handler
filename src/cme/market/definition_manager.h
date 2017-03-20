#ifndef __FH_CME_MARKET_DEFINITION_MANAGER_H__
#define __FH_CME_MARKET_DEFINITION_MANAGER_H__

#include <unordered_map>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/message/message_parser_d.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DefinitionManager
    {
        public:
            explicit DefinitionManager(fh::core::zmq::ZmqSender *sender);
            virtual ~DefinitionManager();

        public:
            // 接受到产品定义消息（tag 35-MsgType=d）后保存产品情报
            void On_new_definition(
                    const fh::cme::market::message::MdpMessage &message,
                    std::function<void(const fh::cme::market::message::Instrument &instrument)> callback);
            // 根据 security id 获取其名称
            std::string Get_symbol(std::uint32_t security_id);

        private:
            void Update_definition(const fh::cme::market::message::Instrument &instrument,
                    std::function<void(const fh::cme::market::message::Instrument &instrument)> callback);
            void Send(const fh::cme::market::message::Instrument &instrument);

        private:
            // 保存每个 SecurityID 对应的 definition 情报
            std::unordered_map<std::uint32_t , fh::cme::market::message::Instrument> m_instruments;
            fh::cme::market::message::MessageParserD m_parser_d;
            fh::core::zmq::ZmqSender *m_sender;

        private:
            DISALLOW_COPY_AND_ASSIGN(DefinitionManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DEFINITION_MANAGER_H__
