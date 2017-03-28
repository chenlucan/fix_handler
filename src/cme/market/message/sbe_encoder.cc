
#include "cme/market/message/sbe_encoder.h"
#include "cme/market/message/mdp_message.h"
#include "core/assist/utility.h"
#include "core/assist/logger.h"

#define MESSAGE_HEADER_VERSION 0     // mdp sbe message header version

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{

    SBEEncoder::SBEEncoder() : m_encoded_length(0)
    {
        // noop
    }

    void SBEEncoder::Start_encode(std::uint16_t templateId)
    {
        mktdata::MessageHeader header;
        std::size_t encodeHeaderLength = 0;
        std::size_t encodeMessageLength = 0;

        // 先做下面几种 message 做测试
        LOG_INFO("message create begin: type=", templateId, "(", fh::cme::market::message::MdpMessage::MDP_MESSAGE_TYPES[templateId], ")");
		
		if(templateId == 4)    // ChannelReset4
        {
            mktdata::ChannelReset4 message;
            encodeHeaderLength = this->Encode_header<mktdata::ChannelReset4>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
		else if(templateId == 15)    // AdminLogin15
        {
            mktdata::AdminLogin15 message;
            encodeHeaderLength = this->Encode_header<mktdata::AdminLogin15>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 16)    // AdminLogout16
        {
            mktdata::AdminLogout16 message;
            encodeHeaderLength = this->Encode_header<mktdata::AdminLogout16>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 27)    // MDInstrumentDefinitionFuture27
        {
            mktdata::MDInstrumentDefinitionFuture27 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDInstrumentDefinitionFuture27>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }		
		else if(templateId == 29)   
        {
            mktdata::MDInstrumentDefinitionSpread29 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDInstrumentDefinitionSpread29>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 30)    // SecurityStatus30
        {
            mktdata::SecurityStatus30 message;
            encodeHeaderLength = this->Encode_header<mktdata::SecurityStatus30>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 32)    // MDIncrementalRefreshBook32
        {
            mktdata::MDIncrementalRefreshBook32 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshBook32>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 33)   // MDIncrementalRefreshDailyStatistics33
        { 
            mktdata::MDIncrementalRefreshDailyStatistics33 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshDailyStatistics33>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }	
        else if(templateId == 34)   // MDIncrementalRefreshLimitsBanding34
        {
            mktdata::MDIncrementalRefreshLimitsBanding34 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshLimitsBanding34>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }	
        else if(templateId == 35)   //  MDIncrementalRefreshSessionStatistics35
        {
            mktdata::MDIncrementalRefreshSessionStatistics35 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshSessionStatistics35>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }	
        else if(templateId == 36)    // MDIncrementalRefreshTrade36
        {
            mktdata::MDIncrementalRefreshTrade36 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshTrade36>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }		
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            mktdata::MDIncrementalRefreshVolume37 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshVolume37>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }

        else if(templateId == 38)    // SnapshotFullRefresh38
        {
            mktdata::SnapshotFullRefresh38 message;
            encodeHeaderLength = this->Encode_header<mktdata::SnapshotFullRefresh38>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
		else if(templateId == 41)    // MDInstrumentDefinitionOption41
        {
            mktdata::MDInstrumentDefinitionOption41 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDInstrumentDefinitionOption41>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }		
        else if(templateId == 42)   // MDIncrementalRefreshTradeSummary42
        {
            mktdata::MDIncrementalRefreshTradeSummary42 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshTradeSummary42>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }		
        else if(templateId == 43)   // MDIncrementalRefreshOrderBook43
        {
            mktdata::MDIncrementalRefreshOrderBook43 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshOrderBook43>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }		
		else if(templateId == 44)    // SnapshotFullRefreshOrderBook44
        {
            mktdata::SnapshotFullRefreshOrderBook44 message;
            encodeHeaderLength = this->Encode_header<mktdata::SnapshotFullRefreshOrderBook44>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }	
	
        LOG_INFO("message created: type=", templateId, "(", fh::cme::market::message::MdpMessage::MDP_MESSAGE_TYPES[templateId], ")");

        m_encoded_length = encodeHeaderLength + encodeMessageLength;
    }

    template <typename MessageType> 
    std::size_t SBEEncoder::Encode_header(mktdata::MessageHeader &header)
    {
        header.wrap(m_buffer, 0, MESSAGE_HEADER_VERSION, sizeof(m_buffer))
              .blockLength(MessageType::sbeBlockLength())
              .templateId(MessageType::sbeTemplateId())
              .schemaId(MessageType::sbeSchemaId())
              .version(MessageType::sbeSchemaVersion());

        return header.encodedLength();
    }
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::ChannelReset4 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        mktdata::ChannelReset4::NoMDEntries& entries = message.noMDEntriesCount(1);
        entries.next().applID(360);

        return message.encodedLength();
    }	
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::AdminLogin15 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.heartBtInt(3);

        return message.encodedLength();
    }

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::AdminLogout16 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.putText("12345");
        return message.encodedLength();
    }
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionFuture27 &message)
    {
        char symbol[20] = "ProductName-123";
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(false)
               .lastQuoteMsg(false)
               .lastStatsMsg(false)
               .lastImpliedMsg(false)
               .recoveryMsg(false)
               .reserved(true)
               .endOfEvent(true);
        message.totNumReports(10)
               .securityUpdateAction(mktdata::SecurityUpdateAction::Add)
               .lastUpdateTime( fh::core::assist::utility::Current_time_ns())
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .applID(12)
               .marketSegmentID(13)
               .underlyingProduct(14)
               .putSecurityExchange("abcd")
               .putSecurityGroup("efghjk")
               .putAsset("A1234A")
               .putSymbol(symbol)
               .securityID(fh::core::assist::utility::Random_number(72, 73))
               .putSecurityType("D1234D")
               .putCFICode("E1234E")
               .maturityMonthYear().year(2017).month(1).day(12).week(3);
        message.putCurrency("JPY")
               .putSettlCurrency("USA")
               .matchAlgorithm('a')
               .minTradeVol(100)
               .maxTradeVol(200)
               .minPriceIncrement().mantissa(700001);
        message.displayFactor().mantissa(700002);
        message.mainFraction(20)
               .subFraction(30)
               .priceDisplayFormat(40)
               .putUnitOfMeasure("E1234567890123456789012345678E")
               .unitOfMeasureQty().mantissa(700000);
        message.tradingReferencePrice().mantissa(700003);
        message.settlPriceType().clear().finalrc(true);
        message.openInterestQty(50)
               .clearedVolume(60)
               .highLimitPrice().mantissa(700004);
        message.lowLimitPrice().mantissa(700005);
        message.maxPriceVariation().mantissa(700006);
        message.decayStartDate(18302)
               .originalContractSize(70)
               .contractMultiplier(80)
               .contractMultiplierUnit(90)
               .flowScheduleType(100)
               .minPriceIncrementAmount().mantissa(700007);
        message.userDefinedInstrument('x')
               .tradingReferenceDate(18305);

        mktdata::MDInstrumentDefinitionFuture27::NoEvents& noEvents = message.noEventsCount(1);
        noEvents.next().eventType(mktdata::EventType::Activation).eventTime(18400);

        mktdata::MDInstrumentDefinitionFuture27::NoMDFeedTypes& fs = message.noMDFeedTypesCount(2);
        fs.next().putMDFeedType("GBX").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));
        fs.next().putMDFeedType("GBI").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));

        mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& is = message.noInstAttribCount(1);
        is.next().instAttribValue()
                 .clear()
                 .electronicMatchEligible(true);

        mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& ts = message.noLotTypeRulesCount(2);
        ts.next().lotType(81).minLotSize().mantissa(20001);
        ts.next().lotType(82).minLotSize().mantissa(20002);

        LOG_INFO("message 27(d): securityID=", message.securityID(), ", symbol=", message.getSymbolAsString());

        return message.encodedLength();
    }
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionSpread29 &message)
    {
        char symbol[20] = "ProductName-123";
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(false)
               .lastQuoteMsg(false)
               .lastStatsMsg(false)
               .lastImpliedMsg(false)
               .recoveryMsg(false)
               .reserved(true)
               .endOfEvent(true);
        message.totNumReports(10)
               .securityUpdateAction(mktdata::SecurityUpdateAction::Add)
               .lastUpdateTime( fh::core::assist::utility::Current_time_ns())
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .applID(12)
               .marketSegmentID(13)
               .underlyingProduct(14)
			   .putSecuritySubType("jpt")
               .putSecurityExchange("abcd")
               .putSecurityGroup("efghjk")
               .putAsset("A1234A")
               .putSymbol(symbol)
               .securityID(fh::core::assist::utility::Random_number(72, 73))
               .putSecurityType("D1234D")
               .putCFICode("E1234E")
               .maturityMonthYear().year(2017).month(1).day(12).week(3);
        message.putCurrency("JPY")
               .matchAlgorithm('a')
			   .tickRule(15)
               .minTradeVol(100)
               .maxTradeVol(200)
               .minPriceIncrement().mantissa(700001);
		message.priceRatio().mantissa(1356);
        message.displayFactor().mantissa(700002);
        message.mainFraction(20)
               .subFraction(30)
               .priceDisplayFormat(40)
               .putUnitOfMeasure("E1234567890123456789012345678E");
        message.tradingReferencePrice().mantissa(700003);
        message.settlPriceType().clear().finalrc(true);
        message.openInterestQty(50)
               .clearedVolume(60)
               .highLimitPrice().mantissa(700004);
        message.lowLimitPrice().mantissa(700005);
        message.maxPriceVariation().mantissa(700006);
        message.userDefinedInstrument('x')
               .tradingReferenceDate(18305);

        mktdata::MDInstrumentDefinitionSpread29::NoEvents& noEvents = message.noEventsCount(1);
        noEvents.next().eventType(mktdata::EventType::Activation).eventTime(18400);

        mktdata::MDInstrumentDefinitionSpread29::NoMDFeedTypes& fs = message.noMDFeedTypesCount(2);
        fs.next().putMDFeedType("GBX").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));
        fs.next().putMDFeedType("GBI").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));

        mktdata::MDInstrumentDefinitionSpread29::NoInstAttrib& is = message.noInstAttribCount(1);
        is.next().instAttribValue()
                 .clear()
                 .electronicMatchEligible(true);

        mktdata::MDInstrumentDefinitionSpread29::NoLotTypeRules& ts = message.noLotTypeRulesCount(2);
        ts.next().lotType(81).minLotSize().mantissa(20001);
        ts.next().lotType(82).minLotSize().mantissa(20002);

        mktdata::MDInstrumentDefinitionSpread29::NoLegs& ns = message.noLegsCount(1);
		mktdata::MDInstrumentDefinitionSpread29::NoLegs& ns1 = ns.next();
        ns1.legSecurityID(81).legSide(mktdata::LegSide::Value::BuySide).legRatioQty(15).legPrice().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
		ns1.legSecurityIDSource(fh::core::assist::utility::Random_number(0, 1));
        ns1.legOptionDelta().mantissa(6526);
		
        return message.encodedLength();
    }		
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SecurityStatus30 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns())
               .putSecurityGroup("ABCDEF")
               .putAsset("+-*/")
               .securityID(654)
               .tradeDate(2017);
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(false)
               .lastQuoteMsg(false)
               .lastStatsMsg(false)
               .lastImpliedMsg(false)
               .recoveryMsg(false)
               .reserved(true)
               .endOfEvent(true);

        message.securityTradingStatus(mktdata::SecurityTradingStatus::Close) // 4
               .haltReason(mktdata::HaltReason::InstrumentExpiration) // 4
               .securityTradingEvent(mktdata::SecurityTradingEvent::ResetStatistics); // 4

        return message.encodedLength();
    }	

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshBook32 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
               .numberOfOrders(fh::core::assist::utility::Random_number(1, 30))
               .mDEntrySize(fh::core::assist::utility::Random_number(10, 100))
               .mDPriceLevel(fh::core::assist::utility::Random_number(1, 10))
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4))       // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom
               .mDEntryType((mktdata::MDEntryTypeBook::Value)fh::core::assist::utility::Random_number(48, 49));     // '0'(48): Bid  '1'(49): Offer

            LOG_INFO("entity in message 32(X): securityID=", n.securityID(),
                                ", type=", (char)n.mDEntryType(),  ", action=", n.mDUpdateAction(), ", level=", (int)n.mDPriceLevel(),
                                ", order=", n.numberOfOrders(), ", count=", n.mDEntrySize(), ", price=", n.mDEntryPx().mantissa());
        }

        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& e = message.noOrderIDEntriesCount(1);
        e.next().orderID(123).mDOrderPriority(456).mDDisplayQty(789).referenceID(120).orderUpdateAction(mktdata::OrderUpdateAction::New);

        return message.encodedLength();
    }

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshDailyStatistics33 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
			n.settlPriceType().clear().finalrc(true);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
               .mDEntrySize(fh::core::assist::utility::Random_number(10, 100))
			   .tradingReferenceDate(18305)
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4))      // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom
               .mDEntryType((mktdata::MDEntryTypeDailyStatistics::Value)fh::core::assist::utility::Random_number(48, 49));      // '0'(48): Bid  '1'(49): Offer

            LOG_INFO("entity in message 33(X): securityID=", n.securityID(),
                                ", type=", (char)n.mDEntryType(),  ", action=", n.mDUpdateAction(), ", tradingReferenceDate=", (int)n.tradingReferenceDate(),
                                ", count=", n.mDEntrySize(), ", price=", n.mDEntryPx().mantissa());
        }

        return message.encodedLength();
    }
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshLimitsBanding34 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
			n.highLimitPrice().mantissa(700004);
            n.lowLimitPrice().mantissa(700005);
            n.maxPriceVariation().mantissa(700006);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
            .mDEntryType(fh::core::assist::utility::Random_number(48, 49));     // '0'(48): Bid  '1'(49): Offer

	    }

        return message.encodedLength();
    }		

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshSessionStatistics35 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
			   .openCloseSettlFlag(mktdata::OpenCloseSettlFlag::DailyOpenPrice)
               .mDEntrySize(fh::core::assist::utility::Random_number(10, 100))
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4))       // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom
               .mDEntryType((mktdata::MDEntryTypeStatistics::Value)fh::core::assist::utility::Random_number(48, 49));     // '0'(48): Bid  '1'(49): Offer
        }
		
        return message.encodedLength();
    }		
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshTrade36 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 3);
        mktdata::MDIncrementalRefreshTrade36::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*2000000);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
               .numberOfOrders(fh::core::assist::utility::Random_number(1, 30))
               .mDEntrySize(fh::core::assist::utility::Random_number(10, 100))
               .tradeID(fh::core::assist::utility::Random_number(1, 10))
               .aggressorSide((mktdata::AggressorSide::Value)fh::core::assist::utility::Random_number(1, 2))      // 1: Buy  2: Sell
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4));       // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom

            LOG_INFO("entity in message 36(X): securityID=", n.securityID(),
                                "action=", n.mDUpdateAction(), ", trade id=", n.tradeID(), ", aggressor=", n.aggressorSide(),
                                ", order=", n.numberOfOrders(), ", count=", n.mDEntrySize(), ", price=", n.mDEntryPx().mantissa());
        }

        return message.encodedLength();
    }	
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshVolume37 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& entries = message.noMDEntriesCount(3);
        entries.next().mDEntrySize(71).securityID(72).rptSeq(73).mDUpdateAction(mktdata::MDUpdateAction::New);  // 0
        entries.next().mDEntrySize(61).securityID(62).rptSeq(63).mDUpdateAction(mktdata::MDUpdateAction::Change); // 1
        entries.next().mDEntrySize(51).securityID(52).rptSeq(53).mDUpdateAction(mktdata::MDUpdateAction::Delete); // 2

        return message.encodedLength();
    }
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SnapshotFullRefresh38 &message)
    {
        static int inc = 0;    // 用这个来让每次生成的 lastMsgSeqNumProcessed 慢慢递增
        inc += fh::core::assist::utility::Random_number(0, 1);

        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.lastMsgSeqNumProcessed(7 + inc)
               .totNumReports(4)
               .securityID(72)
               .rptSeq(100)
               .transactTime(fh::core::assist::utility::Current_time_ns())
               .lastUpdateTime(fh::core::assist::utility::Current_time_ns())
               .tradeDate(18300)
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .highLimitPrice().mantissa(80000000);
        message.lowLimitPrice().mantissa(9000000);
        message.maxPriceVariation().mantissa(800003);

        mktdata::SnapshotFullRefresh38::NoMDEntries& entries = message.noMDEntriesCount(2);
        {
        mktdata::SnapshotFullRefresh38::NoMDEntries& n = entries.next();
        n.mDEntryPx().mantissa((600 + inc)*100000);
        n.mDEntrySize(900 + inc).numberOfOrders(90 + inc).mDPriceLevel(1).tradingReferenceDate(18300);
        n.openCloseSettlFlag(mktdata::OpenCloseSettlFlag::DailyOpenPrice);
        n.settlPriceType().clear().finalrc(true);
        n.mDEntryType(mktdata::MDEntryType::Bid);
        }
        {
        mktdata::SnapshotFullRefresh38::NoMDEntries& n = entries.next();
        n.mDEntryPx().mantissa((700 + inc)*100000);
        n.mDEntrySize(700 + inc).numberOfOrders(70 + inc).mDPriceLevel(3).tradingReferenceDate(18300);
        n.openCloseSettlFlag(mktdata::OpenCloseSettlFlag::DailyOpenPrice);
        n.settlPriceType().clear().finalrc(true);
        n.mDEntryType(mktdata::MDEntryType::Bid);
        }
        return message.encodedLength();
    }	

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionOption41 &message)
    {
        char symbol[20] = "ProductName-123";
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(false)
               .lastQuoteMsg(false)
               .lastStatsMsg(false)
               .lastImpliedMsg(false)
               .recoveryMsg(false)
               .reserved(true)
               .endOfEvent(true);
        message.totNumReports(10)
               .securityUpdateAction(mktdata::SecurityUpdateAction::Add)
               .lastUpdateTime( fh::core::assist::utility::Current_time_ns())
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .applID(12)
               .marketSegmentID(13)
               .underlyingProduct(14)
               .putSecurityExchange("abcd")
			   .tickRule(15)
               .putSecurityGroup("efghjk")
               .putAsset("A1234A")
               .putSymbol(symbol)
               .securityID(fh::core::assist::utility::Random_number(72, 73))
               .putSecurityType("D1234D")
               .putCFICode("E1234E")
			   .tickRule(15)
               .maturityMonthYear().year(2017).month(1).day(12).week(3);
		message.putOrCall(mktdata::PutOrCall::Value::Put);
		message.strikePrice().mantissa(1024); 
		message.strikeCurrency(1,3);
		message.minCabPrice().mantissa(1524);
        message.putCurrency("JPY")
               .putSettlCurrency("USA")
               .matchAlgorithm('a')
               .minTradeVol(100)
               .maxTradeVol(200)
               .minPriceIncrement().mantissa(700001);
        message.displayFactor().mantissa(700002);

        message.mainFraction(20)
               .subFraction(30)
               .priceDisplayFormat(40)
               .putUnitOfMeasure("E1234567890123456789012345678E")
               .unitOfMeasureQty().mantissa(700000);
        message.tradingReferencePrice().mantissa(700003);
        message.settlPriceType().clear().finalrc(true);
        message.openInterestQty(50)
               .clearedVolume(60)
               .highLimitPrice().mantissa(700004);
        message.lowLimitPrice().mantissa(700005);
        message.minPriceIncrementAmount().mantissa(700007);
        message.userDefinedInstrument('x')
               .tradingReferenceDate(18305);

        mktdata::MDInstrumentDefinitionOption41::NoEvents& noEvents = message.noEventsCount(1);
        noEvents.next().eventType(mktdata::EventType::Activation).eventTime(18400);

        mktdata::MDInstrumentDefinitionOption41::NoMDFeedTypes& fs = message.noMDFeedTypesCount(2);
        fs.next().putMDFeedType("GBX").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));
        fs.next().putMDFeedType("GBI").marketDepth(3 + fh::core::assist::utility::Random_number(1, 5));

        mktdata::MDInstrumentDefinitionOption41::NoInstAttrib& is = message.noInstAttribCount(1);
        is.next().instAttribValue()
                 .clear()
                 .electronicMatchEligible(true);

        mktdata::MDInstrumentDefinitionOption41::NoLotTypeRules& ts = message.noLotTypeRulesCount(2);
        ts.next().lotType(81).minLotSize().mantissa(20001);
        ts.next().lotType(82).minLotSize().mantissa(20002);		
		
		mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& us = message.noUnderlyingsCount(2);
		mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& ts1 = us.next();
        ts1.underlyingSecurityID(81).underlyingSecurityIDSource(fh::core::assist::utility::Random_number(0, 1));
		ts1.underlyingSymbol(10,15);
		mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& ts2 =  us.next();
        ts2.underlyingSecurityID(45).underlyingSecurityIDSource(fh::core::assist::utility::Random_number(0, 1));
		ts2.underlyingSymbol(10,12);

		mktdata::MDInstrumentDefinitionOption41::NoRelatedInstruments& tts = message.noRelatedInstrumentsCount(1);
		mktdata::MDInstrumentDefinitionOption41::NoRelatedInstruments& tts1 = tts.next();
        tts1.relatedSecurityID(253).relatedSecurityIDSource(fh::core::assist::utility::Random_number(0, 1));	
        tts1.relatedSymbol(10,12);		
        return message.encodedLength();
    }		
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshTradeSummary42 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
            n.securityID(fh::core::assist::utility::Random_number(72, 73)).rptSeq(73)
               .numberOfOrders(fh::core::assist::utility::Random_number(1, 30))
			   .mDTradeEntryID(259)
			   .aggressorSide((mktdata::AggressorSide::Value)fh::core::assist::utility::Random_number(1, 2))
               .mDEntrySize(fh::core::assist::utility::Random_number(10, 100))
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4))       // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom
               .mDEntryType(fh::core::assist::utility::Random_number(48, 49));     // '0'(48): Bid  '1'(49): Offer
        }

        mktdata::MDIncrementalRefreshTradeSummary42::NoOrderIDEntries& e = message.noOrderIDEntriesCount(1);
        e.next().orderID(123).lastQty(789);

        return message.encodedLength();
    }	

	std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshOrderBook43 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(fh::core::assist::utility::Current_time_ns());
        message.matchEventIndicator()
               .clear()
               .lastTradeMsg(false)
               .lastVolumeMsg(true)
               .lastQuoteMsg(false)
               .lastStatsMsg(true)
               .lastImpliedMsg(false)
               .recoveryMsg(true)
               .reserved(false)
               .endOfEvent(true);

        int count =  fh::core::assist::utility::Random_number(1, 10);
        mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& entries = message.noMDEntriesCount(count);
        while (entries.hasNext())
        {
            auto n = entries.next();
			n.orderID(123).mDOrderPriority(456);
            n.mDEntryPx().mantissa(fh::core::assist::utility::Random_number(1, 100)*10000000);
            n.securityID(fh::core::assist::utility::Random_number(72, 73))
			   .mDOrderPriority(456)
			   .mDDisplayQty(789)
               .mDUpdateAction((mktdata::MDUpdateAction::Value)fh::core::assist::utility::Random_number(0, 4))       // 0: New  1: Change 2: Delete  3: DeleteThru  4: DeleteFrom
               .mDEntryType((mktdata::MDEntryTypeBook::Value)fh::core::assist::utility::Random_number(48, 49));     // '0'(48): Bid  '1'(49): Offer

        }

        return message.encodedLength();
    }	
	
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SnapshotFullRefreshOrderBook44 &message)
    {
        static int inc = 0;    
        inc += fh::core::assist::utility::Random_number(0, 1);

        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.lastMsgSeqNumProcessed(7 + inc)
               .totNumReports(5)
               .securityID(73)
			   .noChunks(150)
			   .currentChunk(20)			   
               .transactTime(fh::core::assist::utility::Current_time_ns());

        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& entries = message.noMDEntriesCount(3);
        {
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& n = entries.next();
		n.orderID(123);
		n.mDOrderPriority(3678);
        n.mDEntryPx().mantissa((600 + inc)*100000);
		n.mDDisplayQty(456);
        n.mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }
        {
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& n = entries.next();
		n.orderID(567);
		n.mDOrderPriority(2345);
        n.mDEntryPx().mantissa((600 + inc)*100000);
		n.mDDisplayQty(587);
        n.mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }
        {
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& n = entries.next();
		n.orderID(222);
		n.mDOrderPriority(7963);
        n.mDEntryPx().mantissa((600 + inc)*100000);
		n.mDDisplayQty(363);
        n.mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }		
        return message.encodedLength();
    }	

    std::pair<char*, std::size_t> SBEEncoder::Encoded_buffer()
    {
        return std::make_pair(m_buffer, m_encoded_length);
    }

    std::string SBEEncoder::Encoded_hex_str() const
    {
        return fh::core::assist::utility::Hex_str(m_buffer, m_encoded_length);
    }

    SBEEncoder::~SBEEncoder()
    {
        // noop
    }

} // namespace message
} // namespace market
} // namespace cme
} // namespace fh
