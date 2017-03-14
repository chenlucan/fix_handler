/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _MKTDATA_MDINCREMENTALREFRESHBOOK32_H_
#define _MKTDATA_MDINCREMENTALREFRESHBOOK32_H_

#if defined(SBE_HAVE_CMATH)
/* cmath needed for std::numeric_limits<double>::quiet_NaN() */
#  include <cmath>
#  define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#  define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#else
/* math.h needed for NAN */
#  include <math.h>
#  define SBE_FLOAT_NAN NAN
#  define SBE_DOUBLE_NAN NAN
#endif

#if __cplusplus >= 201103L
#  include <cstdint>
#  include <string>
#  include <cstring>
#endif

#if __cplusplus >= 201103L
#  define SBE_CONSTEXPR constexpr
#else
#  define SBE_CONSTEXPR
#endif

#include <sbe/sbe.h>

#include "MDEntryTypeBook.h"
#include "OpenCloseSettlFlag.h"
#include "MatchEventIndicator.h"
#include "MaturityMonthYear.h"
#include "FLOAT.h"
#include "MDEntryTypeDailyStatistics.h"
#include "EventType.h"
#include "DecimalQty.h"
#include "MDUpdateAction.h"
#include "GroupSize8Byte.h"
#include "HaltReason.h"
#include "MDEntryType.h"
#include "PRICENULL.h"
#include "SecurityTradingStatus.h"
#include "LegSide.h"
#include "OrderUpdateAction.h"
#include "PRICE.h"
#include "PutOrCall.h"
#include "SecurityUpdateAction.h"
#include "SecurityTradingEvent.h"
#include "MDEntryTypeStatistics.h"
#include "InstAttribValue.h"
#include "AggressorSide.h"
#include "GroupSize.h"
#include "SettlPriceType.h"

using namespace sbe;

namespace mktdata {

class MDIncrementalRefreshBook32
{
private:
    char *m_buffer;
    std::uint64_t m_bufferLength;
    std::uint64_t *m_positionPtr;
    std::uint64_t m_offset;
    std::uint64_t m_position;
    std::uint64_t m_actingBlockLength;
    std::uint64_t m_actingVersion;

    inline void reset(
        char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength, const std::uint64_t actingVersion)
    {
        m_buffer = buffer;
        m_offset = offset;
        m_bufferLength = bufferLength;
        m_actingBlockLength = actingBlockLength;
        m_actingVersion = actingVersion;
        m_positionPtr = &m_position;
        position(offset + m_actingBlockLength);
    }

public:

    MDIncrementalRefreshBook32(void) : m_buffer(nullptr), m_bufferLength(0), m_offset(0) {}

    MDIncrementalRefreshBook32(char *buffer, const std::uint64_t bufferLength)
    {
        reset(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion());
    }

    MDIncrementalRefreshBook32(char *buffer, const std::uint64_t bufferLength, const std::uint64_t actingBlockLength, const std::uint64_t actingVersion)
    {
        reset(buffer, 0, bufferLength, actingBlockLength, actingVersion);
    }

    static SBE_CONSTEXPR const std::uint16_t sbeBlockLength(void)
    {
        return (std::uint16_t)11;
    }

    static SBE_CONSTEXPR const std::uint16_t sbeTemplateId(void)
    {
        return (std::uint16_t)32;
    }

    static SBE_CONSTEXPR const std::uint16_t sbeSchemaId(void)
    {
        return (std::uint16_t)1;
    }

    static SBE_CONSTEXPR const std::uint16_t sbeSchemaVersion(void)
    {
        return (std::uint16_t)8;
    }

    static SBE_CONSTEXPR const char * sbeSemanticType(void)
    {
        return "X";
    }

    std::uint64_t offset(void) const
    {
        return m_offset;
    }

    MDIncrementalRefreshBook32 &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        reset(buffer, offset, bufferLength, sbeBlockLength(), sbeSchemaVersion());
        return *this;
    }

    MDIncrementalRefreshBook32 &wrapForDecode(
         char *buffer, const std::uint64_t offset, const std::uint64_t actingBlockLength,
         const std::uint64_t actingVersion, const std::uint64_t bufferLength)
    {
        reset(buffer, offset, bufferLength, actingBlockLength, actingVersion);
        return *this;
    }

    std::uint64_t position(void) const
    {
        return m_position;
    }

    void position(const std::uint64_t position)
    {
        if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
        {
            throw std::runtime_error("buffer too short [E100]");
        }
        m_position = position;
    }

    std::uint64_t encodedLength(void) const
    {
        return position() - m_offset;
    }

    char *buffer(void)
    {
        return m_buffer;
    }

    std::uint64_t actingVersion(void) const
    {
        return m_actingVersion;
    }

    static SBE_CONSTEXPR const std::uint16_t transactTimeId(void)
    {
        return 60;
    }

    static SBE_CONSTEXPR const std::uint64_t transactTimeSinceVersion(void)
    {
         return 0;
    }

    bool transactTimeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= transactTimeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *TransactTimeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint64_t transactTimeNullValue()
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR const std::uint64_t transactTimeMinValue()
    {
        return 0x0L;
    }

    static SBE_CONSTEXPR const std::uint64_t transactTimeMaxValue()
    {
        return 0xfffffffffffffffeL;
    }

    std::uint64_t transactTime(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 0)));
    }

    MDIncrementalRefreshBook32 &transactTime(const std::uint64_t value)
    {
        *((std::uint64_t *)(m_buffer + m_offset + 0)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t matchEventIndicatorId(void)
    {
        return 5799;
    }

    static SBE_CONSTEXPR const std::uint64_t matchEventIndicatorSinceVersion(void)
    {
         return 0;
    }

    bool matchEventIndicatorInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= matchEventIndicatorSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MatchEventIndicatorMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "MultipleCharValue";
        }

        return "";
    }

private:
    MatchEventIndicator m_matchEventIndicator;

public:

    MatchEventIndicator &matchEventIndicator()
    {
        m_matchEventIndicator.wrap(m_buffer, m_offset + 8, m_actingVersion, m_bufferLength);
        return m_matchEventIndicator;
    }

    class NoMDEntries
    {
    private:
        char *m_buffer;
        std::uint64_t m_bufferLength;
        std::uint64_t *m_positionPtr;
        std::uint64_t m_blockLength;
        std::uint64_t m_count;
        std::uint64_t m_index;
        std::uint64_t m_offset;
        std::uint64_t m_actingVersion;
        GroupSize m_dimensions;

    public:

        inline void wrapForDecode(char *buffer, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_blockLength = m_dimensions.blockLength();
            m_count = m_dimensions.numInGroup();
            m_index = -1;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 3;
        }

        inline void wrapForEncode(char *buffer, const std::uint8_t count, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            if (count < 0 || count > 254)
            {
                throw std::runtime_error("count outside of allowed range [E110]");
            }
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_dimensions.blockLength((std::uint16_t)32);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 32;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 3;
        }

        static SBE_CONSTEXPR const std::uint64_t sbeHeaderSize()
        {
            return 3;
        }

        static SBE_CONSTEXPR const std::uint64_t sbeBlockLength()
        {
            return 32;
        }

        std::uint64_t position(void) const
        {
            return *m_positionPtr;
        }

        void position(const std::uint64_t position)
        {
            if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
            {
                 throw std::runtime_error("buffer too short [E100]");
            }
            *m_positionPtr = position;
        }

        inline std::uint64_t count(void) const
        {
            return m_count;
        }

        inline bool hasNext(void) const
        {
            return m_index + 1 < m_count;
        }

        inline NoMDEntries &next(void)
        {
            m_offset = *m_positionPtr;
            if (SBE_BOUNDS_CHECK_EXPECT(( (m_offset + m_blockLength) > m_bufferLength ), false))
            {
                throw std::runtime_error("buffer too short to support next group index [E108]");
            }
            *m_positionPtr = m_offset + m_blockLength;
            ++m_index;

            return *this;
        }

    #if __cplusplus < 201103L
        template<class Func> inline void forEach(Func& func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #else
        template<class Func> inline void forEach(Func&& func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #endif


        static SBE_CONSTEXPR const std::uint16_t mDEntryPxId(void)
        {
            return 270;
        }

        static SBE_CONSTEXPR const std::uint64_t mDEntryPxSinceVersion(void)
        {
             return 0;
        }

        bool mDEntryPxInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDEntryPxSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDEntryPxMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
            }

            return "";
        }

private:
        PRICENULL m_mDEntryPx;

public:

        PRICENULL &mDEntryPx(void)
        {
            m_mDEntryPx.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
            return m_mDEntryPx;
        }

        static SBE_CONSTEXPR const std::uint16_t mDEntrySizeId(void)
        {
            return 271;
        }

        static SBE_CONSTEXPR const std::uint64_t mDEntrySizeSinceVersion(void)
        {
             return 0;
        }

        bool mDEntrySizeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDEntrySizeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDEntrySizeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t mDEntrySizeNullValue()
        {
            return 2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t mDEntrySizeMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t mDEntrySizeMaxValue()
        {
            return 2147483647;
        }

        std::int32_t mDEntrySize(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 8)));
        }

        NoMDEntries &mDEntrySize(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 8)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t securityIDId(void)
        {
            return 48;
        }

        static SBE_CONSTEXPR const std::uint64_t securityIDSinceVersion(void)
        {
             return 0;
        }

        bool securityIDInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= securityIDSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *SecurityIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t securityIDNullValue()
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR const std::int32_t securityIDMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t securityIDMaxValue()
        {
            return 2147483647;
        }

        std::int32_t securityID(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 12)));
        }

        NoMDEntries &securityID(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 12)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t rptSeqId(void)
        {
            return 83;
        }

        static SBE_CONSTEXPR const std::uint64_t rptSeqSinceVersion(void)
        {
             return 0;
        }

        bool rptSeqInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= rptSeqSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *RptSeqMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint32_t rptSeqNullValue()
        {
            return SBE_NULLVALUE_UINT32;
        }

        static SBE_CONSTEXPR const std::uint32_t rptSeqMinValue()
        {
            return 0;
        }

        static SBE_CONSTEXPR const std::uint32_t rptSeqMaxValue()
        {
            return 4294967293;
        }

        std::uint32_t rptSeq(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::uint32_t *)(m_buffer + m_offset + 16)));
        }

        NoMDEntries &rptSeq(const std::uint32_t value)
        {
            *((std::uint32_t *)(m_buffer + m_offset + 16)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t numberOfOrdersId(void)
        {
            return 346;
        }

        static SBE_CONSTEXPR const std::uint64_t numberOfOrdersSinceVersion(void)
        {
             return 0;
        }

        bool numberOfOrdersInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= numberOfOrdersSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *NumberOfOrdersMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t numberOfOrdersNullValue()
        {
            return 2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t numberOfOrdersMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t numberOfOrdersMaxValue()
        {
            return 2147483647;
        }

        std::int32_t numberOfOrders(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 20)));
        }

        NoMDEntries &numberOfOrders(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 20)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t mDPriceLevelId(void)
        {
            return 1023;
        }

        static SBE_CONSTEXPR const std::uint64_t mDPriceLevelSinceVersion(void)
        {
             return 0;
        }

        bool mDPriceLevelInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDPriceLevelSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDPriceLevelMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint8_t mDPriceLevelNullValue()
        {
            return SBE_NULLVALUE_UINT8;
        }

        static SBE_CONSTEXPR const std::uint8_t mDPriceLevelMinValue()
        {
            return (std::uint8_t)0;
        }

        static SBE_CONSTEXPR const std::uint8_t mDPriceLevelMaxValue()
        {
            return (std::uint8_t)254;
        }

        std::uint8_t mDPriceLevel(void) const
        {
            return (*((std::uint8_t *)(m_buffer + m_offset + 24)));
        }

        NoMDEntries &mDPriceLevel(const std::uint8_t value)
        {
            *((std::uint8_t *)(m_buffer + m_offset + 24)) = (value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t mDUpdateActionId(void)
        {
            return 279;
        }

        static SBE_CONSTEXPR const std::uint64_t mDUpdateActionSinceVersion(void)
        {
             return 0;
        }

        bool mDUpdateActionInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDUpdateActionSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDUpdateActionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        MDUpdateAction::Value mDUpdateAction(void) const
        {
            return MDUpdateAction::get((*((std::uint8_t *)(m_buffer + m_offset + 25))));
        }

        NoMDEntries &mDUpdateAction(const MDUpdateAction::Value value)
        {
            *((std::uint8_t *)(m_buffer + m_offset + 25)) = (value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t mDEntryTypeId(void)
        {
            return 269;
        }

        static SBE_CONSTEXPR const std::uint64_t mDEntryTypeSinceVersion(void)
        {
             return 0;
        }

        bool mDEntryTypeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDEntryTypeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDEntryTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "char";
            }

            return "";
        }

        MDEntryTypeBook::Value mDEntryType(void) const
        {
            return MDEntryTypeBook::get((*((char *)(m_buffer + m_offset + 26))));
        }

        NoMDEntries &mDEntryType(const MDEntryTypeBook::Value value)
        {
            *((char *)(m_buffer + m_offset + 26)) = (value);
            return *this;
        }
    };

private:
    NoMDEntries m_noMDEntries;

public:

    static SBE_CONSTEXPR const std::uint16_t NoMDEntriesId(void)
    {
        return 268;
    }


    inline NoMDEntries &noMDEntries(void)
    {
        m_noMDEntries.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDEntries;
    }

    NoMDEntries &noMDEntriesCount(const std::uint8_t count)
    {
        m_noMDEntries.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDEntries;
    }

    static SBE_CONSTEXPR const std::uint64_t noMDEntriesSinceVersion(void)
    {
         return 0;
    }

    bool noMDEntriesInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noMDEntriesSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoOrderIDEntries
    {
    private:
        char *m_buffer;
        std::uint64_t m_bufferLength;
        std::uint64_t *m_positionPtr;
        std::uint64_t m_blockLength;
        std::uint64_t m_count;
        std::uint64_t m_index;
        std::uint64_t m_offset;
        std::uint64_t m_actingVersion;
        GroupSize8Byte m_dimensions;

    public:

        inline void wrapForDecode(char *buffer, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_blockLength = m_dimensions.blockLength();
            m_count = m_dimensions.numInGroup();
            m_index = -1;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 8;
        }

        inline void wrapForEncode(char *buffer, const std::uint8_t count, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            if (count < 0 || count > 254)
            {
                throw std::runtime_error("count outside of allowed range [E110]");
            }
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_dimensions.blockLength((std::uint16_t)24);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 24;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 8;
        }

        static SBE_CONSTEXPR const std::uint64_t sbeHeaderSize()
        {
            return 8;
        }

        static SBE_CONSTEXPR const std::uint64_t sbeBlockLength()
        {
            return 24;
        }

        std::uint64_t position(void) const
        {
            return *m_positionPtr;
        }

        void position(const std::uint64_t position)
        {
            if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
            {
                 throw std::runtime_error("buffer too short [E100]");
            }
            *m_positionPtr = position;
        }

        inline std::uint64_t count(void) const
        {
            return m_count;
        }

        inline bool hasNext(void) const
        {
            return m_index + 1 < m_count;
        }

        inline NoOrderIDEntries &next(void)
        {
            m_offset = *m_positionPtr;
            if (SBE_BOUNDS_CHECK_EXPECT(( (m_offset + m_blockLength) > m_bufferLength ), false))
            {
                throw std::runtime_error("buffer too short to support next group index [E108]");
            }
            *m_positionPtr = m_offset + m_blockLength;
            ++m_index;

            return *this;
        }

    #if __cplusplus < 201103L
        template<class Func> inline void forEach(Func& func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #else
        template<class Func> inline void forEach(Func&& func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #endif


        static SBE_CONSTEXPR const std::uint16_t orderIDId(void)
        {
            return 37;
        }

        static SBE_CONSTEXPR const std::uint64_t orderIDSinceVersion(void)
        {
             return 0;
        }

        bool orderIDInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= orderIDSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *OrderIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint64_t orderIDNullValue()
        {
            return SBE_NULLVALUE_UINT64;
        }

        static SBE_CONSTEXPR const std::uint64_t orderIDMinValue()
        {
            return 0x0L;
        }

        static SBE_CONSTEXPR const std::uint64_t orderIDMaxValue()
        {
            return 0xfffffffffffffffeL;
        }

        std::uint64_t orderID(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 0)));
        }

        NoOrderIDEntries &orderID(const std::uint64_t value)
        {
            *((std::uint64_t *)(m_buffer + m_offset + 0)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t mDOrderPriorityId(void)
        {
            return 37707;
        }

        static SBE_CONSTEXPR const std::uint64_t mDOrderPrioritySinceVersion(void)
        {
             return 0;
        }

        bool mDOrderPriorityInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDOrderPrioritySinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDOrderPriorityMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint64_t mDOrderPriorityNullValue()
        {
            return 0xffffffffffffffffL;
        }

        static SBE_CONSTEXPR const std::uint64_t mDOrderPriorityMinValue()
        {
            return 0x0L;
        }

        static SBE_CONSTEXPR const std::uint64_t mDOrderPriorityMaxValue()
        {
            return 0xfffffffffffffffeL;
        }

        std::uint64_t mDOrderPriority(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 8)));
        }

        NoOrderIDEntries &mDOrderPriority(const std::uint64_t value)
        {
            *((std::uint64_t *)(m_buffer + m_offset + 8)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t mDDisplayQtyId(void)
        {
            return 37706;
        }

        static SBE_CONSTEXPR const std::uint64_t mDDisplayQtySinceVersion(void)
        {
             return 0;
        }

        bool mDDisplayQtyInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDDisplayQtySinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDDisplayQtyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t mDDisplayQtyNullValue()
        {
            return 2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t mDDisplayQtyMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t mDDisplayQtyMaxValue()
        {
            return 2147483647;
        }

        std::int32_t mDDisplayQty(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 16)));
        }

        NoOrderIDEntries &mDDisplayQty(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 16)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t referenceIDId(void)
        {
            return 9633;
        }

        static SBE_CONSTEXPR const std::uint64_t referenceIDSinceVersion(void)
        {
             return 0;
        }

        bool referenceIDInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= referenceIDSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *ReferenceIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint8_t referenceIDNullValue()
        {
            return (std::uint8_t)255;
        }

        static SBE_CONSTEXPR const std::uint8_t referenceIDMinValue()
        {
            return (std::uint8_t)0;
        }

        static SBE_CONSTEXPR const std::uint8_t referenceIDMaxValue()
        {
            return (std::uint8_t)254;
        }

        std::uint8_t referenceID(void) const
        {
            return (*((std::uint8_t *)(m_buffer + m_offset + 20)));
        }

        NoOrderIDEntries &referenceID(const std::uint8_t value)
        {
            *((std::uint8_t *)(m_buffer + m_offset + 20)) = (value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t orderUpdateActionId(void)
        {
            return 37708;
        }

        static SBE_CONSTEXPR const std::uint64_t orderUpdateActionSinceVersion(void)
        {
             return 0;
        }

        bool orderUpdateActionInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= orderUpdateActionSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *OrderUpdateActionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        OrderUpdateAction::Value orderUpdateAction(void) const
        {
            return OrderUpdateAction::get((*((std::uint8_t *)(m_buffer + m_offset + 21))));
        }

        NoOrderIDEntries &orderUpdateAction(const OrderUpdateAction::Value value)
        {
            *((std::uint8_t *)(m_buffer + m_offset + 21)) = (value);
            return *this;
        }
    };

private:
    NoOrderIDEntries m_noOrderIDEntries;

public:

    static SBE_CONSTEXPR const std::uint16_t NoOrderIDEntriesId(void)
    {
        return 37705;
    }


    inline NoOrderIDEntries &noOrderIDEntries(void)
    {
        m_noOrderIDEntries.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noOrderIDEntries;
    }

    NoOrderIDEntries &noOrderIDEntriesCount(const std::uint8_t count)
    {
        m_noOrderIDEntries.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noOrderIDEntries;
    }

    static SBE_CONSTEXPR const std::uint64_t noOrderIDEntriesSinceVersion(void)
    {
         return 7;
    }

    bool noOrderIDEntriesInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noOrderIDEntriesSinceVersion();
#pragma GCC diagnostic pop
    }
};
}
#endif
