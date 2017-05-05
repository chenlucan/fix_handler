
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

    BookManager::BookManager(fh::core::market::MarketListenerI *sender)
    : m_parser_r(), m_parser_x(), m_book_state_controller(), m_book_sender(sender),
      m_definition_manager(sender), m_status_manager(sender, &m_definition_manager),
      m_recovery_manager(&m_book_state_controller)
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
        LOG_INFO("parsed. books count in message=", increment_books.size());

        // 和恢复数据进行 merge：删除 increment 中过时的 book
        m_recovery_manager.Remove_past_books(message.packet_seq_num(), increment_books);

        LOG_INFO("parse to books(after merge): ", t.Elapsed_nanoseconds(), "ns, count=", increment_books.size());

        std::for_each(increment_books.cbegin(), increment_books.cend(),
                [this, &t](const fh::cme::market::message::Book &b)
                {
                    auto changed_state_or_trade = m_book_state_controller.Modify_state(b);
                    BookManager::Send(m_book_sender, b, changed_state_or_trade, m_definition_manager.Get_symbol(b.securityID));
                    LOG_INFO("send to zmq: ", t.Elapsed_nanoseconds(), "ns");
                }
        );
    }

    void BookManager::Send(
        fh::core::market::MarketListenerI *sender,
        const fh::cme::market::message::Book &org_book,
        std::pair<std::uint8_t, const void *> changed_state,
        const std::string &contract)
    {
        std::uint8_t flag = changed_state.first;   // 0: no data  changed 1: reset  2: book state changed  3: trade info
        const void *data = changed_state.second;
        if(flag == 1)
        {
            // TODO reset 后要不要通知策略端
        }
        else if(flag == 2 && data != nullptr)
        {
            // 发送行情数据
            const fh::cme::market::BookState * bs = static_cast<const fh::cme::market::BookState *>(data);
            BookManager::Send_l2(sender, bs);
            if(BookManager::Is_BBO_changed(org_book))
            {
                BookManager::Send_bbo(sender, bs);
            }
        }
        else if(flag == 3 && data != nullptr)
        {
            // 发送 trade 数据
            const fh::cme::market::message::Book *trade = static_cast<const fh::cme::market::message::Book *>(data);
            BookManager::Send_trade(sender, trade, contract);
        }
    }

    void BookManager::Send_l2(fh::core::market::MarketListenerI *sender, const fh::cme::market::BookState *state)
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

        sender->OnL2(l2_info);
    }

    void BookManager::Send_bbo(fh::core::market::MarketListenerI *sender, const fh::cme::market::BookState *state)
    {
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

            sender->OnBBO(bbo_info);
        }
        else if(is_bid_empty)
        {
            // 只有 ask
            pb::dms::Offer offer_info;
            offer_info.set_contract(state->symbol);
            pb::dms::DataPoint *offer = offer_info.mutable_offer();
            offer->set_price(fh::cme::market::message::utility::Get_price(state->ask.front().mDEntryPx));
            offer->set_size(state->ask.front().mDEntrySize);

            sender->OnOffer(offer_info);
        }
        else
        {
            // 只有 bid
            pb::dms::Bid bid_info;
            bid_info.set_contract(state->symbol);
            pb::dms::DataPoint *bid = bid_info.mutable_bid();
            bid->set_price(fh::cme::market::message::utility::Get_price(state->bid.front().mDEntryPx));
            bid->set_size(state->bid.front().mDEntrySize);

            sender->OnBid(bid_info);
        }
    }

    void BookManager::Send_trade(fh::core::market::MarketListenerI *sender, const fh::cme::market::message::Book *trade_book, const std::string &contract)
    {
        // 发送 trade 数据 TODO 这里要根据 mDUpdateAction 区分下不同的动作吧
        pb::dms::Trade trade;
        trade.set_contract(contract);
        pb::dms::DataPoint *last = trade.mutable_last();
        last->set_price(fh::cme::market::message::utility::Get_price(trade_book->mDEntryPx));
        last->set_size(trade_book->mDEntrySize);

        sender->OnTrade(trade);
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
        m_recovery_manager.On_new_recovery(messages, [this](const fh::cme::market::BookState *bs){
            BookManager::Send_l2(m_book_sender, bs);
            BookManager::Send_bbo(m_book_sender, bs);
        });
    }

    std::vector<fh::cme::market::message::Book> BookManager::Parse_increment(const fh::cme::market::message::MdpMessage &message)
    {
        std::vector<fh::cme::market::message::Book> books;

        char type = message.message_type();
        if(type == 'f')
        {
            m_status_manager.On_new_status(message);
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

    void BookManager::On_definition_changed(const fh::cme::market::message::Instrument &instrument)
    {
        // 根据更新的产品信息修正对应的 book state 情报
        m_book_state_controller.Create_or_shrink(instrument);
    }

    // 指定的 book 情报会不会引起 BBO 的变化
    bool BookManager::Is_BBO_changed(const fh::cme::market::message::Book &b)
    {
        // 恢复数据中的 book 当作 new 来处理的
        if(b.type == 'W') return b.mDPriceLevel == 1;
        if(b.type == 'X')
        {
            switch(b.mDUpdateAction)
            {
                case mktdata::MDUpdateAction::Value::New:
                case mktdata::MDUpdateAction::Value::Change:
                case mktdata::MDUpdateAction::Value::Delete: return b.mDPriceLevel == 1;
                case mktdata::MDUpdateAction::Value::DeleteThru:
                case mktdata::MDUpdateAction::Value::DeleteFrom: return true;
                default: return false;
            }
        }

        return false;
    }

} // namespace market
} // namespace cme
} // namespace fh
