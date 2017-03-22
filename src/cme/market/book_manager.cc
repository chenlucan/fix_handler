
#include "cme/market/book_manager.h"
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

    BookManager::BookManager(fh::cme::market::BookSender *sender)
    : m_recovery_books(), m_recovery_wait_merge(m_recovery_books.cbegin()),
      m_parser_f(), m_parser_r(), m_parser_x(), m_parser_w(),
      m_book_state_controller(), m_book_sender(sender), m_definition_manager(sender)
    {
        // noop
    }

    BookManager::~BookManager()
    {
        // noop
    }

    // set received definition messages
    void BookManager::Set_definition_data(const std::vector<fh::cme::market::message::MdpMessage> *definition_datas)
    {
        this->Parse_definition(*definition_datas);
    }

    // set received recovery messages
    void BookManager::Set_recovery_data(const std::vector<fh::cme::market::message::MdpMessage> *recovery_datas)
    {
        this->Parse_recovery(*recovery_datas);
    }

    // parse increment message to books and send to target zeromq
    void BookManager::Parse_to_send(const fh::cme::market::message::MdpMessage &message)
    {
        fh::core::assist::TimeMeasurer t;

        std::vector<fh::cme::market::message::Book> increment_books = this->Parse_increment(message);
        if(increment_books.empty())
        {
            LOG_INFO("parsed. no book info in message.");
            return;
        }

        // 看看有没有 BookReset
        auto index = std::find_if(increment_books.rbegin(), increment_books.rend(),
                                                [](const fh::cme::market::message::Book &b) { return b.mDEntryType == mktdata::MDEntryType::BookReset; });
        if(index == increment_books.rend())
        {
            // 没有的话，就和恢复数据进行 merge
            this->Merge_with_recovery(message.packet_seq_num(), increment_books);
        }
        else
        {
            // 有的话，在它之前的数据都不要了，还要把保存的恢复数据扔掉（留下 BookReset 自己）
            increment_books.erase(increment_books.begin(), std::next(index).base());
            m_recovery_books.clear();
            m_recovery_wait_merge = m_recovery_books.cbegin();
        }

        LOG_INFO("parse to books: ", t.Elapsed_nanoseconds(), "ns, count=", increment_books.size());

        std::for_each(increment_books.cbegin(), increment_books.cend(),
                [this, &t](const fh::cme::market::message::Book &b)
                {
                    auto changed_state_or_trade = m_book_state_controller.Modify_state(b);
                    std::uint8_t flag = changed_state_or_trade.first;   // 0: no data  changed 1: reset  2: book state  3: trade state
                    const void *data = changed_state_or_trade.second;
                    if(flag == 1)
                    {
                        // TODO reset 后如果通知策略端
                    }
                    else if(flag == 2 && data != nullptr)
                    {
                        this->Send(static_cast<const fh::cme::market::BookState *>(data));
                        LOG_INFO("send to zmq(book state): ", t.Elapsed_nanoseconds(), "ns");
                    }
                    else if(flag == 3 && data != nullptr)
                    {
                        this->Send(static_cast<const fh::cme::market::message::Book *>(data));
                        LOG_INFO("send to zmq(trade book): ", t.Elapsed_nanoseconds(), "ns");
                    }
                }
        );
    }

    void BookManager::Send(const fh::cme::market::BookState *state)
    {
        // 发送二级行情
        pb::dms::L2 l2_info;
        l2_info.set_contract(state->symbol);
        std::for_each(state->bid.cbegin(), state->bid.cend(), [&l2_info](const BookPrice &p){
            pb::dms::DataPoint *bid = l2_info.add_bid();
            bid->set_price(fh::cme::market::message::utility::Get_price(p.mDEntryPx));
            bid->set_size(p.mDEntrySize);
        });
        std::for_each(state->ask.cbegin(), state->ask.cend(), [&l2_info](const BookPrice &p){
            pb::dms::DataPoint *ask = l2_info.add_offer();
            ask->set_price(fh::cme::market::message::utility::Get_price(p.mDEntryPx));
            ask->set_size(p.mDEntrySize);
        });

        m_book_sender->OnL2(l2_info);

        // 发送最优价位
        bool is_bid_empty = state->bid.empty();
        bool is_ask_empty = state->ask.empty();
        if(is_bid_empty && is_ask_empty)
        {
            // TODO bid 和 ask 都不存在
        }
        else if(!is_bid_empty && !is_ask_empty)
        {
            // bid 和 ask 都存在
            pb::dms::BBO bbo_info;
            bbo_info.set_contract(state->symbol);
            pb::dms::DataPoint *bid = bbo_info.mutable_bid();
            bid->set_price(fh::cme::market::message::utility::Get_price(state->bid.front().mDEntryPx));
            bid->set_size(state->bid.front().mDEntrySize);
            pb::dms::DataPoint *ask = bbo_info.mutable_offer();
            ask->set_price(fh::cme::market::message::utility::Get_price(state->ask.front().mDEntryPx));
            ask->set_size(state->ask.front().mDEntrySize);

            m_book_sender->OnBBO(bbo_info);
        }
        else if(is_bid_empty)
        {
            // 只有 ask
            pb::dms::Offer offer_info;
            offer_info.set_contract(state->symbol);
            pb::dms::DataPoint *offer = offer_info.mutable_offer();
            offer->set_price(fh::cme::market::message::utility::Get_price(state->ask.front().mDEntryPx));
            offer->set_size(state->ask.front().mDEntrySize);

            m_book_sender->OnOffer(offer_info);
        }
        else
        {
            // 只有 bid
            pb::dms::Bid bid_info;
            bid_info.set_contract(state->symbol);
            pb::dms::DataPoint *bid = bid_info.mutable_bid();
            bid->set_price(fh::cme::market::message::utility::Get_price(state->bid.front().mDEntryPx));
            bid->set_size(state->bid.front().mDEntrySize);

            m_book_sender->OnBid(bid_info);
        }
    }

    void BookManager::Send(const fh::cme::market::message::Book *trade_book)
    {
        // 发送 trade 数据
        pb::dms::Trade trade;
        trade.set_contract(m_definition_manager.Get_symbol(trade_book->securityID));
        pb::dms::DataPoint *last = trade.mutable_last();
        last->set_price(fh::cme::market::message::utility::Get_price(trade_book->mDEntryPx));
        last->set_size(trade_book->mDEntrySize);

        m_book_sender->OnTrade(trade);
    }

    void BookManager::Parse_definition(const std::vector<fh::cme::market::message::MdpMessage> &messages)
    {
        std::for_each(messages.cbegin(), messages.cend(),
                [this](const fh::cme::market::message::MdpMessage &message){
                    m_definition_manager.On_new_definition(message, std::bind(&BookManager::On_definition_changed, this, std::placeholders::_1));
                }
        );
    }

    void BookManager::Parse_recovery(const std::vector<fh::cme::market::message::MdpMessage> &messages)
    {
        // parse revocery data, 35=W
        std::for_each(messages.cbegin(), messages.cend(),
                [this](const fh::cme::market::message::MdpMessage &message){
                    std::vector<fh::cme::market::message::Book> books;
                    m_parser_w.Parse(message, books);
                    BookManager::Move_append(books, m_recovery_books);
                }
        );

        m_recovery_wait_merge = m_recovery_books.cbegin();
    }

    std::vector<fh::cme::market::message::Book> BookManager::Parse_increment(const fh::cme::market::message::MdpMessage &message)
    {
        std::vector<fh::cme::market::message::Book> books;

        char type = message.message_type();
        if(type == 'f')
        {
            m_parser_f.Parse(message, books);
        }
        else if(type == 'd')
        {
            m_definition_manager.On_new_definition(message, std::bind(&BookManager::On_definition_changed, this, std::placeholders::_1));
        }
        else if(type == 'R')
        {
            m_parser_r.Parse(message, books);
        }
        else if(type == 'X')
        {
            m_parser_x.Parse(message, books);
        }
        else
        {
            LOG_DEBUG("message type is ", type, ", ignore.");
        }

        return books;
    }

    void BookManager::Merge_with_recovery(std::uint32_t message_seq, std::vector<fh::cme::market::message::Book> &increment_books)
    {
        if(m_recovery_books.empty())
        {
            // 没有 recovery 数据，直接返回
            return;
        }

        std::vector<fh::cme::market::message::Book> merged_books;
        auto old_pos = m_recovery_wait_merge;

        // 首先将 recovery books 中 LastMsgSeqNumProcessed 在该 message_seq 之前（包括）的 books 保存下来
        while(m_recovery_wait_merge != m_recovery_books.cend() && m_recovery_wait_merge->packet_seq_num <= message_seq)
        {
            merged_books.push_back(std::ref(*m_recovery_wait_merge));
            ++m_recovery_wait_merge;
        }

        // 看看 increment_books 中有没有 SecurityID 在 recovery books 的 [之前保存下的位置，末尾] 中存在
        for(auto pos = old_pos; pos != m_recovery_books.cend(); ++pos)
        {
            // 只看 LastMsgSeqNumProcessed 在该 increment book 的 message_seq 之后（包括）的数据
            if(pos->packet_seq_num >= message_seq)
            {
                // 删除 increment books 中和当前 recovery book 一致的数据 （SecurityID 一样）
                increment_books.erase(
                        std::remove_if(increment_books.begin(), increment_books.end(), [pos](fh::cme::market::message::Book &ib){ return ib.securityID == pos->securityID; }),
                        increment_books.end()
                );
            }
        }

        // 剩下的 increment books 是需要保存的
        BookManager::Move_append(increment_books, merged_books);
        increment_books = std::move(merged_books);
    }

    void BookManager::On_definition_changed(const fh::cme::market::message::Instrument &instrument)
    {
        // 根据更新的产品信息修正对应的 book state 情报
        m_book_state_controller.Create_or_shrink(instrument);
    }

    // move vector contents to another vector
    void BookManager::Move_append(std::vector<fh::cme::market::message::Book>& src, std::vector<fh::cme::market::message::Book>& dst)
    {
        if (dst.empty())
        {
            dst = std::move(src);
        }
        else
        {
            dst.reserve(dst.size() + src.size());
            std::move(std::begin(src), std::end(src), std::back_inserter(dst));
            src.clear();
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
