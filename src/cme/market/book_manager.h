#ifndef __FH_CME_MARKET_BOOK_MANAGER_H__
#define __FH_CME_MARKET_BOOK_MANAGER_H__

#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/book_state_controller.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/message/message_parser_r.h"
#include "cme/market/message/message_parser_x.h"
#include "cme/market/message/message_parser_w.h"
#include "cme/market/definition_manager.h"
#include "cme/market/status_manager.h"
#include "core/market/marketlisteneri.h"

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
            void Merge_with_recovery(std::uint32_t message_seq, std::vector<fh::cme::market::message::Book> &increment_books);
            void On_definition_changed(const fh::cme::market::message::Instrument &instrument);
            void Send(bool is_bbo_changed, const fh::cme::market::BookState *state);
            void Send(const fh::cme::market::message::Book *trade_book);
            bool Is_BBO_changed(const fh::cme::market::message::Book &b);

        private:
            // 保存 revocery message 解析出的 books 情报
            std::vector<fh::cme::market::message::Book> m_recovery_books;
            // 这个指向 recovery books 数组的一个位置，从该位置开始的数据都是需要继续保存的(在保存 increment books 数据之前)
            // 这个位置的 message 的 LastMsgSeqNumProcessed 的值应该是下面二者之一：
            // 1.等于下一条要处理的 increment message 的 MsgSeqNum
            // 2.等于下一条要处理的 increment message 的 MsgSeqNum - 1（该位置是 recovery books 数组头的场合）
            std::vector<fh::cme::market::message::Book>::const_iterator m_recovery_wait_merge;
            fh::cme::market::message::MessageParserR m_parser_r;
            fh::cme::market::message::MessageParserX m_parser_x;
            fh::cme::market::message::MessageParserW m_parser_w;
            BookStateController m_book_state_controller;
            fh::core::market::MarketListenerI *m_book_sender;
            fh::cme::market::DefinitionManager m_definition_manager;
            fh::cme::market::StatusManager m_status_manager;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_MANAGER_H__
