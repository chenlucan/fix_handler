#ifndef __FH_CME_MARKET_RECOVERY_MANAGER_H__
#define __FH_CME_MARKET_RECOVERY_MANAGER_H__

#include <vector>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/message_parser_w.h"
#include "cme/market/book_state_controller.h"


namespace fh
{
namespace cme
{
namespace market
{
    class RecoveryManager
    {
        public:
            explicit RecoveryManager(BookStateController *book_state_controller);
            virtual ~RecoveryManager();

        public:
            void On_new_recovery(
                    const std::vector<fh::cme::market::message::MdpMessage> &recovery_datas,
                    std::function<void(const fh::cme::market::BookState *)> callback);
            // 从指定的 increment 中找出那些已经过时的 book（就是已经在 recovery book 中发送过最新状态的）删除掉
            void Remove_past_books(std::uint32_t message_seq, std::vector<fh::cme::market::message::Book> &increment_books);

        private:
            fh::cme::market::message::MessageParserW m_parser_w;
            std::vector<fh::cme::market::message::Book> m_recovery_books;
            // 这个指向 recovery books 数组的一个位置，在保存 increment books 数据之前，
            // 要利用从该位置开始的 recovery 数据对 increment books 进行检查
            // 这个位置的 message 的 LastMsgSeqNumProcessed 的值应该是下面二者之一：
            // 1.等于下一条要处理的 increment message 的 MsgSeqNum
            // 2.等于下一条要处理的 increment message 的 MsgSeqNum - 1（该位置是 recovery books 数组头的场合）
            std::vector<fh::cme::market::message::Book>::const_iterator m_recovery_wait_merge;
            BookStateController *m_book_state_controller;

        private:
            DISALLOW_COPY_AND_ASSIGN(RecoveryManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_RECOVERY_MANAGER_H__
