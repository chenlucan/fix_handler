#ifndef __FH_CME_MARKET_BOOK_MANAGER_H__
#define __FH_CME_MARKET_BOOK_MANAGER_H__

#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/book_state_controller.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/message_parser_r.h"
#include "cme/market/message/message_parser_x.h"
#include "cme/market/definition_manager.h"
#include "cme/market/status_manager.h"
#include "core/market/marketlisteneri.h"
#include "cme/market/recovery_manager.h"

namespace fh
{
namespace cme
{
namespace market
{
    class BookManager
    {
        public:
            explicit BookManager(fh::core::market::MarketListenerI *sender);
            virtual ~BookManager();

        public:
            // set received definition messages
            void Set_definition_data(const std::vector<fh::cme::market::message::MdpMessage> *definition_datas);
            // set received recovery messages
            void Set_recovery_data(const std::vector<fh::cme::market::message::MdpMessage> *recovery_datas);
            // parse increment message to books and send to target zeromq
            void Parse_to_send(const fh::cme::market::message::MdpMessage &message);

        private:
            void Parse_definition(const std::vector<fh::cme::market::message::MdpMessage> &messages);
            void Parse_recovery(const std::vector<fh::cme::market::message::MdpMessage> &messages);
            std::vector<fh::cme::market::message::Book> Parse_increment(const fh::cme::market::message::MdpMessage &message);
            void On_definition_changed(const fh::cme::market::message::Instrument &instrument);
            void Send(bool is_bbo_changed, const fh::cme::market::BookState *state);
            void Send_trade(const fh::cme::market::message::Book *trade_book);
            bool Is_BBO_changed(const fh::cme::market::message::Book &b);

        private:
            fh::cme::market::message::MessageParserR m_parser_r;
            fh::cme::market::message::MessageParserX m_parser_x;
            BookStateController m_book_state_controller;
            fh::core::market::MarketListenerI *m_book_sender;
            fh::cme::market::DefinitionManager m_definition_manager;
            fh::cme::market::StatusManager m_status_manager;
            fh::cme::market::RecoveryManager m_recovery_manager;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_MANAGER_H__
