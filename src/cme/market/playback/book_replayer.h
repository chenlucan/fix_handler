
#ifndef __FH_CME_MARKET_PLAYBACK_BOOK_REPLAYER_H__
#define __FH_CME_MARKET_PLAYBACK_BOOK_REPLAYER_H__

#include <vector>
#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include "core/global.h"
#include "cme/market/book_state_controller.h"


#define JSON_ELEMENT bsoncxx::document::element


namespace fh
{
namespace cme
{
namespace market
{
namespace playback
{
    class BookReplayer
    {
        public:
            BookReplayer();
            virtual ~BookReplayer();

        public:
            void Apply_message(const std::string &message);
            std::unordered_map<std::uint32_t , fh::cme::market::BookState> &Get_all_states();

        private:
            void Parse_definitions(const JSON_ELEMENT &message);
            void Parse_recoveries(const JSON_ELEMENT &message);
            void Parse_increments(std::uint32_t seq, const JSON_ELEMENT &message);

        private:
            // 管理 book state
            fh::cme::market::BookStateController m_book_state_controller;
            // 保存 SecurityID 对应的 definition 情报
            std::unordered_map<std::uint32_t , fh::cme::market::message::Instrument> m_instruments;
            // 保存恢复数据
            std::vector<fh::cme::market::message::Book> m_recoveries;
            // 是否第一条 recovery 数据（收到第一条后需要重置内部数据）
            bool m_is_first_recovery;

        private:
            DISALLOW_COPY_AND_ASSIGN(BookReplayer);
    };
} // namespace playback
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_PLAYBACK_BOOK_REPLAYER_H__
