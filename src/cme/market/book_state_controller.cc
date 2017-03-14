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

        auto state_pos = m_md_book_states.find(security_id);
        if(state_pos == m_md_book_states.end())
        {
            // 该产品的 book state 还不存在，需要创建
            BookState bs = BookStateController::Create(1, security_id, instrument_depth);    // 1: Multiple Depth Book
            m_md_book_states.insert({security_id, bs});
        }
        else
        {
            // 该产品的 book state 已经存在，需要根据新深度进行调整（变浅的场合需要删除溢出的）
            BookStateController::Shrink(state_pos->second, instrument_depth);
        }
    }

    bool BookStateController::Modify_state(const fh::cme::market::message::Book &b)
    {
        LOG_DEBUG("modify book state by: book_type=", b.type, " security_id=", b.securityID, " mDEntryType=", b.mDEntryType);

        auto pos = m_md_book_states.find(b.securityID);
        if(pos == m_md_book_states.end())
        {
            // 这个产品没有定义情报，扔掉该 book
            LOG_WARN("no difinition found: security id=", b.securityID, "; discard");
            return false;
        }

        switch(b.mDEntryType)
        {
            case mktdata::MDEntryType::Value::BookReset:
                return this->Reset_state(b, pos->second);
            case mktdata::MDEntryType::Value::Bid:
            case mktdata::MDEntryType::Value::Offer:
                return this->Update_price(b, pos->second);
            case mktdata::MDEntryType::Value::ImpliedBid:
            case mktdata::MDEntryType::Value::ImpliedOffer:
            default:
                LOG_INFO("ignore mDEntryType: ", b.mDEntryType);
                return false;
        }
    }

    bool BookStateController::Has_instrument()
    {
        return !m_md_book_states.empty();
    }

    std::string BookStateController::Get()
    {
        std::ostringstream os;
        std::for_each(m_md_book_states.cbegin(), m_md_book_states.cend(), [&os](const std::pair<std::uint32_t , fh::cme::market::BookState> &bs)
        {
            os << bs.second.Serialize();
        });
        return os.str();
    }

    BookState BookStateController::Create(std::uint8_t book_type, std::uint32_t security_id, std::uint8_t market_depth)
    {
        BookState bs;
        bs.bookType = book_type;
        bs.securityID = security_id;
        bs.marketDepth = market_depth;

        LOG_DEBUG("create new book state: book_type=", book_type, " security_id=", security_id, " market_depth=", (int)market_depth);

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

    bool BookStateController::Reset_state(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        // TODO
        LOG_INFO("reset book state, TODO");
        return false;
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
            // increment book - other
            // TODO 35=f, 35=R
            return false;
        }
    }

    bool BookStateController::New_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("create price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType);

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

        LOG_INFO("new price: ", b.numberOfOrders, "-", b.mDEntrySize, "-", b.mDEntryPx);
        return true;
    }

    bool BookStateController::Change_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("change price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType);

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        if(target.size() < (std::size_t)b.mDPriceLevel)
        {
            LOG_WARN("change not exist price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType, "; ignore");
            return false;
        }

        BookPrice &price = target.at(b.mDPriceLevel - 1);
        price.numberOfOrders = b.numberOfOrders;
        price.mDEntrySize = b.mDEntrySize;
        price.mDEntryPx = b.mDEntryPx;

        LOG_INFO("new price: ", b.numberOfOrders, "-", b.mDEntrySize, "-", b.mDEntryPx);
        return true;
    }

    bool BookStateController::Delete_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType);

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        if(target.size() < (std::size_t)b.mDPriceLevel)
        {
            LOG_WARN("delete not exist price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType, "; ignore");
            return false;
        }

        target.erase(target.begin() + b.mDPriceLevel - 1);

        LOG_INFO("delete ok, new size=", target.size());
        return true;
    }

    bool BookStateController::Delete_all_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete all price level of ", b.securityID, "-", b.mDEntryType);

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        target.clear();

        LOG_INFO("delete ok, new size=", target.size());
        return true;
    }

    bool BookStateController::Delete_top_price(const fh::cme::market::message::Book &b, BookState &book_state)
    {
        LOG_INFO("delete top price level ", (int)b.mDPriceLevel, " of ", b.securityID, "-", b.mDEntryType);

        std::deque<BookPrice> &target = (b.mDEntryType == mktdata::MDEntryType::Value::Bid ? book_state.bid : book_state.ask);
        target.erase(target.begin(), target.begin() + std::min((std::size_t)b.mDPriceLevel, target.size()));

        LOG_INFO("delete ok, new size=", target.size());
        return true;
    }

} // namespace market
} // namespace cme
} // namespace fh
