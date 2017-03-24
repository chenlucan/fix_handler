/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _MKTDATA_MDINSTRUMENTDEFINITIONOPTION41_H_
#define _MKTDATA_MDINSTRUMENTDEFINITIONOPTION41_H_

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

class MDInstrumentDefinitionOption41
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

    MDInstrumentDefinitionOption41(void) : m_buffer(nullptr), m_bufferLength(0), m_offset(0) {}

    MDInstrumentDefinitionOption41(char *buffer, const std::uint64_t bufferLength)
    {
        reset(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion());
    }

    MDInstrumentDefinitionOption41(char *buffer, const std::uint64_t bufferLength, const std::uint64_t actingBlockLength, const std::uint64_t actingVersion)
    {
        reset(buffer, 0, bufferLength, actingBlockLength, actingVersion);
    }

    static SBE_CONSTEXPR const std::uint16_t sbeBlockLength(void)
    {
        return (std::uint16_t)213;
    }

    static SBE_CONSTEXPR const std::uint16_t sbeTemplateId(void)
    {
        return (std::uint16_t)41;
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
        return "d";
    }

    std::uint64_t offset(void) const
    {
        return m_offset;
    }

    MDInstrumentDefinitionOption41 &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        reset(buffer, offset, bufferLength, sbeBlockLength(), sbeSchemaVersion());
        return *this;
    }

    MDInstrumentDefinitionOption41 &wrapForDecode(
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
        m_matchEventIndicator.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
        return m_matchEventIndicator;
    }

    static SBE_CONSTEXPR const std::uint16_t totNumReportsId(void)
    {
        return 911;
    }

    static SBE_CONSTEXPR const std::uint64_t totNumReportsSinceVersion(void)
    {
         return 0;
    }

    bool totNumReportsInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= totNumReportsSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *TotNumReportsMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint32_t totNumReportsNullValue()
    {
        return 4294967295;
    }

    static SBE_CONSTEXPR const std::uint32_t totNumReportsMinValue()
    {
        return 0;
    }

    static SBE_CONSTEXPR const std::uint32_t totNumReportsMaxValue()
    {
        return 4294967293;
    }

    std::uint32_t totNumReports(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::uint32_t *)(m_buffer + m_offset + 1)));
    }

    MDInstrumentDefinitionOption41 &totNumReports(const std::uint32_t value)
    {
        *((std::uint32_t *)(m_buffer + m_offset + 1)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t securityUpdateActionId(void)
    {
        return 980;
    }

    static SBE_CONSTEXPR const std::uint64_t securityUpdateActionSinceVersion(void)
    {
         return 0;
    }

    bool securityUpdateActionInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= securityUpdateActionSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SecurityUpdateActionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "char";
        }

        return "";
    }

    SecurityUpdateAction::Value securityUpdateAction(void) const
    {
        return SecurityUpdateAction::get((*((char *)(m_buffer + m_offset + 5))));
    }

    MDInstrumentDefinitionOption41 &securityUpdateAction(const SecurityUpdateAction::Value value)
    {
        *((char *)(m_buffer + m_offset + 5)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t lastUpdateTimeId(void)
    {
        return 779;
    }

    static SBE_CONSTEXPR const std::uint64_t lastUpdateTimeSinceVersion(void)
    {
         return 0;
    }

    bool lastUpdateTimeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= lastUpdateTimeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *LastUpdateTimeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint64_t lastUpdateTimeNullValue()
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR const std::uint64_t lastUpdateTimeMinValue()
    {
        return 0x0L;
    }

    static SBE_CONSTEXPR const std::uint64_t lastUpdateTimeMaxValue()
    {
        return 0xfffffffffffffffeL;
    }

    std::uint64_t lastUpdateTime(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 6)));
    }

    MDInstrumentDefinitionOption41 &lastUpdateTime(const std::uint64_t value)
    {
        *((std::uint64_t *)(m_buffer + m_offset + 6)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t mDSecurityTradingStatusId(void)
    {
        return 1682;
    }

    static SBE_CONSTEXPR const std::uint64_t mDSecurityTradingStatusSinceVersion(void)
    {
         return 0;
    }

    bool mDSecurityTradingStatusInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= mDSecurityTradingStatusSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MDSecurityTradingStatusMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    SecurityTradingStatus::Value mDSecurityTradingStatus(void) const
    {
        return SecurityTradingStatus::get((*((std::uint8_t *)(m_buffer + m_offset + 14))));
    }

    MDInstrumentDefinitionOption41 &mDSecurityTradingStatus(const SecurityTradingStatus::Value value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 14)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t applIDId(void)
    {
        return 1180;
    }

    static SBE_CONSTEXPR const std::uint64_t applIDSinceVersion(void)
    {
         return 0;
    }

    bool applIDInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= applIDSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *ApplIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::int16_t applIDNullValue()
    {
        return SBE_NULLVALUE_INT16;
    }

    static SBE_CONSTEXPR const std::int16_t applIDMinValue()
    {
        return (std::int16_t)-32767;
    }

    static SBE_CONSTEXPR const std::int16_t applIDMaxValue()
    {
        return (std::int16_t)32767;
    }

    std::int16_t applID(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_16(*((std::int16_t *)(m_buffer + m_offset + 15)));
    }

    MDInstrumentDefinitionOption41 &applID(const std::int16_t value)
    {
        *((std::int16_t *)(m_buffer + m_offset + 15)) = SBE_LITTLE_ENDIAN_ENCODE_16(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t marketSegmentIDId(void)
    {
        return 1300;
    }

    static SBE_CONSTEXPR const std::uint64_t marketSegmentIDSinceVersion(void)
    {
         return 0;
    }

    bool marketSegmentIDInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= marketSegmentIDSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MarketSegmentIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint8_t marketSegmentIDNullValue()
    {
        return SBE_NULLVALUE_UINT8;
    }

    static SBE_CONSTEXPR const std::uint8_t marketSegmentIDMinValue()
    {
        return (std::uint8_t)0;
    }

    static SBE_CONSTEXPR const std::uint8_t marketSegmentIDMaxValue()
    {
        return (std::uint8_t)254;
    }

    std::uint8_t marketSegmentID(void) const
    {
        return (*((std::uint8_t *)(m_buffer + m_offset + 17)));
    }

    MDInstrumentDefinitionOption41 &marketSegmentID(const std::uint8_t value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 17)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t underlyingProductId(void)
    {
        return 462;
    }

    static SBE_CONSTEXPR const std::uint64_t underlyingProductSinceVersion(void)
    {
         return 0;
    }

    bool underlyingProductInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= underlyingProductSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *UnderlyingProductMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint8_t underlyingProductNullValue()
    {
        return SBE_NULLVALUE_UINT8;
    }

    static SBE_CONSTEXPR const std::uint8_t underlyingProductMinValue()
    {
        return (std::uint8_t)0;
    }

    static SBE_CONSTEXPR const std::uint8_t underlyingProductMaxValue()
    {
        return (std::uint8_t)254;
    }

    std::uint8_t underlyingProduct(void) const
    {
        return (*((std::uint8_t *)(m_buffer + m_offset + 18)));
    }

    MDInstrumentDefinitionOption41 &underlyingProduct(const std::uint8_t value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 18)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t securityExchangeId(void)
    {
        return 207;
    }

    static SBE_CONSTEXPR const std::uint64_t securityExchangeSinceVersion(void)
    {
         return 0;
    }

    bool securityExchangeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= securityExchangeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SecurityExchangeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Exchange";
        }

        return "";
    }

    static SBE_CONSTEXPR const char securityExchangeNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char securityExchangeMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char securityExchangeMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t securityExchangeLength(void)
    {
        return 4;
    }

    const char *securityExchange(void) const
    {
        return (m_buffer + m_offset + 19);
    }

    char securityExchange(const std::uint64_t index) const
    {
        if (index >= 4)
        {
            throw std::runtime_error("index out of range for securityExchange [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 19 + (index * 1))));
    }

    void securityExchange(const std::uint64_t index, const char value)
    {
        if (index >= 4)
        {
            throw std::runtime_error("index out of range for securityExchange [E105]");
        }

        *((char *)(m_buffer + m_offset + 19 + (index * 1))) = (value);
    }

    std::uint64_t getSecurityExchange(char *dst, const std::uint64_t length) const
    {
        if (length > 4)
        {
             throw std::runtime_error("length too large for getSecurityExchange [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 19, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putSecurityExchange(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 19, src, 4);
        return *this;
    }

    std::string getSecurityExchangeAsString() const
    {
        std::string result(m_buffer + m_offset + 19, 4);
        return result;
    }

    MDInstrumentDefinitionOption41 &putSecurityExchange(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 19, str.c_str(), 4);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t securityGroupId(void)
    {
        return 1151;
    }

    static SBE_CONSTEXPR const std::uint64_t securityGroupSinceVersion(void)
    {
         return 0;
    }

    bool securityGroupInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= securityGroupSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SecurityGroupMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char securityGroupNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char securityGroupMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char securityGroupMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t securityGroupLength(void)
    {
        return 6;
    }

    const char *securityGroup(void) const
    {
        return (m_buffer + m_offset + 23);
    }

    char securityGroup(const std::uint64_t index) const
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for securityGroup [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 23 + (index * 1))));
    }

    void securityGroup(const std::uint64_t index, const char value)
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for securityGroup [E105]");
        }

        *((char *)(m_buffer + m_offset + 23 + (index * 1))) = (value);
    }

    std::uint64_t getSecurityGroup(char *dst, const std::uint64_t length) const
    {
        if (length > 6)
        {
             throw std::runtime_error("length too large for getSecurityGroup [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 23, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putSecurityGroup(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 23, src, 6);
        return *this;
    }

    std::string getSecurityGroupAsString() const
    {
        std::string result(m_buffer + m_offset + 23, 6);
        return result;
    }

    MDInstrumentDefinitionOption41 &putSecurityGroup(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 23, str.c_str(), 6);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t assetId(void)
    {
        return 6937;
    }

    static SBE_CONSTEXPR const std::uint64_t assetSinceVersion(void)
    {
         return 0;
    }

    bool assetInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= assetSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *AssetMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char assetNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char assetMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char assetMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t assetLength(void)
    {
        return 6;
    }

    const char *asset(void) const
    {
        return (m_buffer + m_offset + 29);
    }

    char asset(const std::uint64_t index) const
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for asset [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 29 + (index * 1))));
    }

    void asset(const std::uint64_t index, const char value)
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for asset [E105]");
        }

        *((char *)(m_buffer + m_offset + 29 + (index * 1))) = (value);
    }

    std::uint64_t getAsset(char *dst, const std::uint64_t length) const
    {
        if (length > 6)
        {
             throw std::runtime_error("length too large for getAsset [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 29, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putAsset(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 29, src, 6);
        return *this;
    }

    std::string getAssetAsString() const
    {
        std::string result(m_buffer + m_offset + 29, 6);
        return result;
    }

    MDInstrumentDefinitionOption41 &putAsset(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 29, str.c_str(), 6);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t symbolId(void)
    {
        return 55;
    }

    static SBE_CONSTEXPR const std::uint64_t symbolSinceVersion(void)
    {
         return 0;
    }

    bool symbolInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= symbolSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SymbolMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char symbolNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char symbolMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char symbolMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t symbolLength(void)
    {
        return 20;
    }

    const char *symbol(void) const
    {
        return (m_buffer + m_offset + 35);
    }

    char symbol(const std::uint64_t index) const
    {
        if (index >= 20)
        {
            throw std::runtime_error("index out of range for symbol [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 35 + (index * 1))));
    }

    void symbol(const std::uint64_t index, const char value)
    {
        if (index >= 20)
        {
            throw std::runtime_error("index out of range for symbol [E105]");
        }

        *((char *)(m_buffer + m_offset + 35 + (index * 1))) = (value);
    }

    std::uint64_t getSymbol(char *dst, const std::uint64_t length) const
    {
        if (length > 20)
        {
             throw std::runtime_error("length too large for getSymbol [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 35, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putSymbol(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 35, src, 20);
        return *this;
    }

    std::string getSymbolAsString() const
    {
        std::string result(m_buffer + m_offset + 35, 20);
        return result;
    }

    MDInstrumentDefinitionOption41 &putSymbol(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 35, str.c_str(), 20);
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
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 55)));
    }

    MDInstrumentDefinitionOption41 &securityID(const std::int32_t value)
    {
        *((std::int32_t *)(m_buffer + m_offset + 55)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t securityIDSourceId(void)
    {
        return 22;
    }

    static SBE_CONSTEXPR const std::uint64_t securityIDSourceSinceVersion(void)
    {
         return 0;
    }

    bool securityIDSourceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= securityIDSourceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SecurityIDSourceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "char";
        }

        return "";
    }

    static SBE_CONSTEXPR const char securityIDSourceNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char securityIDSourceMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char securityIDSourceMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t securityIDSourceLength(void)
    {
        return 1;
    }

    const char *securityIDSource(void) const
    {
        static sbe_uint8_t securityIDSourceValues[] = {56};

        return (const char *)securityIDSourceValues;
    }

    char securityIDSource(const std::uint64_t index) const
    {
        static sbe_uint8_t securityIDSourceValues[] = {56};

        return securityIDSourceValues[index];
    }

    std::uint64_t getSecurityIDSource(char *dst, const std::uint64_t length) const
    {
        static sbe_uint8_t securityIDSourceValues[] = {56};
        std::uint64_t bytesToCopy = (length < sizeof(securityIDSourceValues)) ? length : sizeof(securityIDSourceValues);

        std::memcpy(dst, securityIDSourceValues, bytesToCopy);
        return bytesToCopy;
    }

    static SBE_CONSTEXPR const std::uint16_t securityTypeId(void)
    {
        return 167;
    }

    static SBE_CONSTEXPR const std::uint64_t securityTypeSinceVersion(void)
    {
         return 0;
    }

    bool securityTypeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= securityTypeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SecurityTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char securityTypeNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char securityTypeMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char securityTypeMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t securityTypeLength(void)
    {
        return 6;
    }

    const char *securityType(void) const
    {
        return (m_buffer + m_offset + 59);
    }

    char securityType(const std::uint64_t index) const
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for securityType [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 59 + (index * 1))));
    }

    void securityType(const std::uint64_t index, const char value)
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for securityType [E105]");
        }

        *((char *)(m_buffer + m_offset + 59 + (index * 1))) = (value);
    }

    std::uint64_t getSecurityType(char *dst, const std::uint64_t length) const
    {
        if (length > 6)
        {
             throw std::runtime_error("length too large for getSecurityType [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 59, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putSecurityType(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 59, src, 6);
        return *this;
    }

    std::string getSecurityTypeAsString() const
    {
        std::string result(m_buffer + m_offset + 59, 6);
        return result;
    }

    MDInstrumentDefinitionOption41 &putSecurityType(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 59, str.c_str(), 6);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t cFICodeId(void)
    {
        return 461;
    }

    static SBE_CONSTEXPR const std::uint64_t cFICodeSinceVersion(void)
    {
         return 0;
    }

    bool cFICodeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= cFICodeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *CFICodeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char cFICodeNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char cFICodeMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char cFICodeMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t cFICodeLength(void)
    {
        return 6;
    }

    const char *cFICode(void) const
    {
        return (m_buffer + m_offset + 65);
    }

    char cFICode(const std::uint64_t index) const
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for cFICode [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 65 + (index * 1))));
    }

    void cFICode(const std::uint64_t index, const char value)
    {
        if (index >= 6)
        {
            throw std::runtime_error("index out of range for cFICode [E105]");
        }

        *((char *)(m_buffer + m_offset + 65 + (index * 1))) = (value);
    }

    std::uint64_t getCFICode(char *dst, const std::uint64_t length) const
    {
        if (length > 6)
        {
             throw std::runtime_error("length too large for getCFICode [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 65, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putCFICode(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 65, src, 6);
        return *this;
    }

    std::string getCFICodeAsString() const
    {
        std::string result(m_buffer + m_offset + 65, 6);
        return result;
    }

    MDInstrumentDefinitionOption41 &putCFICode(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 65, str.c_str(), 6);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t putOrCallId(void)
    {
        return 201;
    }

    static SBE_CONSTEXPR const std::uint64_t putOrCallSinceVersion(void)
    {
         return 0;
    }

    bool putOrCallInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= putOrCallSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *PutOrCallMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    PutOrCall::Value putOrCall(void) const
    {
        return PutOrCall::get((*((std::uint8_t *)(m_buffer + m_offset + 71))));
    }

    MDInstrumentDefinitionOption41 &putOrCall(const PutOrCall::Value value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 71)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t maturityMonthYearId(void)
    {
        return 200;
    }

    static SBE_CONSTEXPR const std::uint64_t maturityMonthYearSinceVersion(void)
    {
         return 0;
    }

    bool maturityMonthYearInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= maturityMonthYearSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MaturityMonthYearMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "MonthYear";
        }

        return "";
    }

private:
    MaturityMonthYear m_maturityMonthYear;

public:

    MaturityMonthYear &maturityMonthYear(void)
    {
        m_maturityMonthYear.wrap(m_buffer, m_offset + 72, m_actingVersion, m_bufferLength);
        return m_maturityMonthYear;
    }

    static SBE_CONSTEXPR const std::uint16_t currencyId(void)
    {
        return 15;
    }

    static SBE_CONSTEXPR const std::uint64_t currencySinceVersion(void)
    {
         return 0;
    }

    bool currencyInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= currencySinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *CurrencyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Currency";
        }

        return "";
    }

    static SBE_CONSTEXPR const char currencyNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char currencyMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char currencyMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t currencyLength(void)
    {
        return 3;
    }

    const char *currency(void) const
    {
        return (m_buffer + m_offset + 77);
    }

    char currency(const std::uint64_t index) const
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 77 + (index * 1))));
    }

    void currency(const std::uint64_t index, const char value)
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency [E105]");
        }

        *((char *)(m_buffer + m_offset + 77 + (index * 1))) = (value);
    }

    std::uint64_t getCurrency(char *dst, const std::uint64_t length) const
    {
        if (length > 3)
        {
             throw std::runtime_error("length too large for getCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 77, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putCurrency(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 77, src, 3);
        return *this;
    }

    std::string getCurrencyAsString() const
    {
        std::string result(m_buffer + m_offset + 77, 3);
        return result;
    }

    MDInstrumentDefinitionOption41 &putCurrency(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 77, str.c_str(), 3);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t strikePriceId(void)
    {
        return 202;
    }

    static SBE_CONSTEXPR const std::uint64_t strikePriceSinceVersion(void)
    {
         return 0;
    }

    bool strikePriceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= strikePriceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *StrikePriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_strikePrice;

public:

    PRICENULL &strikePrice(void)
    {
        m_strikePrice.wrap(m_buffer, m_offset + 80, m_actingVersion, m_bufferLength);
        return m_strikePrice;
    }

    static SBE_CONSTEXPR const std::uint16_t strikeCurrencyId(void)
    {
        return 947;
    }

    static SBE_CONSTEXPR const std::uint64_t strikeCurrencySinceVersion(void)
    {
         return 0;
    }

    bool strikeCurrencyInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= strikeCurrencySinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *StrikeCurrencyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Currency";
        }

        return "";
    }

    static SBE_CONSTEXPR const char strikeCurrencyNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char strikeCurrencyMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char strikeCurrencyMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t strikeCurrencyLength(void)
    {
        return 3;
    }

    const char *strikeCurrency(void) const
    {
        return (m_buffer + m_offset + 88);
    }

    char strikeCurrency(const std::uint64_t index) const
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for strikeCurrency [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 88 + (index * 1))));
    }

    void strikeCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for strikeCurrency [E105]");
        }

        *((char *)(m_buffer + m_offset + 88 + (index * 1))) = (value);
    }

    std::uint64_t getStrikeCurrency(char *dst, const std::uint64_t length) const
    {
        if (length > 3)
        {
             throw std::runtime_error("length too large for getStrikeCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 88, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putStrikeCurrency(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 88, src, 3);
        return *this;
    }

    std::string getStrikeCurrencyAsString() const
    {
        std::string result(m_buffer + m_offset + 88, 3);
        return result;
    }

    MDInstrumentDefinitionOption41 &putStrikeCurrency(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 88, str.c_str(), 3);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t settlCurrencyId(void)
    {
        return 120;
    }

    static SBE_CONSTEXPR const std::uint64_t settlCurrencySinceVersion(void)
    {
         return 0;
    }

    bool settlCurrencyInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= settlCurrencySinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SettlCurrencyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Currency";
        }

        return "";
    }

    static SBE_CONSTEXPR const char settlCurrencyNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char settlCurrencyMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char settlCurrencyMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t settlCurrencyLength(void)
    {
        return 3;
    }

    const char *settlCurrency(void) const
    {
        return (m_buffer + m_offset + 91);
    }

    char settlCurrency(const std::uint64_t index) const
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for settlCurrency [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 91 + (index * 1))));
    }

    void settlCurrency(const std::uint64_t index, const char value)
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for settlCurrency [E105]");
        }

        *((char *)(m_buffer + m_offset + 91 + (index * 1))) = (value);
    }

    std::uint64_t getSettlCurrency(char *dst, const std::uint64_t length) const
    {
        if (length > 3)
        {
             throw std::runtime_error("length too large for getSettlCurrency [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 91, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putSettlCurrency(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 91, src, 3);
        return *this;
    }

    std::string getSettlCurrencyAsString() const
    {
        std::string result(m_buffer + m_offset + 91, 3);
        return result;
    }

    MDInstrumentDefinitionOption41 &putSettlCurrency(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 91, str.c_str(), 3);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t minCabPriceId(void)
    {
        return 9850;
    }

    static SBE_CONSTEXPR const std::uint64_t minCabPriceSinceVersion(void)
    {
         return 0;
    }

    bool minCabPriceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= minCabPriceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MinCabPriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_minCabPrice;

public:

    PRICENULL &minCabPrice(void)
    {
        m_minCabPrice.wrap(m_buffer, m_offset + 94, m_actingVersion, m_bufferLength);
        return m_minCabPrice;
    }

    static SBE_CONSTEXPR const std::uint16_t matchAlgorithmId(void)
    {
        return 1142;
    }

    static SBE_CONSTEXPR const std::uint64_t matchAlgorithmSinceVersion(void)
    {
         return 0;
    }

    bool matchAlgorithmInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= matchAlgorithmSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MatchAlgorithmMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "char";
        }

        return "";
    }

    static SBE_CONSTEXPR const char matchAlgorithmNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char matchAlgorithmMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char matchAlgorithmMaxValue()
    {
        return (char)126;
    }

    char matchAlgorithm(void) const
    {
        return (*((char *)(m_buffer + m_offset + 102)));
    }

    MDInstrumentDefinitionOption41 &matchAlgorithm(const char value)
    {
        *((char *)(m_buffer + m_offset + 102)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t minTradeVolId(void)
    {
        return 562;
    }

    static SBE_CONSTEXPR const std::uint64_t minTradeVolSinceVersion(void)
    {
         return 0;
    }

    bool minTradeVolInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= minTradeVolSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MinTradeVolMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint32_t minTradeVolNullValue()
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR const std::uint32_t minTradeVolMinValue()
    {
        return 0;
    }

    static SBE_CONSTEXPR const std::uint32_t minTradeVolMaxValue()
    {
        return 4294967293;
    }

    std::uint32_t minTradeVol(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::uint32_t *)(m_buffer + m_offset + 103)));
    }

    MDInstrumentDefinitionOption41 &minTradeVol(const std::uint32_t value)
    {
        *((std::uint32_t *)(m_buffer + m_offset + 103)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t maxTradeVolId(void)
    {
        return 1140;
    }

    static SBE_CONSTEXPR const std::uint64_t maxTradeVolSinceVersion(void)
    {
         return 0;
    }

    bool maxTradeVolInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= maxTradeVolSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MaxTradeVolMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint32_t maxTradeVolNullValue()
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR const std::uint32_t maxTradeVolMinValue()
    {
        return 0;
    }

    static SBE_CONSTEXPR const std::uint32_t maxTradeVolMaxValue()
    {
        return 4294967293;
    }

    std::uint32_t maxTradeVol(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::uint32_t *)(m_buffer + m_offset + 107)));
    }

    MDInstrumentDefinitionOption41 &maxTradeVol(const std::uint32_t value)
    {
        *((std::uint32_t *)(m_buffer + m_offset + 107)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t minPriceIncrementId(void)
    {
        return 969;
    }

    static SBE_CONSTEXPR const std::uint64_t minPriceIncrementSinceVersion(void)
    {
         return 0;
    }

    bool minPriceIncrementInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= minPriceIncrementSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MinPriceIncrementMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_minPriceIncrement;

public:

    PRICENULL &minPriceIncrement(void)
    {
        m_minPriceIncrement.wrap(m_buffer, m_offset + 111, m_actingVersion, m_bufferLength);
        return m_minPriceIncrement;
    }

    static SBE_CONSTEXPR const std::uint16_t minPriceIncrementAmountId(void)
    {
        return 1146;
    }

    static SBE_CONSTEXPR const std::uint64_t minPriceIncrementAmountSinceVersion(void)
    {
         return 0;
    }

    bool minPriceIncrementAmountInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= minPriceIncrementAmountSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MinPriceIncrementAmountMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_minPriceIncrementAmount;

public:

    PRICENULL &minPriceIncrementAmount(void)
    {
        m_minPriceIncrementAmount.wrap(m_buffer, m_offset + 119, m_actingVersion, m_bufferLength);
        return m_minPriceIncrementAmount;
    }

    static SBE_CONSTEXPR const std::uint16_t displayFactorId(void)
    {
        return 9787;
    }

    static SBE_CONSTEXPR const std::uint64_t displayFactorSinceVersion(void)
    {
         return 0;
    }

    bool displayFactorInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= displayFactorSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *DisplayFactorMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "float";
        }

        return "";
    }

private:
    FLOAT m_displayFactor;

public:

    FLOAT &displayFactor(void)
    {
        m_displayFactor.wrap(m_buffer, m_offset + 127, m_actingVersion, m_bufferLength);
        return m_displayFactor;
    }

    static SBE_CONSTEXPR const std::uint16_t tickRuleId(void)
    {
        return 6350;
    }

    static SBE_CONSTEXPR const std::uint64_t tickRuleSinceVersion(void)
    {
         return 0;
    }

    bool tickRuleInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= tickRuleSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *TickRuleMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::int8_t tickRuleNullValue()
    {
        return (std::int8_t)127;
    }

    static SBE_CONSTEXPR const std::int8_t tickRuleMinValue()
    {
        return (std::int8_t)-127;
    }

    static SBE_CONSTEXPR const std::int8_t tickRuleMaxValue()
    {
        return (std::int8_t)127;
    }

    std::int8_t tickRule(void) const
    {
        return (*((std::int8_t *)(m_buffer + m_offset + 135)));
    }

    MDInstrumentDefinitionOption41 &tickRule(const std::int8_t value)
    {
        *((std::int8_t *)(m_buffer + m_offset + 135)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t mainFractionId(void)
    {
        return 37702;
    }

    static SBE_CONSTEXPR const std::uint64_t mainFractionSinceVersion(void)
    {
         return 0;
    }

    bool mainFractionInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= mainFractionSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *MainFractionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint8_t mainFractionNullValue()
    {
        return (std::uint8_t)255;
    }

    static SBE_CONSTEXPR const std::uint8_t mainFractionMinValue()
    {
        return (std::uint8_t)0;
    }

    static SBE_CONSTEXPR const std::uint8_t mainFractionMaxValue()
    {
        return (std::uint8_t)254;
    }

    std::uint8_t mainFraction(void) const
    {
        return (*((std::uint8_t *)(m_buffer + m_offset + 136)));
    }

    MDInstrumentDefinitionOption41 &mainFraction(const std::uint8_t value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 136)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t subFractionId(void)
    {
        return 37703;
    }

    static SBE_CONSTEXPR const std::uint64_t subFractionSinceVersion(void)
    {
         return 0;
    }

    bool subFractionInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= subFractionSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SubFractionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint8_t subFractionNullValue()
    {
        return (std::uint8_t)255;
    }

    static SBE_CONSTEXPR const std::uint8_t subFractionMinValue()
    {
        return (std::uint8_t)0;
    }

    static SBE_CONSTEXPR const std::uint8_t subFractionMaxValue()
    {
        return (std::uint8_t)254;
    }

    std::uint8_t subFraction(void) const
    {
        return (*((std::uint8_t *)(m_buffer + m_offset + 137)));
    }

    MDInstrumentDefinitionOption41 &subFraction(const std::uint8_t value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 137)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t priceDisplayFormatId(void)
    {
        return 9800;
    }

    static SBE_CONSTEXPR const std::uint64_t priceDisplayFormatSinceVersion(void)
    {
         return 0;
    }

    bool priceDisplayFormatInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= priceDisplayFormatSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *PriceDisplayFormatMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "int";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint8_t priceDisplayFormatNullValue()
    {
        return (std::uint8_t)255;
    }

    static SBE_CONSTEXPR const std::uint8_t priceDisplayFormatMinValue()
    {
        return (std::uint8_t)0;
    }

    static SBE_CONSTEXPR const std::uint8_t priceDisplayFormatMaxValue()
    {
        return (std::uint8_t)254;
    }

    std::uint8_t priceDisplayFormat(void) const
    {
        return (*((std::uint8_t *)(m_buffer + m_offset + 138)));
    }

    MDInstrumentDefinitionOption41 &priceDisplayFormat(const std::uint8_t value)
    {
        *((std::uint8_t *)(m_buffer + m_offset + 138)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t unitOfMeasureId(void)
    {
        return 996;
    }

    static SBE_CONSTEXPR const std::uint64_t unitOfMeasureSinceVersion(void)
    {
         return 0;
    }

    bool unitOfMeasureInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= unitOfMeasureSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *UnitOfMeasureMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "String";
        }

        return "";
    }

    static SBE_CONSTEXPR const char unitOfMeasureNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char unitOfMeasureMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char unitOfMeasureMaxValue()
    {
        return (char)126;
    }

    static SBE_CONSTEXPR const std::uint64_t unitOfMeasureLength(void)
    {
        return 30;
    }

    const char *unitOfMeasure(void) const
    {
        return (m_buffer + m_offset + 139);
    }

    char unitOfMeasure(const std::uint64_t index) const
    {
        if (index >= 30)
        {
            throw std::runtime_error("index out of range for unitOfMeasure [E104]");
        }

        return (*((char *)(m_buffer + m_offset + 139 + (index * 1))));
    }

    void unitOfMeasure(const std::uint64_t index, const char value)
    {
        if (index >= 30)
        {
            throw std::runtime_error("index out of range for unitOfMeasure [E105]");
        }

        *((char *)(m_buffer + m_offset + 139 + (index * 1))) = (value);
    }

    std::uint64_t getUnitOfMeasure(char *dst, const std::uint64_t length) const
    {
        if (length > 30)
        {
             throw std::runtime_error("length too large for getUnitOfMeasure [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 139, length);
        return length;
    }

    MDInstrumentDefinitionOption41 &putUnitOfMeasure(const char *src)
    {
        std::memcpy(m_buffer + m_offset + 139, src, 30);
        return *this;
    }

    std::string getUnitOfMeasureAsString() const
    {
        std::string result(m_buffer + m_offset + 139, 30);
        return result;
    }

    MDInstrumentDefinitionOption41 &putUnitOfMeasure(const std::string& str)
    {
        std::memcpy(m_buffer + m_offset + 139, str.c_str(), 30);
        return *this;
    }


    static SBE_CONSTEXPR const std::uint16_t unitOfMeasureQtyId(void)
    {
        return 1147;
    }

    static SBE_CONSTEXPR const std::uint64_t unitOfMeasureQtySinceVersion(void)
    {
         return 0;
    }

    bool unitOfMeasureQtyInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= unitOfMeasureQtySinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *UnitOfMeasureQtyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
        }

        return "";
    }

private:
    PRICENULL m_unitOfMeasureQty;

public:

    PRICENULL &unitOfMeasureQty(void)
    {
        m_unitOfMeasureQty.wrap(m_buffer, m_offset + 169, m_actingVersion, m_bufferLength);
        return m_unitOfMeasureQty;
    }

    static SBE_CONSTEXPR const std::uint16_t tradingReferencePriceId(void)
    {
        return 1150;
    }

    static SBE_CONSTEXPR const std::uint64_t tradingReferencePriceSinceVersion(void)
    {
         return 0;
    }

    bool tradingReferencePriceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= tradingReferencePriceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *TradingReferencePriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_tradingReferencePrice;

public:

    PRICENULL &tradingReferencePrice(void)
    {
        m_tradingReferencePrice.wrap(m_buffer, m_offset + 177, m_actingVersion, m_bufferLength);
        return m_tradingReferencePrice;
    }

    static SBE_CONSTEXPR const std::uint16_t settlPriceTypeId(void)
    {
        return 731;
    }

    static SBE_CONSTEXPR const std::uint64_t settlPriceTypeSinceVersion(void)
    {
         return 0;
    }

    bool settlPriceTypeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= settlPriceTypeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *SettlPriceTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    SettlPriceType m_settlPriceType;

public:

    SettlPriceType &settlPriceType()
    {
        m_settlPriceType.wrap(m_buffer, m_offset + 185, m_actingVersion, m_bufferLength);
        return m_settlPriceType;
    }

    static SBE_CONSTEXPR const std::uint16_t clearedVolumeId(void)
    {
        return 5791;
    }

    static SBE_CONSTEXPR const std::uint64_t clearedVolumeSinceVersion(void)
    {
         return 0;
    }

    bool clearedVolumeInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= clearedVolumeSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *ClearedVolumeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::int32_t clearedVolumeNullValue()
    {
        return 2147483647;
    }

    static SBE_CONSTEXPR const std::int32_t clearedVolumeMinValue()
    {
        return -2147483647;
    }

    static SBE_CONSTEXPR const std::int32_t clearedVolumeMaxValue()
    {
        return 2147483647;
    }

    std::int32_t clearedVolume(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 186)));
    }

    MDInstrumentDefinitionOption41 &clearedVolume(const std::int32_t value)
    {
        *((std::int32_t *)(m_buffer + m_offset + 186)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t openInterestQtyId(void)
    {
        return 5792;
    }

    static SBE_CONSTEXPR const std::uint64_t openInterestQtySinceVersion(void)
    {
         return 0;
    }

    bool openInterestQtyInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= openInterestQtySinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *OpenInterestQtyMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::int32_t openInterestQtyNullValue()
    {
        return 2147483647;
    }

    static SBE_CONSTEXPR const std::int32_t openInterestQtyMinValue()
    {
        return -2147483647;
    }

    static SBE_CONSTEXPR const std::int32_t openInterestQtyMaxValue()
    {
        return 2147483647;
    }

    std::int32_t openInterestQty(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 190)));
    }

    MDInstrumentDefinitionOption41 &openInterestQty(const std::int32_t value)
    {
        *((std::int32_t *)(m_buffer + m_offset + 190)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t lowLimitPriceId(void)
    {
        return 1148;
    }

    static SBE_CONSTEXPR const std::uint64_t lowLimitPriceSinceVersion(void)
    {
         return 0;
    }

    bool lowLimitPriceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= lowLimitPriceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *LowLimitPriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_lowLimitPrice;

public:

    PRICENULL &lowLimitPrice(void)
    {
        m_lowLimitPrice.wrap(m_buffer, m_offset + 194, m_actingVersion, m_bufferLength);
        return m_lowLimitPrice;
    }

    static SBE_CONSTEXPR const std::uint16_t highLimitPriceId(void)
    {
        return 1149;
    }

    static SBE_CONSTEXPR const std::uint64_t highLimitPriceSinceVersion(void)
    {
         return 0;
    }

    bool highLimitPriceInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= highLimitPriceSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *HighLimitPriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
    PRICENULL m_highLimitPrice;

public:

    PRICENULL &highLimitPrice(void)
    {
        m_highLimitPrice.wrap(m_buffer, m_offset + 202, m_actingVersion, m_bufferLength);
        return m_highLimitPrice;
    }

    static SBE_CONSTEXPR const std::uint16_t userDefinedInstrumentId(void)
    {
        return 9779;
    }

    static SBE_CONSTEXPR const std::uint64_t userDefinedInstrumentSinceVersion(void)
    {
         return 0;
    }

    bool userDefinedInstrumentInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= userDefinedInstrumentSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *UserDefinedInstrumentMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "char";
        }

        return "";
    }

    static SBE_CONSTEXPR const char userDefinedInstrumentNullValue()
    {
        return (char)0;
    }

    static SBE_CONSTEXPR const char userDefinedInstrumentMinValue()
    {
        return (char)32;
    }

    static SBE_CONSTEXPR const char userDefinedInstrumentMaxValue()
    {
        return (char)126;
    }

    char userDefinedInstrument(void) const
    {
        return (*((char *)(m_buffer + m_offset + 210)));
    }

    MDInstrumentDefinitionOption41 &userDefinedInstrument(const char value)
    {
        *((char *)(m_buffer + m_offset + 210)) = (value);
        return *this;
    }

    static SBE_CONSTEXPR const std::uint16_t tradingReferenceDateId(void)
    {
        return 5796;
    }

    static SBE_CONSTEXPR const std::uint64_t tradingReferenceDateSinceVersion(void)
    {
         return 6;
    }

    bool tradingReferenceDateInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= tradingReferenceDateSinceVersion();
#pragma GCC diagnostic pop
    }


    static const char *TradingReferenceDateMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "LocalMktDate";
        }

        return "";
    }

    static SBE_CONSTEXPR const std::uint16_t tradingReferenceDateNullValue()
    {
        return (std::uint16_t)65535;
    }

    static SBE_CONSTEXPR const std::uint16_t tradingReferenceDateMinValue()
    {
        return (std::uint16_t)0;
    }

    static SBE_CONSTEXPR const std::uint16_t tradingReferenceDateMaxValue()
    {
        return (std::uint16_t)65534;
    }

    std::uint16_t tradingReferenceDate(void) const
    {
        if (m_actingVersion < 6)
        {
            return (std::uint16_t)65535;
        }

        return SBE_LITTLE_ENDIAN_ENCODE_16(*((std::uint16_t *)(m_buffer + m_offset + 211)));
    }

    MDInstrumentDefinitionOption41 &tradingReferenceDate(const std::uint16_t value)
    {
        *((std::uint16_t *)(m_buffer + m_offset + 211)) = SBE_LITTLE_ENDIAN_ENCODE_16(value);
        return *this;
    }

    class NoEvents
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
            m_dimensions.blockLength((std::uint16_t)9);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 9;
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
            return 9;
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

        inline NoEvents &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t eventTypeId(void)
        {
            return 865;
        }

        static SBE_CONSTEXPR const std::uint64_t eventTypeSinceVersion(void)
        {
             return 0;
        }

        bool eventTypeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= eventTypeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *EventTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        EventType::Value eventType(void) const
        {
            return EventType::get((*((std::uint8_t *)(m_buffer + m_offset + 0))));
        }

        NoEvents &eventType(const EventType::Value value)
        {
            *((std::uint8_t *)(m_buffer + m_offset + 0)) = (value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t eventTimeId(void)
        {
            return 1145;
        }

        static SBE_CONSTEXPR const std::uint64_t eventTimeSinceVersion(void)
        {
             return 0;
        }

        bool eventTimeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= eventTimeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *EventTimeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::uint64_t eventTimeNullValue()
        {
            return SBE_NULLVALUE_UINT64;
        }

        static SBE_CONSTEXPR const std::uint64_t eventTimeMinValue()
        {
            return 0x0L;
        }

        static SBE_CONSTEXPR const std::uint64_t eventTimeMaxValue()
        {
            return 0xfffffffffffffffeL;
        }

        std::uint64_t eventTime(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 1)));
        }

        NoEvents &eventTime(const std::uint64_t value)
        {
            *((std::uint64_t *)(m_buffer + m_offset + 1)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
            return *this;
        }
    };

private:
    NoEvents m_noEvents;

public:

    static SBE_CONSTEXPR const std::uint16_t NoEventsId(void)
    {
        return 864;
    }


    inline NoEvents &noEvents(void)
    {
        m_noEvents.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noEvents;
    }

    NoEvents &noEventsCount(const std::uint8_t count)
    {
        m_noEvents.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noEvents;
    }

    static SBE_CONSTEXPR const std::uint64_t noEventsSinceVersion(void)
    {
         return 0;
    }

    bool noEventsInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noEventsSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoMDFeedTypes
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
            m_dimensions.blockLength((std::uint16_t)4);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 4;
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
            return 4;
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

        inline NoMDFeedTypes &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t mDFeedTypeId(void)
        {
            return 1022;
        }

        static SBE_CONSTEXPR const std::uint64_t mDFeedTypeSinceVersion(void)
        {
             return 0;
        }

        bool mDFeedTypeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= mDFeedTypeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MDFeedTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "String";
            }

            return "";
        }

        static SBE_CONSTEXPR const char mDFeedTypeNullValue()
        {
            return (char)0;
        }

        static SBE_CONSTEXPR const char mDFeedTypeMinValue()
        {
            return (char)32;
        }

        static SBE_CONSTEXPR const char mDFeedTypeMaxValue()
        {
            return (char)126;
        }

        static SBE_CONSTEXPR const std::uint64_t mDFeedTypeLength(void)
        {
            return 3;
        }

        const char *mDFeedType(void) const
        {
            return (m_buffer + m_offset + 0);
        }

        char mDFeedType(const std::uint64_t index) const
        {
            if (index >= 3)
            {
                throw std::runtime_error("index out of range for mDFeedType [E104]");
            }

            return (*((char *)(m_buffer + m_offset + 0 + (index * 1))));
        }

        void mDFeedType(const std::uint64_t index, const char value)
        {
            if (index >= 3)
            {
                throw std::runtime_error("index out of range for mDFeedType [E105]");
            }

            *((char *)(m_buffer + m_offset + 0 + (index * 1))) = (value);
        }

        std::uint64_t getMDFeedType(char *dst, const std::uint64_t length) const
        {
            if (length > 3)
            {
                 throw std::runtime_error("length too large for getMDFeedType [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 0, length);
            return length;
        }

        NoMDFeedTypes &putMDFeedType(const char *src)
        {
            std::memcpy(m_buffer + m_offset + 0, src, 3);
            return *this;
        }

        std::string getMDFeedTypeAsString() const
        {
            std::string result(m_buffer + m_offset + 0, 3);
            return result;
        }

        NoMDFeedTypes &putMDFeedType(const std::string& str)
        {
            std::memcpy(m_buffer + m_offset + 0, str.c_str(), 3);
            return *this;
        }


        static SBE_CONSTEXPR const std::uint16_t marketDepthId(void)
        {
            return 264;
        }

        static SBE_CONSTEXPR const std::uint64_t marketDepthSinceVersion(void)
        {
             return 0;
        }

        bool marketDepthInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= marketDepthSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MarketDepthMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int8_t marketDepthNullValue()
        {
            return SBE_NULLVALUE_INT8;
        }

        static SBE_CONSTEXPR const std::int8_t marketDepthMinValue()
        {
            return (std::int8_t)-127;
        }

        static SBE_CONSTEXPR const std::int8_t marketDepthMaxValue()
        {
            return (std::int8_t)127;
        }

        std::int8_t marketDepth(void) const
        {
            return (*((std::int8_t *)(m_buffer + m_offset + 3)));
        }

        NoMDFeedTypes &marketDepth(const std::int8_t value)
        {
            *((std::int8_t *)(m_buffer + m_offset + 3)) = (value);
            return *this;
        }
    };

private:
    NoMDFeedTypes m_noMDFeedTypes;

public:

    static SBE_CONSTEXPR const std::uint16_t NoMDFeedTypesId(void)
    {
        return 1141;
    }


    inline NoMDFeedTypes &noMDFeedTypes(void)
    {
        m_noMDFeedTypes.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDFeedTypes;
    }

    NoMDFeedTypes &noMDFeedTypesCount(const std::uint8_t count)
    {
        m_noMDFeedTypes.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDFeedTypes;
    }

    static SBE_CONSTEXPR const std::uint64_t noMDFeedTypesSinceVersion(void)
    {
         return 0;
    }

    bool noMDFeedTypesInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noMDFeedTypesSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoInstAttrib
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
            m_dimensions.blockLength((std::uint16_t)4);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 4;
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
            return 4;
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

        inline NoInstAttrib &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t instAttribTypeId(void)
        {
            return 871;
        }

        static SBE_CONSTEXPR const std::uint64_t instAttribTypeSinceVersion(void)
        {
             return 0;
        }

        bool instAttribTypeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= instAttribTypeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *InstAttribTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int8_t instAttribTypeNullValue()
        {
            return SBE_NULLVALUE_INT8;
        }

        static SBE_CONSTEXPR const std::int8_t instAttribTypeMinValue()
        {
            return (std::int8_t)-127;
        }

        static SBE_CONSTEXPR const std::int8_t instAttribTypeMaxValue()
        {
            return (std::int8_t)127;
        }

        static SBE_CONSTEXPR const std::int8_t instAttribType(void)
        {
            return (std::int8_t)24;
        }

        static SBE_CONSTEXPR const std::uint16_t instAttribValueId(void)
        {
            return 872;
        }

        static SBE_CONSTEXPR const std::uint64_t instAttribValueSinceVersion(void)
        {
             return 0;
        }

        bool instAttribValueInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= instAttribValueSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *InstAttribValueMetaAttribute(const MetaAttribute::Attribute metaAttribute)
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
        InstAttribValue m_instAttribValue;

    public:

        InstAttribValue &instAttribValue()
        {
            m_instAttribValue.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
            return m_instAttribValue;
        }
    };

private:
    NoInstAttrib m_noInstAttrib;

public:

    static SBE_CONSTEXPR const std::uint16_t NoInstAttribId(void)
    {
        return 870;
    }


    inline NoInstAttrib &noInstAttrib(void)
    {
        m_noInstAttrib.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noInstAttrib;
    }

    NoInstAttrib &noInstAttribCount(const std::uint8_t count)
    {
        m_noInstAttrib.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noInstAttrib;
    }

    static SBE_CONSTEXPR const std::uint64_t noInstAttribSinceVersion(void)
    {
         return 0;
    }

    bool noInstAttribInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noInstAttribSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoLotTypeRules
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
            m_dimensions.blockLength((std::uint16_t)5);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 5;
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
            return 5;
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

        inline NoLotTypeRules &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t lotTypeId(void)
        {
            return 1093;
        }

        static SBE_CONSTEXPR const std::uint64_t lotTypeSinceVersion(void)
        {
             return 0;
        }

        bool lotTypeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= lotTypeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *LotTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int8_t lotTypeNullValue()
        {
            return SBE_NULLVALUE_INT8;
        }

        static SBE_CONSTEXPR const std::int8_t lotTypeMinValue()
        {
            return (std::int8_t)-127;
        }

        static SBE_CONSTEXPR const std::int8_t lotTypeMaxValue()
        {
            return (std::int8_t)127;
        }

        std::int8_t lotType(void) const
        {
            return (*((std::int8_t *)(m_buffer + m_offset + 0)));
        }

        NoLotTypeRules &lotType(const std::int8_t value)
        {
            *((std::int8_t *)(m_buffer + m_offset + 0)) = (value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t minLotSizeId(void)
        {
            return 1231;
        }

        static SBE_CONSTEXPR const std::uint64_t minLotSizeSinceVersion(void)
        {
             return 0;
        }

        bool minLotSizeInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= minLotSizeSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *MinLotSizeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            }

            return "";
        }

private:
        DecimalQty m_minLotSize;

public:

        DecimalQty &minLotSize(void)
        {
            m_minLotSize.wrap(m_buffer, m_offset + 1, m_actingVersion, m_bufferLength);
            return m_minLotSize;
        }
    };

private:
    NoLotTypeRules m_noLotTypeRules;

public:

    static SBE_CONSTEXPR const std::uint16_t NoLotTypeRulesId(void)
    {
        return 1234;
    }


    inline NoLotTypeRules &noLotTypeRules(void)
    {
        m_noLotTypeRules.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noLotTypeRules;
    }

    NoLotTypeRules &noLotTypeRulesCount(const std::uint8_t count)
    {
        m_noLotTypeRules.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noLotTypeRules;
    }

    static SBE_CONSTEXPR const std::uint64_t noLotTypeRulesSinceVersion(void)
    {
         return 0;
    }

    bool noLotTypeRulesInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noLotTypeRulesSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoUnderlyings
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
            m_dimensions.blockLength((std::uint16_t)24);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 24;
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

        inline NoUnderlyings &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t underlyingSecurityIDId(void)
        {
            return 309;
        }

        static SBE_CONSTEXPR const std::uint64_t underlyingSecurityIDSinceVersion(void)
        {
             return 0;
        }

        bool underlyingSecurityIDInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= underlyingSecurityIDSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *UnderlyingSecurityIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t underlyingSecurityIDNullValue()
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR const std::int32_t underlyingSecurityIDMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t underlyingSecurityIDMaxValue()
        {
            return 2147483647;
        }

        std::int32_t underlyingSecurityID(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 0)));
        }

        NoUnderlyings &underlyingSecurityID(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 0)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t underlyingSecurityIDSourceId(void)
        {
            return 305;
        }

        static SBE_CONSTEXPR const std::uint64_t underlyingSecurityIDSourceSinceVersion(void)
        {
             return 0;
        }

        bool underlyingSecurityIDSourceInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= underlyingSecurityIDSourceSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *UnderlyingSecurityIDSourceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "char";
            }

            return "";
        }

        static SBE_CONSTEXPR const char underlyingSecurityIDSourceNullValue()
        {
            return (char)0;
        }

        static SBE_CONSTEXPR const char underlyingSecurityIDSourceMinValue()
        {
            return (char)32;
        }

        static SBE_CONSTEXPR const char underlyingSecurityIDSourceMaxValue()
        {
            return (char)126;
        }

        static SBE_CONSTEXPR const std::uint64_t underlyingSecurityIDSourceLength(void)
        {
            return 1;
        }

        const char *underlyingSecurityIDSource(void) const
        {
            static sbe_uint8_t underlyingSecurityIDSourceValues[] = {56};

            return (const char *)underlyingSecurityIDSourceValues;
        }

        char underlyingSecurityIDSource(const std::uint64_t index) const
        {
            static sbe_uint8_t underlyingSecurityIDSourceValues[] = {56};

            return underlyingSecurityIDSourceValues[index];
        }

        std::uint64_t getUnderlyingSecurityIDSource(char *dst, const std::uint64_t length) const
        {
            static sbe_uint8_t underlyingSecurityIDSourceValues[] = {56};
            std::uint64_t bytesToCopy = (length < sizeof(underlyingSecurityIDSourceValues)) ? length : sizeof(underlyingSecurityIDSourceValues);

            std::memcpy(dst, underlyingSecurityIDSourceValues, bytesToCopy);
            return bytesToCopy;
        }

        static SBE_CONSTEXPR const std::uint16_t underlyingSymbolId(void)
        {
            return 311;
        }

        static SBE_CONSTEXPR const std::uint64_t underlyingSymbolSinceVersion(void)
        {
             return 0;
        }

        bool underlyingSymbolInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= underlyingSymbolSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *UnderlyingSymbolMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "String";
            }

            return "";
        }

        static SBE_CONSTEXPR const char underlyingSymbolNullValue()
        {
            return (char)0;
        }

        static SBE_CONSTEXPR const char underlyingSymbolMinValue()
        {
            return (char)32;
        }

        static SBE_CONSTEXPR const char underlyingSymbolMaxValue()
        {
            return (char)126;
        }

        static SBE_CONSTEXPR const std::uint64_t underlyingSymbolLength(void)
        {
            return 20;
        }

        const char *underlyingSymbol(void) const
        {
            return (m_buffer + m_offset + 4);
        }

        char underlyingSymbol(const std::uint64_t index) const
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for underlyingSymbol [E104]");
            }

            return (*((char *)(m_buffer + m_offset + 4 + (index * 1))));
        }

        void underlyingSymbol(const std::uint64_t index, const char value)
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for underlyingSymbol [E105]");
            }

            *((char *)(m_buffer + m_offset + 4 + (index * 1))) = (value);
        }

        std::uint64_t getUnderlyingSymbol(char *dst, const std::uint64_t length) const
        {
            if (length > 20)
            {
                 throw std::runtime_error("length too large for getUnderlyingSymbol [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 4, length);
            return length;
        }

        NoUnderlyings &putUnderlyingSymbol(const char *src)
        {
            std::memcpy(m_buffer + m_offset + 4, src, 20);
            return *this;
        }

        std::string getUnderlyingSymbolAsString() const
        {
            std::string result(m_buffer + m_offset + 4, 20);
            return result;
        }

        NoUnderlyings &putUnderlyingSymbol(const std::string& str)
        {
            std::memcpy(m_buffer + m_offset + 4, str.c_str(), 20);
            return *this;
        }

    };

private:
    NoUnderlyings m_noUnderlyings;

public:

    static SBE_CONSTEXPR const std::uint16_t NoUnderlyingsId(void)
    {
        return 711;
    }


    inline NoUnderlyings &noUnderlyings(void)
    {
        m_noUnderlyings.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noUnderlyings;
    }

    NoUnderlyings &noUnderlyingsCount(const std::uint8_t count)
    {
        m_noUnderlyings.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noUnderlyings;
    }

    static SBE_CONSTEXPR const std::uint64_t noUnderlyingsSinceVersion(void)
    {
         return 0;
    }

    bool noUnderlyingsInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noUnderlyingsSinceVersion();
#pragma GCC diagnostic pop
    }

    class NoRelatedInstruments
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
            m_dimensions.blockLength((std::uint16_t)24);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 24;
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

        inline NoRelatedInstruments &next(void)
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


        static SBE_CONSTEXPR const std::uint16_t relatedSecurityIDId(void)
        {
            return 1650;
        }

        static SBE_CONSTEXPR const std::uint64_t relatedSecurityIDSinceVersion(void)
        {
             return 0;
        }

        bool relatedSecurityIDInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= relatedSecurityIDSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *RelatedSecurityIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static SBE_CONSTEXPR const std::int32_t relatedSecurityIDNullValue()
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR const std::int32_t relatedSecurityIDMinValue()
        {
            return -2147483647;
        }

        static SBE_CONSTEXPR const std::int32_t relatedSecurityIDMaxValue()
        {
            return 2147483647;
        }

        std::int32_t relatedSecurityID(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 0)));
        }

        NoRelatedInstruments &relatedSecurityID(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 0)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static SBE_CONSTEXPR const std::uint16_t relatedSecurityIDSourceId(void)
        {
            return 1651;
        }

        static SBE_CONSTEXPR const std::uint64_t relatedSecurityIDSourceSinceVersion(void)
        {
             return 0;
        }

        bool relatedSecurityIDSourceInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= relatedSecurityIDSourceSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *RelatedSecurityIDSourceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "char";
            }

            return "";
        }

        static SBE_CONSTEXPR const char relatedSecurityIDSourceNullValue()
        {
            return (char)0;
        }

        static SBE_CONSTEXPR const char relatedSecurityIDSourceMinValue()
        {
            return (char)32;
        }

        static SBE_CONSTEXPR const char relatedSecurityIDSourceMaxValue()
        {
            return (char)126;
        }

        static SBE_CONSTEXPR const std::uint64_t relatedSecurityIDSourceLength(void)
        {
            return 1;
        }

        const char *relatedSecurityIDSource(void) const
        {
            static sbe_uint8_t relatedSecurityIDSourceValues[] = {56};

            return (const char *)relatedSecurityIDSourceValues;
        }

        char relatedSecurityIDSource(const std::uint64_t index) const
        {
            static sbe_uint8_t relatedSecurityIDSourceValues[] = {56};

            return relatedSecurityIDSourceValues[index];
        }

        std::uint64_t getRelatedSecurityIDSource(char *dst, const std::uint64_t length) const
        {
            static sbe_uint8_t relatedSecurityIDSourceValues[] = {56};
            std::uint64_t bytesToCopy = (length < sizeof(relatedSecurityIDSourceValues)) ? length : sizeof(relatedSecurityIDSourceValues);

            std::memcpy(dst, relatedSecurityIDSourceValues, bytesToCopy);
            return bytesToCopy;
        }

        static SBE_CONSTEXPR const std::uint16_t relatedSymbolId(void)
        {
            return 1649;
        }

        static SBE_CONSTEXPR const std::uint64_t relatedSymbolSinceVersion(void)
        {
             return 0;
        }

        bool relatedSymbolInActingVersion(void)
        {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
            return m_actingVersion >= relatedSymbolSinceVersion();
    #pragma GCC diagnostic pop
        }


        static const char *RelatedSymbolMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "String";
            }

            return "";
        }

        static SBE_CONSTEXPR const char relatedSymbolNullValue()
        {
            return (char)0;
        }

        static SBE_CONSTEXPR const char relatedSymbolMinValue()
        {
            return (char)32;
        }

        static SBE_CONSTEXPR const char relatedSymbolMaxValue()
        {
            return (char)126;
        }

        static SBE_CONSTEXPR const std::uint64_t relatedSymbolLength(void)
        {
            return 20;
        }

        const char *relatedSymbol(void) const
        {
            return (m_buffer + m_offset + 4);
        }

        char relatedSymbol(const std::uint64_t index) const
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for relatedSymbol [E104]");
            }

            return (*((char *)(m_buffer + m_offset + 4 + (index * 1))));
        }

        void relatedSymbol(const std::uint64_t index, const char value)
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for relatedSymbol [E105]");
            }

            *((char *)(m_buffer + m_offset + 4 + (index * 1))) = (value);
        }

        std::uint64_t getRelatedSymbol(char *dst, const std::uint64_t length) const
        {
            if (length > 20)
            {
                 throw std::runtime_error("length too large for getRelatedSymbol [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 4, length);
            return length;
        }

        NoRelatedInstruments &putRelatedSymbol(const char *src)
        {
            std::memcpy(m_buffer + m_offset + 4, src, 20);
            return *this;
        }

        std::string getRelatedSymbolAsString() const
        {
            std::string result(m_buffer + m_offset + 4, 20);
            return result;
        }

        NoRelatedInstruments &putRelatedSymbol(const std::string& str)
        {
            std::memcpy(m_buffer + m_offset + 4, str.c_str(), 20);
            return *this;
        }

    };

private:
    NoRelatedInstruments m_noRelatedInstruments;

public:

    static SBE_CONSTEXPR const std::uint16_t NoRelatedInstrumentsId(void)
    {
        return 1647;
    }


    inline NoRelatedInstruments &noRelatedInstruments(void)
    {
        m_noRelatedInstruments.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noRelatedInstruments;
    }

    NoRelatedInstruments &noRelatedInstrumentsCount(const std::uint8_t count)
    {
        m_noRelatedInstruments.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noRelatedInstruments;
    }

    static SBE_CONSTEXPR const std::uint64_t noRelatedInstrumentsSinceVersion(void)
    {
         return 7;
    }

    bool noRelatedInstrumentsInActingVersion(void)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
        return m_actingVersion >= noRelatedInstrumentsSinceVersion();
#pragma GCC diagnostic pop
    }
};
}
#endif
