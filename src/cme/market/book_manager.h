#ifndef __FH_CME_MARKET_BOOK_MANAGER_H__
#define __FH_CME_MARKET_BOOK_MANAGER_H__

#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/book_state_controller.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/message/message_parser_d.h"
#include "cme/market/message/message_parser_f.h"
#include "cme/market/message/message_parser_r.h"
#include "cme/market/message/message_parser_x.h"
#include "cme/market/message/message_parser_w.h"

namespace fh
{
namespace cme
{
namespace market
{
    class BookManager
    {
        public:
            BookManager();
            virtual ~BookManager();

        public:
            // set received definition messages
            void Set_definition_data(const std::vector<fh::cme::market::message::MdpMessage> *definition_datas);
            // set received recovery messages
            void Set_recovery_data(const std::vector<fh::cme::market::message::MdpMessage> *recovery_datas);
            // parse increment message to books and send to target zeromq
            void Parse_to_send(const fh::cme::market::message::MdpMessage &message, fh::core::zmq::ZmqSender &sender);

        private:
            void Parse_definition(const std::vector<fh::cme::market::message::MdpMessage> &messages);
            void Parse_recovery(const std::vector<fh::cme::market::message::MdpMessage> &messages);
            std::vector<fh::cme::market::message::Book> Parse_increment(const fh::cme::market::message::MdpMessage &message);
            void Update_definition(const fh::cme::market::message::MdpMessage &message);
            std::vector<fh::cme::market::message::Book> Merge_with_recovery(std::uint32_t message_seq, std::vector<fh::cme::market::message::Book> &increment_books);
            void On_definition_changed(std::uint32_t security_id);
            static void Move_append(std::vector<fh::cme::market::message::Book>& src, std::vector<fh::cme::market::message::Book>& dst);

        private:
            // 保存每个 SecurityID 对应的 definition 情报
            std::unordered_map<std::uint32_t , fh::cme::market::message::Instrument> m_instruments;
            // 保存 revocery message 解析出的 books 情报
            std::vector<fh::cme::market::message::Book> m_recovery_books;
            // 这个指向 recovery books 数组的一个位置，从该位置开始的数据都是需要继续保存的(在保存 increment books 数据之前)
            // 这个位置的 message 的 LastMsgSeqNumProcessed 的值应该是下面二者之一：
            // 1.等于下一条要处理的 increment message 的 MsgSeqNum
            // 2.等于下一条要处理的 increment message 的 MsgSeqNum - 1（该位置是 recovery books 数组头的场合）
            std::vector<fh::cme::market::message::Book>::const_iterator m_recovery_wait_merge;
            fh::cme::market::message::MessageParserD m_parser_d;
            fh::cme::market::message::MessageParserF m_parser_f;
            fh::cme::market::message::MessageParserR m_parser_r;
            fh::cme::market::message::MessageParserX m_parser_x;
            fh::cme::market::message::MessageParserW m_parser_w;
            BookStateController m_book_state_controller;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_MANAGER_H__
