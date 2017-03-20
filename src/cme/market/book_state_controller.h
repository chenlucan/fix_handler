#ifndef __FH_CME_MARKET_BOOK_STATE_CONTROLLER_H__
#define __FH_CME_MARKET_BOOK_STATE_CONTROLLER_H__

#include <unordered_map>
#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/book_state.h"
#include "cme/market/message/instrument.h"

namespace fh
{
namespace cme
{
namespace market
{
    class BookStateController
    {
        public:
            BookStateController();
            virtual ~BookStateController();

        public:
            void Create_or_shrink(const fh::cme::market::message::Instrument &instrument);
            std::pair<std::uint8_t, const void *> Modify_state(const fh::cme::market::message::Book &b);

        private:
            static BookState Create(std::uint8_t book_type, std::uint32_t security_id, std::string symbol, std::uint8_t market_depth);
            static void Shrink(BookState &book_state, std::uint8_t market_new_depth);
            void Reset_state();
            bool Update_price(const fh::cme::market::message::Book &b, BookState &book_state);
            bool New_price(const fh::cme::market::message::Book &b, BookState &book_state);
            bool Change_price(const fh::cme::market::message::Book &b, BookState &book_state);
            bool Delete_price(const fh::cme::market::message::Book &b, BookState &book_state);
            bool Delete_all_price(const fh::cme::market::message::Book &b, BookState &book_state);
            bool Delete_top_price(const fh::cme::market::message::Book &b, BookState &book_state);

        private:
            // 保存每个 SecurityID 对应的  multiple depth book state 情报
            std::unordered_map<std::uint32_t , fh::cme::market::BookState> m_md_book_states;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookStateController);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_STATE_CONTROLLER_H__
