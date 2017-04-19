
#include <set>
#include "cme/market/recovery_manager.h"
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

    RecoveryManager::RecoveryManager(BookStateController *book_state_controller)
    : m_parser_w(), m_recovery_books(), m_recovery_wait_merge(m_recovery_books.cbegin()),
      m_book_state_controller(book_state_controller)
    {
        // noop
    }

    RecoveryManager::~RecoveryManager()
    {
        // noop
    }

    void RecoveryManager::On_new_recovery(
            const std::vector<fh::cme::market::message::MdpMessage> &recovery_datas,
            std::function<void(const fh::cme::market::BookState *)> callback)
    {
        // 将 sbe 消息中的 book 解析出来
        std::for_each(recovery_datas.cbegin(), recovery_datas.cend(),
                [this](const fh::cme::market::message::MdpMessage &message){
                    std::vector<fh::cme::market::message::Book> books;
                    m_parser_w.Parse(message, books);
                    m_recovery_books.insert(m_recovery_books.end(),
                            std::make_move_iterator(books.begin()), std::make_move_iterator(books.end()));
                }
        );
        m_recovery_wait_merge = m_recovery_books.cbegin();

        // 根据每一个 book 信息更新其 book state 信息
        std::set<const fh::cme::market::BookState *> state_will_sent;
        std::for_each(m_recovery_books.cbegin(), m_recovery_books.cend(),
            [this, &state_will_sent](const fh::cme::market::message::Book &book){
                auto result = m_book_state_controller->Modify_state(book);
                if(result.first == 2 && result.second != nullptr)
                {
                    // 如果造成 book state 的变化，记录之，最后一起发送
                    state_will_sent.insert(static_cast<const fh::cme::market::BookState *>(result.second));
                }
            }
        );

        // 将需要发送的 book state 信息发送出去
        std::for_each(state_will_sent.cbegin(), state_will_sent.cend(), [&callback](const fh::cme::market::BookState *bs){
            callback(bs);
        });
    }

    void RecoveryManager::Remove_past_books(std::uint32_t message_seq, std::vector<fh::cme::market::message::Book> &increment_books)
    {
        if(m_recovery_wait_merge == m_recovery_books.cend() || increment_books.empty())
        {
            // 没有 recovery 数据或者都处理完了，或者需要检查的数据本身是空的，直接返回
            return;
        }

        // 首先跳过 LastMsgSeqNumProcessed 比 message_seq 小的记录
        while(m_recovery_wait_merge != m_recovery_books.cend() && m_recovery_wait_merge->packet_seq_num < message_seq)
        {
            ++m_recovery_wait_merge;
        }

        // 看看大于等于 message_seq 的数据中和 increment_books 数据中有没有 SecurityID 一样的数据，有的话需要删除（说明 increment book 中是旧的状态）
        for(auto pos = m_recovery_wait_merge; pos != m_recovery_books.cend(); ++pos)
        {
            increment_books.erase(
                    std::remove_if(increment_books.begin(), increment_books.end(), [pos](fh::cme::market::message::Book &ib){ return ib.securityID == pos->securityID; }),
                    increment_books.end()
            );
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
