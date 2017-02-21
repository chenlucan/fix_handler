
#include "sbe_encoder.h"
#include "utility.h"

namespace rczg
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

        if(templateId == 30)    // SecurityStatus30
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
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            mktdata::MDIncrementalRefreshVolume37 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDIncrementalRefreshVolume37>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 27)    // MDInstrumentDefinitionFuture27
        {
            mktdata::MDInstrumentDefinitionFuture27 message;
            encodeHeaderLength = this->Encode_header<mktdata::MDInstrumentDefinitionFuture27>(header);
            encodeMessageLength = this->Encode_message(header, message);
        }
        else if(templateId == 38)    // SnapshotFullRefresh38
        {
            mktdata::SnapshotFullRefresh38 message;
            encodeHeaderLength = this->Encode_header<mktdata::SnapshotFullRefresh38>(header);
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
        // TODO other messages
        
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

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshBook32 &message)
    {
    	static int inc = 0;	// 用这个来让每次生成的数值递增
    	inc += rczg::utility::Random_number(1, 3);

    	message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(rczg::utility::Current_time_ns());
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

        mktdata::MDIncrementalRefreshBook32::NoMDEntries& entries = message.noMDEntriesCount(3);
        {
        auto n = entries.next();
        n.mDEntryPx().mantissa(1000000 + (inc++));
        n.mDEntrySize(10 + inc).securityID(3).rptSeq(73).numberOfOrders(100 + inc).mDPriceLevel(1)
        		.mDUpdateAction(mktdata::MDUpdateAction::New).mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }
        {
        auto n = entries.next();
        n.mDEntryPx().mantissa(1000000 + (inc++));
        n.mDEntrySize(10 + inc).securityID(3).rptSeq(73).numberOfOrders(100 + inc).mDPriceLevel(1)
        		.mDUpdateAction(mktdata::MDUpdateAction::Change).mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }
        {
        auto n = entries.next();
        n.mDEntryPx().mantissa(1000000 + (inc++));
        n.mDEntrySize(10 + inc).securityID(3).rptSeq(73).numberOfOrders(100 + inc).mDPriceLevel(1)
        		.mDUpdateAction(mktdata::MDUpdateAction::Delete).mDEntryType(mktdata::MDEntryTypeBook::Bid);
        }

        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& e = message.noOrderIDEntriesCount(1);
        e.next().orderID(123).mDOrderPriority(456).mDDisplayQty(789).referenceID(120).orderUpdateAction(mktdata::OrderUpdateAction::New);

        return message.encodedLength();
    }

    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshVolume37 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(rczg::utility::Current_time_ns());
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
    
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SecurityStatus30 &message)
    {
        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.transactTime(rczg::utility::Current_time_ns())
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
    
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionFuture27 &message)
    {
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
               .securityUpdateAction(mktdata::SecurityUpdateAction::Delete)
               .lastUpdateTime(rczg::utility::Current_time_ns())
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .applID(12)
               .marketSegmentID(13)
               .underlyingProduct(14)
               .putSecurityExchange("abcd")
               .putSecurityGroup("efghjk")
               .putAsset("A1234A")
               .putSymbol("B123456789012345678B")
               .securityID(3)
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
        fs.next().putMDFeedType("GBX").marketDepth(10 + rczg::utility::Random_number(1, 5));
        fs.next().putMDFeedType("GBI").marketDepth(10 + rczg::utility::Random_number(1, 5));
                      
        mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& is = message.noInstAttribCount(1);
        is.next().instAttribValue()
                 .clear()
                 .electronicMatchEligible(true);
        
        mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& ts = message.noLotTypeRulesCount(2);
        ts.next().lotType(81).minLotSize().mantissa(20001);
        ts.next().lotType(82).minLotSize().mantissa(20002);
        
        return message.encodedLength();
    }
    
    std::size_t SBEEncoder::Encode_message(mktdata::MessageHeader &header, mktdata::SnapshotFullRefresh38 &message)
    {
    	static int inc = 0;	// 用这个来让每次生成的 lastMsgSeqNumProcessed 慢慢递增
    	inc += rczg::utility::Random_number(0, 1);

        message.wrapForEncode(m_buffer, header.encodedLength(), sizeof(m_buffer));
        message.lastMsgSeqNumProcessed(7 + inc)
               .totNumReports(4)
               .securityID(3)
               .rptSeq(100)
               .transactTime(rczg::utility::Current_time_ns())
               .lastUpdateTime(rczg::utility::Current_time_ns())
               .tradeDate(18300)
               .mDSecurityTradingStatus(mktdata::SecurityTradingStatus::Close)
               .highLimitPrice().mantissa(800001);
        message.lowLimitPrice().mantissa(800002);
        message.maxPriceVariation().mantissa(800003);
        
        mktdata::SnapshotFullRefresh38::NoMDEntries& entries = message.noMDEntriesCount(2);
        {
        mktdata::SnapshotFullRefresh38::NoMDEntries& n = entries.next();
        n.mDEntryPx().mantissa(9000000 + inc);
        n.mDEntrySize(900 + inc).numberOfOrders(90 + inc).mDPriceLevel(1).tradingReferenceDate(18300);
        n.openCloseSettlFlag(mktdata::OpenCloseSettlFlag::DailyOpenPrice);
        n.settlPriceType().clear().finalrc(true);
        n.mDEntryType(mktdata::MDEntryType::Bid);
        }
        {
        mktdata::SnapshotFullRefresh38::NoMDEntries& n = entries.next();
        n.mDEntryPx().mantissa(7000000 + inc);
        n.mDEntrySize(700 + inc).numberOfOrders(70 + inc).mDPriceLevel(3).tradingReferenceDate(18300);
        n.openCloseSettlFlag(mktdata::OpenCloseSettlFlag::DailyOpenPrice);
        n.settlPriceType().clear().finalrc(true);
        n.mDEntryType(mktdata::MDEntryType::Bid);
        }
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

    std::pair<char*, std::size_t> SBEEncoder::Encoded_buffer()
    {
        return std::make_pair(m_buffer, m_encoded_length);
    }

    std::string SBEEncoder::Encoded_hex_str() const
    {
        return rczg::utility::Hex_str(m_buffer, m_encoded_length);
    }

    SBEEncoder::~SBEEncoder()
    {
        // noop
    }
    
} // namespace rczg
    
