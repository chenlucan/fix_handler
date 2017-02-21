#ifndef __BOOK_STATE_CONTROLLER_H__
#define __BOOK_STATE_CONTROLLER_H__

#include "global.h"
#include "book.h"
#include "book_state.h"
#include "instrument.h"

namespace rczg
{
	class BookStateController
	{
		public:
			BookStateController();
			virtual ~BookStateController();

		public:
			void Create_or_shrink(const rczg::Instrument &instrument);
			bool Modify_state(const Book &b);
			bool Has_instrument();
			std::string Get();

		private:
			static BookState Create(std::uint8_t book_type, std::uint32_t security_id, std::uint8_t market_depth);
			static void Shrink(BookState &book_state, std::uint8_t market_new_depth);
			bool Reset_state(const Book &b, BookState &book_state);
			bool Update_price(const Book &b, BookState &book_state);
			bool New_price(const Book &b, BookState &book_state);
			bool Change_price(const Book &b, BookState &book_state);
			bool Delete_price(const Book &b, BookState &book_state);
			bool Delete_all_price(const Book &b, BookState &book_state);
			bool Delete_top_price(const Book &b, BookState &book_state);

		private:
            // 保存每个 SecurityID 对应的  multiple depth book state 情报
            std::unordered_map<std::uint32_t , rczg::BookState> m_md_book_states;

		private:
			DISALLOW_COPY_AND_ASSIGN(BookStateController);
	};
}

#endif // __BOOK_STATE_CONTROLLER_H__
