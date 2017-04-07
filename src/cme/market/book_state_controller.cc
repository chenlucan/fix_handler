#include "cme/market/book_state_controller.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{

    BookStateController::BookStateController() : m_md_book_states()
    {
        // noop
    }

    BookStateController::~BookStateController()
    {
        // noop
    }

    void BookStateController::Create_or_shrink(const fh::cme::market::message::Instrument &instrument)
    {
        auto instrument_depth = instrument.depthGBX;
        auto security_id = instrument.securityID;

        if(instrument.securityUpdateAction == 'D')
        {
            // 删除的场合
            m_md_book_states.erase(security_id);
            return;
        }

        auto state_pos = m_md_book_states.find(security_id);
        if(state_pos == m_md_book_states.end())
        {
            // 该产品的 book state 还不存在，需要创建
            BookState bs = BookStateController::Create(1, security_id, instrument.symbol, instrument_depth);    // 1: Multiple Depth Book
            m_md_book_states.insert({security_id, bs});
        }
        else
        {
            // 该产品的 book state 已经存在，需要根据新深度进行调整（变浅的场合需要删除溢出的）
            BookStateController::Shrink(state_pos->second, instrument_depth);
        }
    }

    // 将新的 book 情报更新到保存的 book state
    // 如果是 BookReset 消息，则清除所有的 book state 信息，返回 {1, null}
    // 如果找不到该 book 对应的定义情报，返回  {0, null}
    // 如果是 Trade 消息，返回 {3, 传入的 book}
    // 如果是 Bid 或者 Offer 消息，更新其对应的 book state 信息，返回 {2, 更新后 book state}
    // 否则返回  {0, null}
    std::pair<std::uint8_t, const void *> BookStateController::Modify_state(const fh::cme::market::message::Book &b)
    {
        LOG_DEBUG("modify book state by book: type=", b.type, " security_id=", b.securityID, " mDEntryType=", (char)b.mDEntryType);

        if(b.mDEntryType == mktdata::MDEntryType::Value::BookReset)
        {
            // 重置所有的 book state 情报
            this->Reset_state();
            return std::make_pair(1, nullptr);
        }

        auto pos = m_md_book_states.find(b.securityID);
        if(pos == m_md_book_states.end())
        {
            // 这个产品没有定义情报，扔掉该 book
            LOG_WARN("no difinition found: security id=", b.securityID, "; discard");
            return std::make_pair(0, nullptr);
        }

        switch(b.mDEntryType)
        {
            case mktdata::MDEntryType::Value::Bid:
            case mktdata::MDEntryType::Value::Offer:
            {
                auto book_state = this->Update_price(b, pos->second) ? &pos->second : nullptr;
                return std::make_pair(2, book_state);
            }
            case mktdata::MDEntryType::Value::Trade:
            {
                auto trade_book = &b;
                return std::make_pair(3, trade_book);
            }
            case mktdata::MDEntryType::Value::ImpliedBid:
            case mktdata::MDEntryType::Value::ImpliedOffer:
            default:
            {
                LOG_INFO("ignore mDEntryType: ", (char)b.mDEntryType);
                return std::make_pair(0, nullptr);
            }
        }
    }

    BookState BookStateController::Create(std::uint8_t book_type, std::uint32_t security_id, std::string symbol, std::uint8_t market_depth)
    {
        BookState bs;
        bs.bookType = book_type;
        bs.securityID = security_id;
        bs.symbol = symbol;
        bs.marketDepth = market_depth;

        LOG_DEBUG("create new book state: type=", (std::uint32_t)book_type, " security_id=", security_id, " symbol=", symbol, " market_depth=", (int)market_depth);

        return bs;
    }

    void BookStateController::Shrink(BookState &book_state, std::uint8_t market_new_depth)
    {
        book_state.marketDepth = market_new_depth;
        if(book_state.bid.size() > market_new_depth)
        {
            book_state.bid.resize(market_new_depth);
        }
        if(book_state.ask.size() > market_new_depth)
        {
            book_state.ask.resize(market_new_depth);
        }

        LOG_DEBUG("reset book state depth: security_id=", book_state.securityID, " market_depth=", (int)book_state.marketDepth);
    }

    void BookStateController::Reset_state()
    {
        // 收到 BookReset 后需要清除当前所有的 bid 和 ask 信息
        LOG_INFO("reset book state: clear bid and ask");
        std::for_each(m_md_book_states.begin(), m_md_book_states.end(), [](std::pair<const std::uint32_t , fh::cme::market::BookState> &s){
            s.second.bid.clear();
            s.second.ask.clear();
        });
    }

    bool BookStateController::Update_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        if(b.type == 'W')
        {
            // recovery book - 35=W, 全部当作 MDUpdateAction::New 来处理
            return this->New_price(b, book_state);
        }
        else if(b.type == 'X')
        {
            // increment book - 35=X
            switch(b.mDUpdateAction)
            {
                case mktdata::MDUpdateAction::Value::New:
                    return this->New_price(b, book_state);
                case mktdata::MDUpdateAction::Value::Change:
                    return this->Change_price(b, book_state);
                case mktdata::MDUpdateAction::Value::Delete:
                    return this->Delete_price(b, book_state);
                case mktdata::MDUpdateAction::Value::DeleteThru:
                    return this->Delete_all_price(b, book_state);
                case mktdata::MDUpdateAction::Value::DeleteFrom:
                    return this->Delete_top_price(b, book_state);
                default:
                    LOG_WARN("invalid mDUpdateAction: ", b.mDUpdateAction, "; ignore");
                    return false;
            }
        }
        else
        {
            // increment book - 其他类型无需更新
            return false;
        }
    }

    bool BookStateController::New_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("create price: level=", (int)b.mDPriceLevel, ", securityID=", b.securityID, ", type=", (char)b.mDEntryType);

        if(b.mDPriceLevel == 0 || b.mDPriceLevel > book_state.marketDepth)
        {
            LOG_WARN("price level invalid, ignore:", (int)b.mDPriceLevel);
            return false;
        }

        std::uint8_t depth = book_state.marketDepth;
        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        BookPrice new_price = {b.numberOfOrders, b.mDEntrySize, b.mDEntryPx};

        std::size_t current_size = target.size();
        std::size_t new_size;
        if(b.mDPriceLevel <= current_size + 1)
        {
            // 要插入的位置之前都有值的话，直接插入
            target.insert(target.begin() + b.mDPriceLevel - 1, new_price);
            new_size = current_size + 1;
        }
        else
        {
            // 否则在最后一行和要插入的位置之间要填充空行
            target.resize(b.mDPriceLevel - 1, {0, 0, 0});
            target.insert(target.end(), new_price);
            new_size = b.mDPriceLevel;
        }

        // 最后，如果有超出该产品的最大深度定义的行删除
        if(new_size > (std::size_t)depth)
        {
            target.erase(target.begin() + depth, target.end());
        }

        LOG_INFO("new price: order-number=", b.numberOfOrders, ", count=", b.mDEntrySize, ", price=", b.mDEntryPx);
        return true;
    }

    bool BookStateController::Change_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("change price: level=", (int)b.mDPriceLevel, ", securityID=", b.securityID, ", type=", (char)b.mDEntryType);

        if(b.mDPriceLevel == 0 || b.mDPriceLevel > book_state.marketDepth)
        {
            LOG_WARN("price level invalid, ignore:", (int)b.mDPriceLevel);
            return false;
        }

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        if(target.size() < (std::size_t)b.mDPriceLevel)
        {
            LOG_WARN("price level not exist, ignore");
            return false;
        }

        BookPrice &price = target.at(b.mDPriceLevel - 1);
        price.numberOfOrders = b.numberOfOrders;
        price.mDEntrySize = b.mDEntrySize;
        price.mDEntryPx = b.mDEntryPx;

        LOG_INFO("new price: order-number=", b.numberOfOrders, ", count=", b.mDEntrySize, ", price=", b.mDEntryPx);
        return true;
    }

    bool BookStateController::Delete_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete price: level=", (int)b.mDPriceLevel, ", securityID=", b.securityID, ", type=", (char)b.mDEntryType);

        if(b.mDPriceLevel == 0 || b.mDPriceLevel > book_state.marketDepth)
        {
            LOG_WARN("price level invalid, ignore:", (int)b.mDPriceLevel);
            return false;
        }

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        if(target.size() < (std::size_t)b.mDPriceLevel)
        {
            LOG_WARN("price level not exist, ignore");
            return false;
        }

        target.erase(target.begin() + b.mDPriceLevel - 1);

        LOG_INFO("delete ok, new size=", target.size());
        return true;
    }

    bool BookStateController::Delete_all_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete all price: securityID=", b.securityID, ", type=", (char)b.mDEntryType);

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        target.clear();

        LOG_INFO("delete ok");
        return true;
    }

    bool BookStateController::Delete_top_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete top price: level=", (int)b.mDPriceLevel, ", securityID=", b.securityID, ", type=", (char)b.mDEntryType);

        if(b.mDPriceLevel == 0 || b.mDPriceLevel > book_state.marketDepth)
        {
            LOG_WARN("price level invalid, ignore:", (int)b.mDPriceLevel);
            return false;
        }

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        target.erase(target.begin(), target.begin() + std::min((std::size_t)b.mDPriceLevel, target.size()));

        LOG_INFO("delete ok");
        return true;
    }

} // namespace market
} // namespace cme
} // namespace fh
