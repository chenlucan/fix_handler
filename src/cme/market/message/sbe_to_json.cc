
#include "cme/market/message/sbe_to_json.h"
#include "core/assist/utility.h"
#include <bsoncxx/json.hpp>

#define PUT_TO_JSON_VALUE(json, key, value)  json.append(bsoncxx::builder::basic::kvp(key, T(value)))
#define PUT_TO_JSON_CHILD(json, key, child) json.append(bsoncxx::builder::basic::kvp(key, child))
#define PUT_TO_JSON_ARRAY_ITEM(json, item) json.append(item);
#define JSON_TO_STRING(json)  bsoncxx::to_json(json.view());

#define PUT_TO_JSON(json, message, field) PUT_TO_JSON_VALUE(json, #field, (message).field())
#define PUT_TO_JSON_CHAR(json, message, field)  PUT_TO_JSON_VALUE(json, #field, (message).field(0))
#define PUT_TO_JSON_CHAR_ENUM(json, message, field)  PUT_TO_JSON_VALUE(json, #field, (char)(message).field())
#define PUT_TO_JSON_NON_NULL(json, message, field)  {  auto v = (message).field(); if(v != (message).field##NullValue()) PUT_TO_JSON_VALUE(json, #field, v);  }
#define PUT_TO_JSON_STRING(json, message, field) PUT_TO_JSON_VALUE(json, #field, (message).get##field##AsString())
#define PUT_TO_JSON_PRICE(json, message, field)  { JSON_OBJ_TYPE price; \
                                                                                                PUT_TO_JSON_VALUE(price, "mantissa", (message).field().mantissa()); \
                                                                                                PUT_TO_JSON_VALUE(price, "exponent", (message).field().exponent()); \
                                                                                                PUT_TO_JSON_CHILD(json, #field, price); }
#define PUT_TO_JSON_PRICE_NON_NULL(json, message, field)  { auto v =  (message).field().mantissa(); \
                                                                                                                      if(v != (message).field().mantissaNullValue()) { \
                                                                                                                          JSON_OBJ_TYPE price; \
                                                                                                                          PUT_TO_JSON_VALUE(price, "mantissa", v); \
                                                                                                                          PUT_TO_JSON_VALUE(price, "exponent", (message).field().exponent()); \
                                                                                                                          PUT_TO_JSON_CHILD(json, #field, price); } }
#define PUT_TO_JSON_MATCH_EVENT(json, message, field)  { JSON_OBJ_TYPE event; auto f = (message).field(); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "lastTradeMsg", f.lastTradeMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "lastVolumeMsg", f.lastVolumeMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "lastQuoteMsg", f.lastQuoteMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "lastStatsMsg", f.lastStatsMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "lastImpliedMsg", f.lastImpliedMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "recoveryMsg", f.recoveryMsg()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "reserved", f.reserved()); \
                                                                                                                 PUT_TO_JSON_VALUE(event, "endOfEvent", f.endOfEvent()); \
                                                                                                                 PUT_TO_JSON_CHILD(json, #field, event); }
#define PUT_TO_JSON_DATE(json, message, field)  { JSON_OBJ_TYPE date; auto f = (message).field(); \
                                                                                                PUT_TO_JSON_NON_NULL(date, f, year); \
                                                                                                PUT_TO_JSON_NON_NULL(date, f, month); \
                                                                                                PUT_TO_JSON_NON_NULL(date, f, day); \
                                                                                                PUT_TO_JSON_NON_NULL(date, f, week); \
                                                                                                PUT_TO_JSON_CHILD(json, #field, date); }
#define PUT_TO_JSON_SETTL_PRICE(json, message, field)  { JSON_OBJ_TYPE settl; auto f = (message).field(); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "finalrc", f.finalrc()); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "actual", f.actual()); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "rounded", f.rounded()); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "intraday", f.intraday()); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "reservedBits", f.reservedBits()); \
                                                                                                                 PUT_TO_JSON_VALUE(settl, "nullValue", f.nullValue()); \
                                                                                                                 PUT_TO_JSON_CHILD(json, #field, settl); }
#define PUT_TO_JSON_INST_ATTR(json, message, field)  { JSON_OBJ_TYPE ia; auto f = (message).field();\
                                                                                                         PUT_TO_JSON_VALUE(ia, "electronicMatchEligible", f.electronicMatchEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "orderCrossEligible", f.orderCrossEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "blockTradeEligible", f.blockTradeEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "eFPEligible", f.eFPEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "eBFEligible", f.eBFEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "eFSEligible", f.eFSEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "eFREligible", f.eFREligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "oTCEligible", f.oTCEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "iLinkIndicativeMassQuotingEligible", f.iLinkIndicativeMassQuotingEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "negativeStrikeEligible", f.negativeStrikeEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "negativePriceOutrightEligible", f.negativePriceOutrightEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "isFractional", f.isFractional()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "volatilityQuotedOption", f.volatilityQuotedOption()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "rFQCrossEligible", f.rFQCrossEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "zeroPriceOutrightEligible", f.zeroPriceOutrightEligible()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "decayingProductEligibility", f.decayingProductEligibility()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "variableProductEligibility", f.variableProductEligibility()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "dailyProductEligibility", f.dailyProductEligibility()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "gTOrdersEligibility", f.gTOrdersEligibility()); \
                                                                                                         PUT_TO_JSON_VALUE(ia, "impliedMatchingEligibility", f.impliedMatchingEligibility()); \
                                                                                                         PUT_TO_JSON_CHILD(json, #field, ia); }


namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    template <typename IntType>
    inline std::string T(IntType v){return std::to_string(v);}
    inline std::string T(char v){return std::string(1, v);}
    inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
    inline std::string T(const char *v){return std::string(v);}

    SBEtoJSON::SBEtoJSON(const MdpMessage *sbe_message) : m_sbe_message(sbe_message)
    {
        // noop
    }

    SBEtoJSON::~SBEtoJSON()
    {
        // noop
    }

    std::string SBEtoJSON::To_json()
    {
        void *body = m_sbe_message->message_body().get();
        switch(m_sbe_message->template_id())
        {
            case 4:    // ChannelReset4: X
                return this->To_json((mktdata::ChannelReset4 *)body);
            case 12:    // AdminHeartbeat12: 0      -- 无需保存
                return  "";
            case 15:    // AdminLogin15: A      -- 不会收到这种类型
                return "";
            case 16:    // AdminLogout16: 5      -- 不会收到这种类型
                return "";
            case 27:    // MDInstrumentDefinitionFuture27: d
                return this->To_json((mktdata::MDInstrumentDefinitionFuture27 *)body);
            case 29:    // MDInstrumentDefinitionSpread29: d
                return this->To_json((mktdata::MDInstrumentDefinitionSpread29 *)body);
            case 30:    // SecurityStatus30: f
                return this->To_json((mktdata::SecurityStatus30 *)body);
            case 32:    // MDIncrementalRefreshBook32: X
                return this->To_json((mktdata::MDIncrementalRefreshBook32 *)body);
            case 33:    // MDIncrementalRefreshDailyStatistics33: X
                return this->To_json((mktdata::MDIncrementalRefreshDailyStatistics33 *)body);
            case 34:    // MDIncrementalRefreshLimitsBanding34: X
                return this->To_json((mktdata::MDIncrementalRefreshLimitsBanding34 *)body);
            case 35:    // MDIncrementalRefreshSessionStatistics35: X
                return this->To_json((mktdata::MDIncrementalRefreshSessionStatistics35 *)body);
            case 36:    // MDIncrementalRefreshTrade36: X
                return this->To_json((mktdata::MDIncrementalRefreshTrade36 *)body);
            case 37:    // MDIncrementalRefreshVolume37: X
                return this->To_json((mktdata::MDIncrementalRefreshVolume37 *)body);
            case 38:    // SnapshotFullRefresh38: W
                return this->To_json((mktdata::SnapshotFullRefresh38 *)body);
            case 39:    // QuoteRequest39: R
                return this->To_json((mktdata::QuoteRequest39 *)body);
            case 41:    // MDInstrumentDefinitionOption41: d
                return this->To_json((mktdata::MDInstrumentDefinitionOption41 *)body);
            case 42:    // MDIncrementalRefreshTradeSummary42: X
                return this->To_json((mktdata::MDIncrementalRefreshTradeSummary42 *)body);
            case 43:    // MDIncrementalRefreshOrderBook43: X
                return this->To_json((mktdata::MDIncrementalRefreshOrderBook43 *)body);
            case 44:    // SnapshotFullRefreshOrderBook44: W
                return this->To_json((mktdata::SnapshotFullRefreshOrderBook44 *)body);
            default:    // 没有其他类型了
                return "";
        }
    }

    std::string SBEtoJSON::To_json(mktdata::ChannelReset4 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "ChannelReset4");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::ChannelReset4::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON(entity, noMDEntries, applID);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionFuture27 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDInstrumentDefinitionFuture27");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON_CHAR_ENUM(json, *m, securityUpdateAction);
        PUT_TO_JSON(json, *m, lastUpdateTime);
        PUT_TO_JSON(json, *m, mDSecurityTradingStatus);
        PUT_TO_JSON(json, *m, applID);
        PUT_TO_JSON(json, *m, marketSegmentID);
        PUT_TO_JSON(json, *m, underlyingProduct);
        PUT_TO_JSON_STRING(json, *m, SecurityExchange);
        PUT_TO_JSON_STRING(json, *m, SecurityGroup);
        PUT_TO_JSON_STRING(json, *m, Asset);
        PUT_TO_JSON_STRING(json, *m, Symbol);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON_CHAR(json, *m, securityIDSource);
        PUT_TO_JSON_STRING(json, *m, SecurityType);
        PUT_TO_JSON_STRING(json, *m, CFICode);
        PUT_TO_JSON_DATE(json, *m, maturityMonthYear);
        PUT_TO_JSON_STRING(json, *m, Currency);
        PUT_TO_JSON_STRING(json, *m, SettlCurrency);
        PUT_TO_JSON(json, *m, matchAlgorithm);
        PUT_TO_JSON(json, *m, minTradeVol);
        PUT_TO_JSON(json, *m, maxTradeVol);
        PUT_TO_JSON_PRICE(json, *m, minPriceIncrement);
        PUT_TO_JSON_PRICE(json, *m, displayFactor);
        PUT_TO_JSON_STRING(json, *m, UnitOfMeasure);
        PUT_TO_JSON_SETTL_PRICE(json, *m, settlPriceType);
        PUT_TO_JSON_PRICE(json, *m, maxPriceVariation);
        PUT_TO_JSON(json, *m, decayStartDate);
        PUT_TO_JSON(json, *m, userDefinedInstrument);
        PUT_TO_JSON(json, *m, tradingReferenceDate);
        PUT_TO_JSON_NON_NULL(json, *m, openInterestQty);
        PUT_TO_JSON_NON_NULL(json, *m, clearedVolume);
        PUT_TO_JSON_NON_NULL(json, *m, decayQuantity);
        PUT_TO_JSON_NON_NULL(json, *m, originalContractSize);
        PUT_TO_JSON_NON_NULL(json, *m, contractMultiplier);
        PUT_TO_JSON_NON_NULL(json, *m, totNumReports);
        PUT_TO_JSON_NON_NULL(json, *m, contractMultiplierUnit);
        PUT_TO_JSON_NON_NULL(json, *m, flowScheduleType);
        PUT_TO_JSON_NON_NULL(json, *m, mainFraction);
        PUT_TO_JSON_NON_NULL(json, *m, subFraction);
        PUT_TO_JSON_NON_NULL(json, *m, priceDisplayFormat);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, unitOfMeasureQty);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, tradingReferencePrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, highLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, lowLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minPriceIncrementAmount);

        JSON_ARRAY_TYPE events;
        mktdata::MDInstrumentDefinitionFuture27::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            JSON_OBJ_TYPE event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            PUT_TO_JSON_ARRAY_ITEM(events, event);
        }
        PUT_TO_JSON_CHILD(json, "noEvents", events);

        JSON_ARRAY_TYPE feedTypes;
        mktdata::MDInstrumentDefinitionFuture27::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            JSON_OBJ_TYPE feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            PUT_TO_JSON_ARRAY_ITEM(feedTypes, feedType);
        }
        PUT_TO_JSON_CHILD(json, "noMDFeedTypes", feedTypes);

        JSON_ARRAY_TYPE attrs;
        mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            JSON_OBJ_TYPE attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            PUT_TO_JSON_ARRAY_ITEM(attrs, attr);
        }

        PUT_TO_JSON_CHILD(json, "noInstAttrib", attrs);

        JSON_ARRAY_TYPE rules;
        mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            JSON_OBJ_TYPE rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            PUT_TO_JSON_ARRAY_ITEM(rules, rule);
        }
        PUT_TO_JSON_CHILD(json, "noLotTypeRules", rules);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionSpread29 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDInstrumentDefinitionSpread29");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON_CHAR_ENUM(json, *m, securityUpdateAction);
        PUT_TO_JSON(json, *m, lastUpdateTime);
        PUT_TO_JSON(json, *m, mDSecurityTradingStatus);
        PUT_TO_JSON(json, *m, applID);
        PUT_TO_JSON(json, *m, marketSegmentID);
        PUT_TO_JSON_STRING(json, *m, SecurityExchange);
        PUT_TO_JSON_STRING(json, *m, SecurityGroup);
        PUT_TO_JSON_STRING(json, *m, Asset);
        PUT_TO_JSON_STRING(json, *m, Symbol);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON_CHAR(json, *m, securityIDSource);
        PUT_TO_JSON_STRING(json, *m, SecurityType);
        PUT_TO_JSON_STRING(json, *m, CFICode);
        PUT_TO_JSON_DATE(json, *m, maturityMonthYear);
        PUT_TO_JSON_STRING(json, *m, Currency);
        PUT_TO_JSON_STRING(json, *m, SecuritySubType);
        PUT_TO_JSON(json, *m, matchAlgorithm);
        PUT_TO_JSON(json, *m, minTradeVol);
        PUT_TO_JSON(json, *m, maxTradeVol);
        PUT_TO_JSON_PRICE(json, *m, minPriceIncrement);
        PUT_TO_JSON_PRICE(json, *m, displayFactor);
        PUT_TO_JSON_STRING(json, *m, UnitOfMeasure);
        PUT_TO_JSON_SETTL_PRICE(json, *m, settlPriceType);
        PUT_TO_JSON(json, *m, userDefinedInstrument);
        PUT_TO_JSON(json, *m, tradingReferenceDate);
        PUT_TO_JSON_NON_NULL(json, *m, openInterestQty);
        PUT_TO_JSON_NON_NULL(json, *m, clearedVolume);
        PUT_TO_JSON_NON_NULL(json, *m, totNumReports);
        PUT_TO_JSON_NON_NULL(json, *m, tickRule);
        PUT_TO_JSON_NON_NULL(json, *m, mainFraction);
        PUT_TO_JSON_NON_NULL(json, *m, subFraction);
        PUT_TO_JSON_NON_NULL(json, *m, priceDisplayFormat);
        PUT_TO_JSON_NON_NULL(json, *m, underlyingProduct);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, priceRatio);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, tradingReferencePrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, highLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, lowLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, maxPriceVariation);

        JSON_ARRAY_TYPE events;
        mktdata::MDInstrumentDefinitionSpread29::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            JSON_OBJ_TYPE event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            PUT_TO_JSON_ARRAY_ITEM(events, event);
        }
        PUT_TO_JSON_CHILD(json, "noEvents", events);

        JSON_ARRAY_TYPE feedTypes;
        mktdata::MDInstrumentDefinitionSpread29::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            JSON_OBJ_TYPE feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            PUT_TO_JSON_ARRAY_ITEM(feedTypes, feedType);
        }
        PUT_TO_JSON_CHILD(json, "noMDFeedTypes", feedTypes);

        JSON_ARRAY_TYPE attrs;
        mktdata::MDInstrumentDefinitionSpread29::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            JSON_OBJ_TYPE attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            PUT_TO_JSON_ARRAY_ITEM(attrs, attr);
        }
        PUT_TO_JSON_CHILD(json, "noInstAttrib", attrs);

        JSON_ARRAY_TYPE rules;
        mktdata::MDInstrumentDefinitionSpread29::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            JSON_OBJ_TYPE rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            PUT_TO_JSON_ARRAY_ITEM(rules, rule);
        }
        PUT_TO_JSON_CHILD(json, "noLotTypeRules", rules);

        JSON_ARRAY_TYPE legs;
        mktdata::MDInstrumentDefinitionSpread29::NoLegs& noLegs = m->noLegs();
        while (noLegs.hasNext())
        {
            noLegs.next();

            JSON_OBJ_TYPE leg;
            PUT_TO_JSON(leg, noLegs, legSecurityID);
            PUT_TO_JSON_CHAR(leg, noLegs, legSecurityIDSource);
            PUT_TO_JSON(leg, noLegs, legSide);
            PUT_TO_JSON(leg, noLegs, legRatioQty);
            PUT_TO_JSON_PRICE(leg, noLegs, legOptionDelta);
            PUT_TO_JSON_PRICE_NON_NULL(leg, noLegs, legPrice);

            PUT_TO_JSON_ARRAY_ITEM(legs, leg);
        }
        PUT_TO_JSON_CHILD(json, "noLegs", legs);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SecurityStatus30 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "SecurityStatus30");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_STRING(json, *m, SecurityGroup);
        PUT_TO_JSON_STRING(json, *m, Asset);
        PUT_TO_JSON(json, *m, tradeDate);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON(json, *m, securityTradingStatus);
        PUT_TO_JSON(json, *m, haltReason);
        PUT_TO_JSON(json, *m, securityTradingEvent);
        PUT_TO_JSON_NON_NULL(json, *m, securityID);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshBook32 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshBook32");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDPriceLevel);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        JSON_ARRAY_TYPE orderIds;
        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();

            JSON_OBJ_TYPE orderId;
            PUT_TO_JSON(orderId, noOrderIDEntries, orderID);
            PUT_TO_JSON(orderId, noOrderIDEntries, orderUpdateAction);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, mDDisplayQty);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, referenceID);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, mDOrderPriority);

            PUT_TO_JSON_ARRAY_ITEM(orderIds, orderId);
        }
        PUT_TO_JSON_CHILD(json, "noOrderIDEntries", orderIds);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshDailyStatistics33 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshDailyStatistics33");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, tradingReferenceDate);
            PUT_TO_JSON_SETTL_PRICE(entity, noMDEntries, settlPriceType);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshLimitsBanding34 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshLimitsBanding34");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, highLimitPrice);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, lowLimitPrice);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, maxPriceVariation);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshSessionStatistics35 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshSessionStatistics35");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, openCloseSettlFlag);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshTrade36 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshTrade36");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshTrade36::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, tradeID);
            PUT_TO_JSON(entity, noMDEntries, aggressorSide);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshVolume37 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshVolume37");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SnapshotFullRefresh38 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "SnapshotFullRefresh38");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON(json, *m, lastMsgSeqNumProcessed);
        PUT_TO_JSON(json, *m, totNumReports);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON(json, *m, rptSeq);
        PUT_TO_JSON(json, *m, lastUpdateTime);
        PUT_TO_JSON(json, *m, tradeDate);
        PUT_TO_JSON(json, *m, mDSecurityTradingStatus);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, highLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, lowLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, maxPriceVariation);

        JSON_ARRAY_TYPE entities;
        mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, tradingReferenceDate);
            PUT_TO_JSON(entity, noMDEntries, openCloseSettlFlag);
            PUT_TO_JSON_SETTL_PRICE(entity, noMDEntries, settlPriceType);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDPriceLevel);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::QuoteRequest39 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "QuoteRequest39");
        PUT_TO_JSON(json, *m, quoteReqID);
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE syms;
        mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym = m->noRelatedSym();
        while (noRelatedSym.hasNext())
        {
            noRelatedSym.next();

            JSON_OBJ_TYPE sym;
            PUT_TO_JSON_STRING(sym, noRelatedSym, Symbol);
            PUT_TO_JSON(sym, noRelatedSym, securityID);
            PUT_TO_JSON(sym, noRelatedSym, quoteType);
            PUT_TO_JSON_NON_NULL(sym, noRelatedSym, orderQty);
            PUT_TO_JSON_NON_NULL(sym, noRelatedSym, side);

            PUT_TO_JSON_ARRAY_ITEM(syms, sym);
        }
        PUT_TO_JSON_CHILD(json, "noRelatedSym", syms);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionOption41 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDInstrumentDefinitionOption41");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON_CHAR_ENUM(json, *m, securityUpdateAction);
        PUT_TO_JSON(json, *m, lastUpdateTime);
        PUT_TO_JSON(json, *m, mDSecurityTradingStatus);
        PUT_TO_JSON(json, *m, applID);
        PUT_TO_JSON(json, *m, marketSegmentID);
        PUT_TO_JSON(json, *m, underlyingProduct);
        PUT_TO_JSON_STRING(json, *m, SecurityExchange);
        PUT_TO_JSON_STRING(json, *m, SecurityGroup);
        PUT_TO_JSON_STRING(json, *m, Asset);
        PUT_TO_JSON_STRING(json, *m, Symbol);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON_CHAR(json, *m, securityIDSource);
        PUT_TO_JSON_STRING(json, *m, SecurityType);
        PUT_TO_JSON_STRING(json, *m, CFICode);
        PUT_TO_JSON(json, *m, putOrCall);
        PUT_TO_JSON_DATE(json, *m, maturityMonthYear);
        PUT_TO_JSON_STRING(json, *m, Currency);
        PUT_TO_JSON_STRING(json, *m, StrikeCurrency);
        PUT_TO_JSON_STRING(json, *m, SettlCurrency);
        PUT_TO_JSON(json, *m, matchAlgorithm);
        PUT_TO_JSON(json, *m, minTradeVol);
        PUT_TO_JSON(json, *m, maxTradeVol);
        PUT_TO_JSON_PRICE(json, *m, displayFactor);
        PUT_TO_JSON_STRING(json, *m, UnitOfMeasure);
        PUT_TO_JSON_SETTL_PRICE(json, *m, settlPriceType);
        PUT_TO_JSON(json, *m, userDefinedInstrument);
        PUT_TO_JSON(json, *m, tradingReferenceDate);
        PUT_TO_JSON_NON_NULL(json, *m, openInterestQty);
        PUT_TO_JSON_NON_NULL(json, *m, clearedVolume);
        PUT_TO_JSON_NON_NULL(json, *m, totNumReports);
        PUT_TO_JSON_NON_NULL(json, *m, tickRule);
        PUT_TO_JSON_NON_NULL(json, *m, mainFraction);
        PUT_TO_JSON_NON_NULL(json, *m, subFraction);
        PUT_TO_JSON_NON_NULL(json, *m, priceDisplayFormat);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, strikePrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minCabPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minPriceIncrement);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, unitOfMeasureQty);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, tradingReferencePrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, highLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, lowLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minPriceIncrementAmount);

        JSON_ARRAY_TYPE events;
        mktdata::MDInstrumentDefinitionOption41::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            JSON_OBJ_TYPE event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            PUT_TO_JSON_ARRAY_ITEM(events, event);
        }
        PUT_TO_JSON_CHILD(json, "noEvents", events);

        JSON_ARRAY_TYPE feedTypes;
        mktdata::MDInstrumentDefinitionOption41::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            JSON_OBJ_TYPE feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            PUT_TO_JSON_ARRAY_ITEM(feedTypes, feedType);
        }
        PUT_TO_JSON_CHILD(json, "noMDFeedTypes", feedTypes);

        JSON_ARRAY_TYPE attrs;
        mktdata::MDInstrumentDefinitionOption41::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            JSON_OBJ_TYPE attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            PUT_TO_JSON_ARRAY_ITEM(attrs, attr);
        }
        PUT_TO_JSON_CHILD(json, "noInstAttrib", attrs);

        JSON_ARRAY_TYPE rules;
        mktdata::MDInstrumentDefinitionOption41::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            JSON_OBJ_TYPE rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            PUT_TO_JSON_ARRAY_ITEM(rules, rule);
        }
        PUT_TO_JSON_CHILD(json, "noLotTypeRules", rules);

        JSON_ARRAY_TYPE underlyings;
        mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& noUnderlyings = m->noUnderlyings();
        while (noUnderlyings.hasNext())
        {
            noUnderlyings.next();

            JSON_OBJ_TYPE underlying;
            PUT_TO_JSON(underlying, noUnderlyings, underlyingSecurityID);
            PUT_TO_JSON_CHAR(underlying, noUnderlyings, underlyingSecurityIDSource);
            PUT_TO_JSON_STRING(underlying, noUnderlyings, UnderlyingSymbol);

            PUT_TO_JSON_ARRAY_ITEM(underlyings, underlying);
        }
        PUT_TO_JSON_CHILD(json, "noUnderlyings", underlyings);

        JSON_ARRAY_TYPE instuments;
        mktdata::MDInstrumentDefinitionOption41::NoRelatedInstruments& noRelatedInstruments = m->noRelatedInstruments();
        while (noRelatedInstruments.hasNext())
        {
            noRelatedInstruments.next();

            JSON_OBJ_TYPE instument;
            PUT_TO_JSON(instument, noRelatedInstruments, relatedSecurityID);
            PUT_TO_JSON_CHAR(instument, noRelatedInstruments, relatedSecurityIDSource);
            PUT_TO_JSON_STRING(instument, noRelatedInstruments, RelatedSymbol);

            PUT_TO_JSON_ARRAY_ITEM(instuments, instument);
        }
        PUT_TO_JSON_CHILD(json, "noRelatedInstruments", instuments);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshTradeSummary42 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshTradeSummary42");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, aggressorSide);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDTradeEntryID);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        JSON_ARRAY_TYPE orderIds;
        mktdata::MDIncrementalRefreshTradeSummary42::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();

            JSON_OBJ_TYPE orderId;
            PUT_TO_JSON(orderId, noOrderIDEntries, orderID);
            PUT_TO_JSON(orderId, noOrderIDEntries, lastQty);

            PUT_TO_JSON_ARRAY_ITEM(orderIds, orderId);
        }
        PUT_TO_JSON_CHILD(json, "noOrderIDEntries", orderIds);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshOrderBook43 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "MDIncrementalRefreshOrderBook43");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        JSON_ARRAY_TYPE entities;
        mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDDisplayQty);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDOrderPriority);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, orderID);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SnapshotFullRefreshOrderBook44 *m)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "type", "SnapshotFullRefreshOrderBook44");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON(json, *m, lastMsgSeqNumProcessed);
        PUT_TO_JSON(json, *m, totNumReports);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON(json, *m, noChunks);
        PUT_TO_JSON(json, *m, currentChunk);

        JSON_ARRAY_TYPE entities;
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            JSON_OBJ_TYPE entity;
            PUT_TO_JSON(entity, noMDEntries, orderID);
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDDisplayQty);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDOrderPriority);

            PUT_TO_JSON_ARRAY_ITEM(entities, entity);
        }
        PUT_TO_JSON_CHILD(json, "noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_string(const JSON_OBJ_TYPE &json_message)
    {
        JSON_OBJ_TYPE json;
        PUT_TO_JSON_VALUE(json, "market", "CME");
        PUT_TO_JSON_VALUE(json, "insertTime", std::to_string(fh::core::assist::utility::Current_time_ns()));
        PUT_TO_JSON_VALUE(json, "sendingTime", m_sbe_message->packet_sending_time());
        PUT_TO_JSON_VALUE(json, "sendingTimeStr",  fh::core::assist::utility::To_time_str(m_sbe_message->packet_sending_time()));
        PUT_TO_JSON_VALUE(json, "receivedTime", m_sbe_message->received_time());
        PUT_TO_JSON_VALUE(json, "packetSeqNum", m_sbe_message->packet_seq_num());
        PUT_TO_JSON_VALUE(json, "sbeType", m_sbe_message->message_type());
        PUT_TO_JSON_CHILD(json, "message", json_message);

        return JSON_TO_STRING(json);
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
