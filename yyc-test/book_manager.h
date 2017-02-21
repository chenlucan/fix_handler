#ifndef __BOOK_MANAGER_H__
#define __BOOK_MANAGER_H__

#include "global.h"
#include "book.h"
#include "book_state_controller.h"
#include "mdp_message.h"
#include "zmq_sender.h"
#include "message_parser_d.h"
#include "message_parser_f.h"
#include "message_parser_r.h"
#include "message_parser_x.h"
#include "message_parser_w.h"

namespace rczg
{
	class BookManager
	{
		public:
			BookManager();
			virtual ~BookManager();

		public:
            // set received definition messages
            void Set_definition_data(std::vector<rczg::MdpMessage> *definition_datas);
            // set received recovery messages
            void Set_recovery_data(std::vector<rczg::MdpMessage> *recovery_datas);
            // parse increment message to books and send to target zeromq
            void Parse_to_send(const rczg::MdpMessage &message, rczg::ZmqSender &sender);

		private:
            void Parse_definition(const std::vector<rczg::MdpMessage> &messages);
            void Parse_recovery(const std::vector<rczg::MdpMessage> &messages);
            std::vector<Book> Parse_increment(const rczg::MdpMessage &message);
            void Update_definition(const rczg::MdpMessage &message);
            std::vector<Book> Merge_with_recovery(std::uint32_t message_seq, std::vector<Book> &increment_books);
            void On_definition_changed(std::uint32_t security_id);
            static void Move_append(std::vector<Book>& src, std::vector<Book>& dst);

		private:
            // 保存每个 SecurityID 对应的 definition 情报
            std::unordered_map<std::uint32_t , rczg::Instrument> m_instruments;
            // 保存 revocery message 解析出的 books 情报
            std::vector<rczg::Book> m_recovery_books;
            // 这个指向 recovery books 数组的一个位置，从该位置开始的数据都是需要继续保存的(在保存 increment books 数据之前)
            // 这个位置的 message 的 LastMsgSeqNumProcessed 的值应该是下面二者之一：
            // 1.等于下一条要处理的 increment message 的 MsgSeqNum
            // 2.等于下一条要处理的 increment message 的 MsgSeqNum - 1（该位置是 recovery books 数组头的场合）
            std::vector<rczg::Book>::const_iterator m_recovery_wait_merge;
            rczg::MessageParserD m_parser_d;
            rczg::MessageParserF m_parser_f;
            rczg::MessageParserR m_parser_r;
            rczg::MessageParserX m_parser_x;
            rczg::MessageParserW m_parser_w;
            BookStateController m_book_state_controller;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookManager);
	};
}

#endif // __BOOK_MANAGER_H__
