#ifndef __FH_CME_MARKET_MESSAGE_BOOK_H__
#define __FH_CME_MARKET_MESSAGE_BOOK_H__

#include <sstream>
#include "core/global.h"
#include "cme/market/message/mktdata.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    // 保存每次行情数据的变动情况，直接从 increment message 解析得到
    struct Book
    {
        std::uint32_t packet_seq_num;        // recovery books 的场合是 LastMsgSeqNumProcessed
        std::uint16_t template_id;
        std::uint8_t type;        // X  W

        struct MatchEventIndicator
        {
            bool isLastTradeMsg;
            bool isLastVolumeMsg;
            bool isLastQuoteMsg;
            bool isLastStatsMsg;
            bool isLastImpliedMsg;
            bool isRecoveryMsg;
            bool isReserved;
            bool isEndOfEvent;
        } matchEventIndicator;

        mktdata::AggressorSide::Value aggressorSide;
        std::uint16_t applID;
        std::int64_t highLimitPrice;    // exponent is -7
        std::int64_t lowLimitPrice;    // exponent is -7
        std::uint32_t mDDisplayQty;
        std::int64_t mDEntryPx;    // tag 270, exponent is -7
        std::uint32_t mDEntrySize;    // tag 271
        mktdata::MDEntryType::Value mDEntryType;    // tag 269
        std::uint64_t mDOrderPriority;
        std::uint8_t mDPriceLevel;    // tag 1023
        std::uint32_t mDTradeEntryID;
        mktdata::MDUpdateAction::Value mDUpdateAction;    // tag 279
        std::int64_t maxPriceVariation;    // exponent is -7
        std::uint32_t numberOfOrders;    // tag 346
        mktdata::OpenCloseSettlFlag::Value openCloseSettlFlag;
        std::uint64_t orderID;
        std::uint32_t rptSeq;
        std::uint32_t securityID;    // tag 48

        struct SettlPriceType
        {
            bool isFinal;
            bool isActual;
            bool isRounded;
            bool isIntraday;
            bool isReservedBits;
            bool isNullValue;
        } settlPriceType;

        std::uint32_t tradeID;
        std::uint16_t tradingReferenceDate;

        std::string To_string() const
        {
            std::ostringstream os;
            os << "book: ";
            os << "packet_seq_num=" << packet_seq_num << " ";
            os << "template_id=" << template_id << " ";
            os << "type=" << type << " ";
            os << "securityID=" << securityID << " ";

            return os.str();
        }
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_BOOK_H__
