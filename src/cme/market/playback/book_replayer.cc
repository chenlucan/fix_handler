
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include "cme/market/playback/book_replayer.h"
#include "core/assist/logger.h"
#include "core/market/marketlisteneri.h"
#include "cme/market/book_manager.h"
#include "cme/market/definition_manager.h"
#include "cme/market/status_manager.h"

#define  GET_SUB_FROM_JSON(view, key) view[key]
#define  GET_STR_FROM_JSON(view, key) view[key].get_utf8().value.to_string()
#define  GET_ARR_FROM_JSON(view, key) view[key].get_array().value
#define  GET_CHAR_FROM_JSON(view, key) GET_STR_FROM_JSON(view, key).at(0)
#define  GET_INT_FROM_JSON(view, key) std::stol(GET_STR_FROM_JSON(view, key))
#define  GET_INT_OR_DEFAULT_FROM_JSON(view, key, def) GET_SUB_FROM_JSON(view, key) ? GET_INT_FROM_JSON(view, key) : def
#define  GET_OPTIONAL_PRICE_FROM_JSON(target, view, key) { auto v = GET_SUB_FROM_JSON(view, #key); \
                                                                                                                    if(v) target.key = {GET_INT_FROM_JSON (v, "mantissa"), GET_INT_FROM_JSON (v, "exponent")}; }


namespace fh
{
namespace cme
{
namespace market
{
namespace playback
{

    BookReplayer::BookReplayer()
    : m_listener(nullptr), m_book_state_controller(), m_instruments(),
      m_recoveries(), m_is_first_recovery(true)
    {
        // noop
    }

    void BookReplayer::Add_listener(fh::core::market::MarketListenerI *listener)
    {
        m_listener = listener;
    }

    void BookReplayer::Apply_message(const std::string &message)
    {
        auto doc = bsoncxx::from_json(message);
        auto json = doc.view();
        auto sbe_type = GET_STR_FROM_JSON(json, "sbeType");
        auto body = GET_SUB_FROM_JSON(json, "message");
        auto seq = GET_INT_FROM_JSON(json, "packetSeqNum");
        auto is = GET_STR_FROM_JSON(json, "insertTime");

        LOG_INFO("apply message: ", is, " type=", sbe_type, " seq=", seq);

        if(sbe_type == "d")   Parse_definitions(body);
        else if(sbe_type == "W")   Parse_recoveries(body);
        else if(sbe_type == "X")   Parse_increments(seq, body);
        else if(sbe_type == "f")   Parse_status(body);
        else LOG_WARN("ignore sbe type:", sbe_type);
    }

    void BookReplayer::Parse_definitions(const JSON_ELEMENT &message)
    {
        fh::cme::market::message::Instrument ins{};

        ins.securityUpdateAction = GET_CHAR_FROM_JSON(message, "securityUpdateAction");   // A: add  D: delete  M:modify
        ins.securityID = GET_INT_FROM_JSON(message, "securityID");
        ins.symbol = GET_STR_FROM_JSON(message, "Symbol");
        GET_OPTIONAL_PRICE_FROM_JSON(ins, message, minPriceIncrement);
        GET_OPTIONAL_PRICE_FROM_JSON(ins, message, highLimitPrice);
        GET_OPTIONAL_PRICE_FROM_JSON(ins, message, lowLimitPrice);
        for(auto &mft : GET_ARR_FROM_JSON(message, "noMDFeedTypes"))
        {
            auto type = GET_STR_FROM_JSON(mft, "MDFeedType");
            auto depth = GET_INT_FROM_JSON(mft, "marketDepth");

            if(type == "GBI")   ins.depthGBI = depth;
            if(type == "GBX")   ins.depthGBX = depth;
        }

        if(ins.securityUpdateAction == 'D')
        {
            // delete
            std::size_t count = m_instruments.erase(ins.securityID);
            if(count == 0)
            {
                // 原来就没有对应的信息
                LOG_WARN("can not find origin instrument to delete: ", ins.securityID);
                return;
            }
        }
        else
        {
            // modify or add
            m_instruments[ins.securityID] = ins;
        }

        // 更新 book state 信息
        m_book_state_controller.Create_or_shrink(ins);
        // 发送出去
        if(m_listener) fh::cme::market::DefinitionManager::Send(m_listener, ins);
    }

    void BookReplayer::Parse_recoveries(const JSON_ELEMENT &message)
    {
        if(m_is_first_recovery)
        {
            // 收到第一条 recovery 数据后需要重置所有的 book state
            m_book_state_controller.Reset_state();
            m_recoveries.clear();
            m_is_first_recovery = false;
        }

        int security_id = GET_INT_FROM_JSON(message, "securityID");
        std::uint32_t last_seq = GET_INT_FROM_JSON(message, "lastMsgSeqNumProcessed");
        for(auto &e : GET_ARR_FROM_JSON(message, "noMDEntries"))
        {
            fh::cme::market::message::Book b{};
            b.type = 'W';
            b.securityID = security_id;
            b.packet_seq_num = last_seq;
            GET_OPTIONAL_PRICE_FROM_JSON(b, e, mDEntryPx);
            b.mDEntryType = mktdata::MDEntryType::get(GET_CHAR_FROM_JSON(e, "mDEntryType"));
            b.mDEntrySize = GET_INT_OR_DEFAULT_FROM_JSON (e, "mDEntrySize", 0);
            b.numberOfOrders = GET_INT_OR_DEFAULT_FROM_JSON (e, "numberOfOrders", 0);
            b.mDPriceLevel = GET_INT_OR_DEFAULT_FROM_JSON (e, "mDPriceLevel", 0);

            m_recoveries.push_back(b);

            auto changed_state = m_book_state_controller.Modify_state(b);
            if(m_listener) fh::cme::market::BookManager::Send(m_listener, b, changed_state, this->Contract(b.securityID));
        }
    }

    void BookReplayer::Parse_increments(std::uint32_t seq, const JSON_ELEMENT &message)
    {
        std::uint32_t first_recovery_seq = 0;
        std::uint32_t last_recovery_seq = 0;

        // TODO 万一有 MDIncrementalRefreshOrderBook43 类型是否需要忽略？

        if(!m_recoveries.empty())
        {
            first_recovery_seq = m_recoveries.front().packet_seq_num;
            last_recovery_seq = m_recoveries.back().packet_seq_num;
        }

        // 之后再收到 recovery 数据就要重置内部数据了
        m_is_first_recovery = true;

        for(auto &e : GET_ARR_FROM_JSON(message, "noMDEntries"))
        {
            fh::cme::market::message::Book b{};
            b.securityID = GET_INT_OR_DEFAULT_FROM_JSON(e, "securityID", 0);
            b.packet_seq_num = seq;
            b.type = 'X';
            GET_OPTIONAL_PRICE_FROM_JSON(b, e, mDEntryPx);
            b.mDEntryType = mktdata::MDEntryType::get(GET_CHAR_FROM_JSON(e, "mDEntryType"));
            b.mDEntrySize = GET_INT_OR_DEFAULT_FROM_JSON (e, "mDEntrySize", 0);
            b.numberOfOrders = GET_INT_OR_DEFAULT_FROM_JSON (e, "numberOfOrders", 0);
            b.mDPriceLevel = GET_INT_OR_DEFAULT_FROM_JSON (e, "mDPriceLevel", 0);
            b.mDUpdateAction = mktdata::MDUpdateAction::get(GET_INT_FROM_JSON(e, "mDUpdateAction"));

            // 看看本次的 book 信息是否已经在 recovery 数据中存在了，存在的话要丢弃
            if(seq >= first_recovery_seq && seq <= last_recovery_seq)
            {
                auto index = std::find_if(m_recoveries.cbegin(), m_recoveries.cend(), [&b](const fh::cme::market::message::Book &rb){
                    return b.packet_seq_num <= rb.packet_seq_num && b.securityID == rb.securityID;
                });

                if(index != m_recoveries.cend())
                {
                    LOG_WARN("discard security: ", b.securityID);
                    continue;
                }
            }

            auto changed_state = m_book_state_controller.Modify_state(b);
            if(m_listener) fh::cme::market::BookManager::Send(m_listener, b, changed_state, this->Contract(b.securityID));
        }
    }

    void BookReplayer::Parse_status(const JSON_ELEMENT &message)
    {
        std::uint32_t security_id = GET_INT_OR_DEFAULT_FROM_JSON (message, "securityID", 0);
        mktdata::SecurityTradingStatus::Value securityTradingStatus = mktdata::SecurityTradingStatus::get(GET_INT_OR_DEFAULT_FROM_JSON (message, "securityTradingStatus", 255));

        std::string contract = "";
        if(security_id == 0)
        {
            // 如果没设置 security id
            contract = "";
        }
        else
        {
            contract = this->Contract(security_id);
            // 如果找不到名称，就使用 id
            if(contract == "") contract = std::to_string(security_id);
        }

        LOG_INFO("status changed: ", securityTradingStatus);
        if(m_listener) fh::cme::market::StatusManager::Send(m_listener, contract, securityTradingStatus);
    }

    std::string BookReplayer::Contract(int security_id)
    {
        // 根据 id 在回放时整理好的合约列表中找到合约名称，找不到的场合返回 ""
        auto pos = m_instruments.find(security_id);
        if(pos == m_instruments.end()) return "";
        return pos->second.symbol;
    }

    BookReplayer::~BookReplayer()
    {
        // noop
    }

} // namespace playback
} // namespace market
} // namespace cme
} // namespace fh
