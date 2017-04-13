
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include "cme/market/message/sbe_to_json.h"

#define PUT_TO_JSON(json, message, field)  json.put(#field, (message).field())
#define PUT_TO_JSON_CHAR(json, message, field)  json.put(#field, (message).field(0))
#define PUT_TO_JSON_CHAR_ENUM(json, message, field)  json.put(#field, (char)(message).field())
#define PUT_TO_JSON_CONSTANT(json, key, value)  json.put(key, value)
#define PUT_TO_JSON_NON_NULL(json, message, field)  {  auto v = (message).field(); if(v != (message).field##NullValue()) json.put(#field, v);  }
#define PUT_TO_JSON_STRING(json, message, field)  json.put(#field, (message).get##field##AsString())
#define PUT_TO_JSON_PRICE(json, message, field)  { boost::property_tree::ptree price; \
                                                                                                price.put("mantissa", (message).field().mantissa()); \
                                                                                                price.put("exponent", (message).field().exponent()); \
                                                                                                json.put_child(#field, price); }
#define PUT_TO_JSON_PRICE_NON_NULL(json, message, field)  { auto v =  (message).field().mantissa(); \
                                                                                                                      if(v != (message).field().mantissaNullValue()) { \
                                                                                                                          boost::property_tree::ptree price; \
                                                                                                                          price.put("mantissa", v); \
                                                                                                                          price.put("exponent", (message).field().exponent()); \
                                                                                                                          json.put_child(#field, price); } }
#define PUT_TO_JSON_MATCH_EVENT(json, message, field)  { boost::property_tree::ptree event; \
                                                                                                                 event.put("lastTradeMsg", (message).field().lastTradeMsg()); \
                                                                                                                 event.put("lastVolumeMsg", (message).field().lastVolumeMsg()); \
                                                                                                                 event.put("lastQuoteMsg", (message).field().lastQuoteMsg()); \
                                                                                                                 event.put("lastStatsMsg", (message).field().lastStatsMsg()); \
                                                                                                                 event.put("lastImpliedMsg", (message).field().lastImpliedMsg()); \
                                                                                                                 event.put("recoveryMsg", (message).field().recoveryMsg()); \
                                                                                                                 event.put("reserved", (message).field().reserved()); \
                                                                                                                 event.put("endOfEvent", (message).field().endOfEvent()); \
                                                                                                                 json.put_child(#field, event); }
#define PUT_TO_JSON_DATE(json, message, field)  { boost::property_tree::ptree date; \
                                                                                                PUT_TO_JSON_NON_NULL(date, (message).field(), year); \
                                                                                                PUT_TO_JSON_NON_NULL(date, (message).field(), month); \
                                                                                                PUT_TO_JSON_NON_NULL(date, (message).field(), day); \
                                                                                                PUT_TO_JSON_NON_NULL(date, (message).field(), week); \
                                                                                                json.put_child(#field, date); }
#define PUT_TO_JSON_SETTL_PRICE(json, message, field)  { boost::property_tree::ptree settl; \
                                                                                                                 settl.put("finalrc", (message).field().finalrc()); \
                                                                                                                 settl.put("actual", (message).field().actual()); \
                                                                                                                 settl.put("rounded", (message).field().rounded()); \
                                                                                                                 settl.put("intraday", (message).field().intraday()); \
                                                                                                                 settl.put("reservedBits", (message).field().reservedBits()); \
                                                                                                                 settl.put("nullValue", (message).field().nullValue()); \
                                                                                                                 json.put_child(#field, settl); }
#define PUT_TO_JSON_INST_ATTR(json, message, field)  { boost::property_tree::ptree attr; \
                                                                                                         attr.put("electronicMatchEligible", (message).field().electronicMatchEligible()); \
                                                                                                         attr.put("orderCrossEligible", (message).field().orderCrossEligible()); \
                                                                                                         attr.put("blockTradeEligible", (message).field().blockTradeEligible()); \
                                                                                                         attr.put("eFPEligible", (message).field().eFPEligible()); \
                                                                                                         attr.put("eBFEligible", (message).field().eBFEligible()); \
                                                                                                         attr.put("eFSEligible", (message).field().eFSEligible()); \
                                                                                                         attr.put("eFREligible", (message).field().eFREligible()); \
                                                                                                         attr.put("oTCEligible", (message).field().oTCEligible()); \
                                                                                                         attr.put("iLinkIndicativeMassQuotingEligible", (message).field().iLinkIndicativeMassQuotingEligible()); \
                                                                                                         attr.put("negativeStrikeEligible", (message).field().negativeStrikeEligible()); \
                                                                                                         attr.put("negativePriceOutrightEligible", (message).field().negativePriceOutrightEligible()); \
                                                                                                         attr.put("isFractional", (message).field().isFractional()); \
                                                                                                         attr.put("volatilityQuotedOption", (message).field().volatilityQuotedOption()); \
                                                                                                         attr.put("rFQCrossEligible", (message).field().rFQCrossEligible()); \
                                                                                                         attr.put("zeroPriceOutrightEligible", (message).field().zeroPriceOutrightEligible()); \
                                                                                                         attr.put("decayingProductEligibility", (message).field().decayingProductEligibility()); \
                                                                                                         attr.put("variableProductEligibility", (message).field().variableProductEligibility()); \
                                                                                                         attr.put("dailyProductEligibility", (message).field().dailyProductEligibility()); \
                                                                                                         attr.put("gTOrdersEligibility", (message).field().gTOrdersEligibility()); \
                                                                                                         attr.put("impliedMatchingEligibility", (message).field().impliedMatchingEligibility()); \
                                                                                                         json.put_child(#field, attr); }

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

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
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "ChannelReset4");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::ChannelReset4::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON(entity, noMDEntries, applID);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionFuture27 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDInstrumentDefinitionFuture27");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON(json, *m, securityUpdateAction);
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

        boost::property_tree::ptree events;
        mktdata::MDInstrumentDefinitionFuture27::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            boost::property_tree::ptree event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            events.push_back(std::make_pair("",event));
        }
        json.put_child("noEvents", events);

        boost::property_tree::ptree feedTypes;
        mktdata::MDInstrumentDefinitionFuture27::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            boost::property_tree::ptree feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            feedTypes.push_back(std::make_pair("",feedType));
        }
        json.put_child("noMDFeedTypes", feedTypes);

        boost::property_tree::ptree attrs;
        mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            boost::property_tree::ptree attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            attrs.push_back(std::make_pair("",attr));
        }
        json.put_child("noInstAttrib", attrs);

        boost::property_tree::ptree rules;
        mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            boost::property_tree::ptree rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            rules.push_back(std::make_pair("",rule));
        }
        json.put_child("noLotTypeRules", rules);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionSpread29 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDInstrumentDefinitionSpread29");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON(json, *m, securityUpdateAction);
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

        boost::property_tree::ptree events;
        mktdata::MDInstrumentDefinitionSpread29::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            boost::property_tree::ptree event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            events.push_back(std::make_pair("",event));
        }
        json.put_child("noEvents", events);

        boost::property_tree::ptree feedTypes;
        mktdata::MDInstrumentDefinitionSpread29::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            boost::property_tree::ptree feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            feedTypes.push_back(std::make_pair("",feedType));
        }
        json.put_child("noMDFeedTypes", feedTypes);

        boost::property_tree::ptree attrs;
        mktdata::MDInstrumentDefinitionSpread29::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            boost::property_tree::ptree attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            attrs.push_back(std::make_pair("",attr));
        }
        json.put_child("noInstAttrib", attrs);

        boost::property_tree::ptree rules;
        mktdata::MDInstrumentDefinitionSpread29::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            boost::property_tree::ptree rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            rules.push_back(std::make_pair("",rule));
        }
        json.put_child("noLotTypeRules", rules);

        boost::property_tree::ptree legs;
        mktdata::MDInstrumentDefinitionSpread29::NoLegs& noLegs = m->noLegs();
        while (noLegs.hasNext())
        {
            noLegs.next();

            boost::property_tree::ptree leg;
            PUT_TO_JSON(leg, noLegs, legSecurityID);
            PUT_TO_JSON_CHAR(leg, noLegs, legSecurityIDSource);
            PUT_TO_JSON(leg, noLegs, legSide);
            PUT_TO_JSON(leg, noLegs, legRatioQty);
            PUT_TO_JSON_PRICE(leg, noLegs, legOptionDelta);
            PUT_TO_JSON_PRICE_NON_NULL(leg, noLegs, legPrice);

            legs.push_back(std::make_pair("",leg));
        }
        json.put_child("noLegs", legs);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SecurityStatus30 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "SecurityStatus30");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON(json, *m, securityGroup);
        PUT_TO_JSON(json, *m, asset);
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
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshBook32");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDPriceLevel);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        boost::property_tree::ptree orderIds;
        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();

            boost::property_tree::ptree orderId;
            PUT_TO_JSON(orderId, noOrderIDEntries, orderID);
            PUT_TO_JSON(orderId, noOrderIDEntries, orderUpdateAction);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, mDDisplayQty);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, referenceID);
            PUT_TO_JSON_NON_NULL(orderId, noOrderIDEntries, mDOrderPriority);

            orderIds.push_back(std::make_pair("",orderId));
        }
        json.put_child("noOrderIDEntries", orderIds);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshDailyStatistics33 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshDailyStatistics33");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, tradingReferenceDate);
            PUT_TO_JSON_SETTL_PRICE(entity, noMDEntries, settlPriceType);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshLimitsBanding34 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshLimitsBanding34");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, highLimitPrice);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, lowLimitPrice);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, maxPriceVariation);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshSessionStatistics35 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshSessionStatistics35");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, openCloseSettlFlag);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshTrade36 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshTrade36");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshTrade36::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, tradeID);
            PUT_TO_JSON(entity, noMDEntries, aggressorSide);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshVolume37 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshVolume37");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SnapshotFullRefresh38 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "SnapshotFullRefresh38");
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

        boost::property_tree::ptree entities;
        mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, tradingReferenceDate);
            PUT_TO_JSON(entity, noMDEntries, openCloseSettlFlag);
            PUT_TO_JSON_SETTL_PRICE(entity, noMDEntries, settlPriceType);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDPriceLevel);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::QuoteRequest39 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "QuoteRequest39");
        PUT_TO_JSON(json, *m, quoteReqID);
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree syms;
        mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym = m->noRelatedSym();
        while (noRelatedSym.hasNext())
        {
            noRelatedSym.next();

            boost::property_tree::ptree sym;
            PUT_TO_JSON_STRING(sym, noRelatedSym, Symbol);
            PUT_TO_JSON(sym, noRelatedSym, securityID);
            PUT_TO_JSON(sym, noRelatedSym, quoteType);
            PUT_TO_JSON_NON_NULL(sym, noRelatedSym, orderQty);
            PUT_TO_JSON_NON_NULL(sym, noRelatedSym, side);

            syms.push_back(std::make_pair("",sym));
        }
        json.put_child("noRelatedSym", syms);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDInstrumentDefinitionOption41 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDInstrumentDefinitionOption41");
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);
        PUT_TO_JSON(json, *m, securityUpdateAction);
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
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minPriceIncrementAmount);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, unitOfMeasureQty);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, tradingReferencePrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, highLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, lowLimitPrice);
        PUT_TO_JSON_PRICE_NON_NULL(json, *m, minPriceIncrementAmount);

        boost::property_tree::ptree events;
        mktdata::MDInstrumentDefinitionOption41::NoEvents& noEvents = m->noEvents();
        while (noEvents.hasNext())
        {
            noEvents.next();

            boost::property_tree::ptree event;
            PUT_TO_JSON(event, noEvents, eventType);
            PUT_TO_JSON(event, noEvents, eventTime);

            events.push_back(std::make_pair("",event));
        }
        json.put_child("noEvents", events);

        boost::property_tree::ptree feedTypes;
        mktdata::MDInstrumentDefinitionOption41::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        while (noMDFeedTypes.hasNext())
        {
            noMDFeedTypes.next();

            boost::property_tree::ptree feedType;
            PUT_TO_JSON_STRING(feedType, noMDFeedTypes, MDFeedType);
            PUT_TO_JSON(feedType, noMDFeedTypes, marketDepth);

            feedTypes.push_back(std::make_pair("",feedType));
        }
        json.put_child("noMDFeedTypes", feedTypes);

        boost::property_tree::ptree attrs;
        mktdata::MDInstrumentDefinitionOption41::NoInstAttrib& noInstAttrib = m->noInstAttrib();
        while (noInstAttrib.hasNext())
        {
            noInstAttrib.next();

            boost::property_tree::ptree attr;
            PUT_TO_JSON(attr, noInstAttrib, instAttribType);
            PUT_TO_JSON_INST_ATTR(attr, noInstAttrib, instAttribValue);

            attrs.push_back(std::make_pair("",attr));
        }
        json.put_child("noInstAttrib", attrs);

        boost::property_tree::ptree rules;
        mktdata::MDInstrumentDefinitionOption41::NoLotTypeRules& noLotTypeRules = m->noLotTypeRules();
        while (noLotTypeRules.hasNext())
        {
            noLotTypeRules.next();

            boost::property_tree::ptree rule;
            PUT_TO_JSON(rule, noLotTypeRules, lotType);
            PUT_TO_JSON_PRICE(rule, noLotTypeRules, minLotSize);

            rules.push_back(std::make_pair("",rule));
        }
        json.put_child("noLotTypeRules", rules);

        boost::property_tree::ptree underlyings;
        mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& noUnderlyings = m->noUnderlyings();
        while (noUnderlyings.hasNext())
        {
            noUnderlyings.next();

            boost::property_tree::ptree underlying;
            PUT_TO_JSON(underlying, noUnderlyings, underlyingSecurityID);
            PUT_TO_JSON_CHAR(underlying, noUnderlyings, underlyingSecurityIDSource);
            PUT_TO_JSON_STRING(underlying, noUnderlyings, UnderlyingSymbol);

            underlyings.push_back(std::make_pair("",underlying));
        }
        json.put_child("noUnderlyings", underlyings);

        boost::property_tree::ptree instuments;
        mktdata::MDInstrumentDefinitionOption41::NoRelatedInstruments& noRelatedInstruments = m->noRelatedInstruments();
        while (noRelatedInstruments.hasNext())
        {
            noRelatedInstruments.next();

            boost::property_tree::ptree instument;
            PUT_TO_JSON(instument, noRelatedInstruments, relatedSecurityID);
            PUT_TO_JSON_CHAR(instument, noRelatedInstruments, relatedSecurityIDSource);
            PUT_TO_JSON_STRING(instument, noRelatedInstruments, RelatedSymbol);

            instuments.push_back(std::make_pair("",instument));
        }
        json.put_child("noRelatedInstruments", instuments);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshTradeSummary42 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshTradeSummary42");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDEntrySize);
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, rptSeq);
            PUT_TO_JSON(entity, noMDEntries, aggressorSide);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, numberOfOrders);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDTradeEntryID);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        boost::property_tree::ptree orderIds;
        mktdata::MDIncrementalRefreshTradeSummary42::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();

            boost::property_tree::ptree orderId;
            PUT_TO_JSON(orderId, noOrderIDEntries, orderID);
            PUT_TO_JSON(orderId, noOrderIDEntries, lastQty);

            orderIds.push_back(std::make_pair("",orderId));
        }
        json.put_child("noOrderIDEntries", orderIds);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::MDIncrementalRefreshOrderBook43 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "MDIncrementalRefreshOrderBook43");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON_MATCH_EVENT(json, *m, matchEventIndicator);

        boost::property_tree::ptree entities;
        mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, securityID);
            PUT_TO_JSON(entity, noMDEntries, mDUpdateAction);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDDisplayQty);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDOrderPriority);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, orderID);
            PUT_TO_JSON_PRICE_NON_NULL(entity, noMDEntries, mDEntryPx);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_json(mktdata::SnapshotFullRefreshOrderBook44 *m)
    {
        boost::property_tree::ptree json;
        PUT_TO_JSON_CONSTANT(json, "type", "SnapshotFullRefreshOrderBook44");
        PUT_TO_JSON(json, *m, transactTime);
        PUT_TO_JSON(json, *m, lastMsgSeqNumProcessed);
        PUT_TO_JSON(json, *m, totNumReports);
        PUT_TO_JSON(json, *m, securityID);
        PUT_TO_JSON(json, *m, noChunks);
        PUT_TO_JSON(json, *m, currentChunk);

        boost::property_tree::ptree entities;
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& noMDEntries = m->noMDEntries();
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();

            boost::property_tree::ptree entity;
            PUT_TO_JSON(entity, noMDEntries, orderID);
            PUT_TO_JSON_PRICE(entity, noMDEntries, mDEntryPx);
            PUT_TO_JSON(entity, noMDEntries, mDDisplayQty);
            PUT_TO_JSON_CHAR_ENUM(entity, noMDEntries, mDEntryType);
            PUT_TO_JSON_NON_NULL(entity, noMDEntries, mDOrderPriority);

            entities.push_back(std::make_pair("",entity));
        }
        json.put_child("noMDEntries", entities);

        return this->To_string(json);
    }

    std::string SBEtoJSON::To_string(const boost::property_tree::ptree &json_message)
    {
        boost::property_tree::ptree json;
        json.put("market", "CME");
        json.put("reveivedTime", m_sbe_message->received_time());
        json.put("packetSeqNum", m_sbe_message->packet_seq_num());
        json.put("packetSendingTime", m_sbe_message->packet_sending_time());
        json.put_child("message", json_message);

        std::ostringstream buf;
        boost::property_tree::write_json (buf, json, true);
        return buf.str();
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
