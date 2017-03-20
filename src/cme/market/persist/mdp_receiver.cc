#include "cme/market/persist/mdp_receiver.h"
#include "cme/market/message/sbe_decoder.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace persist
{

    MdpReceiver::MdpReceiver(
            const std::string &receive_url,
            const std::string &db_url,
            const std::string &db_name,
            const std::string &collection_name)
    : fh::core::zmq::ZmqReceiver(receive_url), m_db_url(db_url), m_db_name(db_name), m_collection_name(collection_name)
    {
		mongoc_init ();
		m_client = mongoc_client_new (m_db_url.c_str());
		m_collection = mongoc_client_get_collection (m_client, m_db_name.c_str(), m_collection_name.c_str());
    }

    MdpReceiver::~MdpReceiver()
    {
		mongoc_collection_destroy (m_collection);
		mongoc_client_destroy (m_client);
		mongoc_cleanup ();	
    }

	void MdpReceiver::DestroyBsonVector(const std::vector<bson_t *> &valList)
	{
		for (std::vector<bson_t *>::const_iterator iter = valList.cbegin(); iter != valList.cend(); iter++)
		{
			bson_destroy (*iter);
		}
	}	
	
    void MdpReceiver::Save(char *data, size_t size)
    {
        //        8 bytes : m_received_time
        //        2 bytes : m_packet_length
        //        4 bytes : m_packet_seq_num
        //        8 bytes : m_packet_sending_time
        //        2 bytes : m_message_length
        //        (m_message_length) bytes : m_buffer
        std::vector<bson_t *> arraydestroy;
        std::uint64_t received_time = *(std::uint64_t *)data;
        std::uint16_t packet_length = *(std::uint16_t *)(data + 8);     // 这个域不保存
        std::uint32_t packet_seq_num = *(std::uint32_t *)(data + 8 + 2);
        std::uint64_t packet_sending_time = *(std::uint64_t *)(data + 8 + 2 + 4);
        std::uint16_t message_length = *(std::uint16_t *)(data + 8 + 2 + 4 + 8);     // 这个域不保存

        LOG_INFO("now save origin message to db: packet_seq_num=", packet_seq_num, ", packet_length=", packet_length, ", message_length=", message_length);

        fh::cme::market::message::SBEDecoder decoder(data + 8 + 2 + 4 + 8 + 2, *(std::uint16_t *)(data + 8 + 2 + 4 + 8));
        auto sbe_message = decoder.Start_decode();
        auto mdp_header = static_cast<mktdata::MessageHeader*>(sbe_message.first.get());
        void *mdp_message = sbe_message.second.get();

        bson_t *document = BCON_NEW(
                "receivedTime", BCON_INT64(received_time),
                "packetSeqNum", BCON_INT32(packet_seq_num),
                "packetSendingTime", BCON_INT64(packet_sending_time));

        switch(mdp_header->templateId())
        {
            case 4:     // ChannelReset4
                this->AppendData(document, static_cast<mktdata::ChannelReset4*>(mdp_message), arraydestroy);
                break;
            case 27:     // MDInstrumentDefinitionFuture27
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionFuture27*>(mdp_message), arraydestroy);
                break;
            case 29:     // MDInstrumentDefinitionSpread29
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionSpread29*>(mdp_message), arraydestroy);
                break;
            case 30:     // SecurityStatus30
                this->AppendData(document, static_cast<mktdata::SecurityStatus30*>(mdp_message), arraydestroy);
                break;
            case 32:     // MDIncrementalRefreshBook32
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshBook32*>(mdp_message), arraydestroy);
                break;
            case 33:     // MDIncrementalRefreshDailyStatistics33
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshDailyStatistics33*>(mdp_message), arraydestroy);
                break;
            case 34:     // MDIncrementalRefreshLimitsBanding34
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshLimitsBanding34*>(mdp_message), arraydestroy);
                break;
            case 35:     // MDIncrementalRefreshSessionStatistics35
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshSessionStatistics35*>(mdp_message), arraydestroy);
                break;
            case 36:     // MDIncrementalRefreshTrade36
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshTrade36*>(mdp_message), arraydestroy);
                break;
            case 37:     // MDIncrementalRefreshVolume37
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshVolume37*>(mdp_message), arraydestroy);
                break;
            case 38:     // SnapshotFullRefresh38
                this->AppendData(document, static_cast<mktdata::SnapshotFullRefresh38*>(mdp_message), arraydestroy);
                break;
            case 39:     // QuoteRequest39
                this->AppendData(document, static_cast<mktdata::QuoteRequest39*>(mdp_message), arraydestroy);
                break;
            case 41:     // MDInstrumentDefinitionOption41
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionOption41*>(mdp_message), arraydestroy);
                break;
            case 42:     // MDIncrementalRefreshTradeSummary42
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshTradeSummary42*>(mdp_message), arraydestroy);
                break;
            case 43:     // MDIncrementalRefreshOrderBook43
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshOrderBook43*>(mdp_message), arraydestroy);
                break;
            case 44:     // SnapshotFullRefreshOrderBook44
                this->AppendData(document, static_cast<mktdata::SnapshotFullRefreshOrderBook44*>(mdp_message), arraydestroy);
                break;
            default:
                LOG_WARN("invalid message template id: ", mdp_header->templateId());
                break;
        }

        bson_error_t error; 
		size_t  length;
		char *str = bson_as_json (document, &length);
		
		if(length == 0)
		{
			LOG_TRACE("message length == 0");
		}
		
        if (length != 0 && mongoc_collection_insert (m_collection, MONGOC_INSERT_NONE, document, nullptr, &error))
        {
			LOG_TRACE("message inserted: ", length);
			LOG_TRACE("message inserted: ", str);           
        }
        else
        {
            LOG_ERROR("db insert error: ", error.message);
        }
		bson_free (str);
		DestroyBsonVector(arraydestroy);
        bson_destroy (document);
    }


    void MdpReceiver::AppendData(bson_t *document, mktdata::ChannelReset4 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("ChannelReset4"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"matchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"matchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"matchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"matchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"matchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"matchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"matchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );
		
        bson_t *mdEntries = bson_new();
        mktdata::ChannelReset4::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_UTF8(noMDEntries.mDEntryType()),
                    "applID", BCON_INT32((int)noMDEntries.applID())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
			destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
        destroyVector.push_back(mdEntries);		
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND(document, 
		    "type", BCON_UTF8("MDInstrumentDefinitionFuture27"), 
			"totNumReports", BCON_INT32((int)m->totNumReports()),
		    "securityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"lastUpdateTime", BCON_INT64(m->lastUpdateTime()),
			"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
			"ApplID", BCON_INT32(m->applID()),
			"MarketSegmentID", BCON_INT32(m->marketSegmentID()),
			"UnderlyingProduct", BCON_INT32(m->underlyingProduct()), 
			"SecurityExchange", BCON_UTF8(m->getSecurityExchangeAsString().c_str()),
			"SecurityGroup", BCON_UTF8(m->getSecurityGroupAsString().c_str()),
			"Asset", BCON_UTF8(m->getAssetAsString().c_str()),
			"Symbol", BCON_UTF8(m->getSymbolAsString().c_str()),
			"SecurityID", BCON_INT32(m->securityID()),
			"SecurityIDSource", BCON_UTF8(m->securityIDSource()),
			"SecurityType", BCON_UTF8(m->getSecurityTypeAsString().c_str()),
			"CFICode", BCON_UTF8(m->getCFICodeAsString().c_str()),
			"MaturityMonthYear",  BCON_UTF8((std::to_string(m->maturityMonthYear().year()) + "-" 
			                                     + std::to_string(m->maturityMonthYear().month()) + "-" 
												 + std::to_string(m->maturityMonthYear().day())).c_str()),
			"Currency", BCON_UTF8(m->getCurrencyAsString().c_str()),
			"SettlCurrency", BCON_UTF8(m->getSettlCurrencyAsString().c_str()),
			"MatchAlgorithm", BCON_INT32((int)m->matchAlgorithm()),
			"MinTradeVol", BCON_INT32(m->minTradeVol()),
			"MaxTradeVol", BCON_INT32(m->maxTradeVol()),
			"MinPriceIncrementMantissa", BCON_INT64(m->minPriceIncrement().mantissa()),
			"MinPriceIncrementExponent", BCON_INT32(m->minPriceIncrement().exponent()),
			"DisplayFactorMantissa", BCON_INT64(m->displayFactor().mantissa()),
			"DisplayFactorExponent", BCON_INT32(m->displayFactor().exponent()),
			"MainFraction", BCON_INT32(m->mainFraction()),
			"SubFraction", BCON_INT32(m->subFraction()),
			"PriceDisplayFormat", BCON_INT32(m->priceDisplayFormat()),
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"UnitOfMeasureQtyMantissa", BCON_INT64(m->unitOfMeasureQty().mantissa()),
			"UnitOfMeasureQtyExponent", BCON_INT32(m->unitOfMeasureQty().exponent()),
			"TradingReferencePriceMantissa", BCON_INT64(m->tradingReferencePrice().mantissa()),
			"TradingReferencePriceExponent", BCON_INT32(m->tradingReferencePrice().exponent()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),
			"OpenInterestQty", BCON_INT32(m->openInterestQty()),
			"ClearedVolume", BCON_INT32(m->clearedVolume()),
			"HighLimitPriceMantissa", BCON_INT64(m->highLimitPrice().mantissa()),
			"HighLimitPriceExponent", BCON_INT32(m->highLimitPrice().exponent()),
			"LowLimitPriceMantissa", BCON_INT64(m->lowLimitPrice().mantissa()),
			"LowLimitPriceExponent", BCON_INT32(m->lowLimitPrice().exponent()),
			"MaxPriceVariationMantissa", BCON_INT64(m->maxPriceVariation().mantissa()),
			"MaxPriceVariationExponent", BCON_INT32(m->maxPriceVariation().exponent()),			
			"DecayQuantity", BCON_INT32(m->decayQuantity()),
			"DecayStartDate", BCON_INT32(m->decayStartDate()),
			"OriginalContractSize", BCON_INT32(m->originalContractSize()),
			"ContractMultiplier", BCON_INT32(m->contractMultiplier()),
			"ContractMultiplierUnit", BCON_INT32(m->contractMultiplierUnit()),
			"FlowScheduleType", BCON_INT32(m->flowScheduleType()),
			"MinPriceIncrementAmountMantissa", BCON_INT64(m->minPriceIncrementAmount().mantissa()),
			"MinPriceIncrementAmountExponent", BCON_INT32(m->minPriceIncrementAmount().exponent()),			
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"tradingReferenceDate", BCON_INT32(m->tradingReferenceDate())             
		);
		
		bson_t *Events = bson_new();
        mktdata::MDInstrumentDefinitionFuture27::NoEvents& noEvents = m->noEvents();
        std::uint64_t index_noEvents = 0;
		while (noEvents.hasNext())
		{	
			noEvents.next();
            bson_t *temp_child = BCON_NEW (
                    "EventType", BCON_INT32(noEvents.eventType()),
                    "EventTime", BCON_INT64(noEvents.eventTime())
            );
            BSON_APPEND_DOCUMENT(Events, std::to_string(index_noEvents).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_noEvents ++;
		}
		BSON_APPEND_ARRAY(document, "NoEvents", Events);
		destroyVector.push_back(Events);		
		
		bson_t *MDFeedTypes = bson_new();
        mktdata::MDInstrumentDefinitionFuture27::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        std::uint64_t index_NoMDFeedTypes = 0;
		while (noMDFeedTypes.hasNext())
		{
			noMDFeedTypes.next();	
			bson_t *temp_child = BCON_NEW (
				"mdFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"marketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
            );
            BSON_APPEND_DOCUMENT(MDFeedTypes, std::to_string(index_NoMDFeedTypes).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoMDFeedTypes ++;
		}
		BSON_APPEND_ARRAY(document, "NoMDFeedTypes", MDFeedTypes);
		destroyVector.push_back(MDFeedTypes);
		
		bson_t *InstAttrib = bson_new();
        mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& is = m->noInstAttrib();
        std::uint64_t index_NoInstAttrib = 0;
		while (is.hasNext())
		{
			is.next();
			bson_t *temp_child = BCON_NEW (
				"InstAttribType", BCON_INT32(is.instAttribType()), "InstAttribValueElectronicMatchEligible", BCON_INT32(is.instAttribValue().electronicMatchEligible()), 
				"InstAttribValueOrderCrossEligible", BCON_INT32(is.instAttribValue().orderCrossEligible()), 
				"InstAttribValueBlockTradeEligible", BCON_INT32(is.instAttribValue().blockTradeEligible()), 
				"InstAttribValueEFPEligible", BCON_INT32(is.instAttribValue().eFPEligible()), 
				"InstAttribValueEBFEligible", BCON_INT32(is.instAttribValue().eBFEligible()), 
				"InstAttribValueEFSEligible", BCON_INT32(is.instAttribValue().eFSEligible()), 
				"InstAttribValueEFREligible", BCON_INT32(is.instAttribValue().eFREligible()), 
				"InstAttribValueOTCEligible", BCON_INT32(is.instAttribValue().oTCEligible()), 
				"InstAttribValueiLinkIndicativeMassQuotingEligible", BCON_INT32(is.instAttribValue().iLinkIndicativeMassQuotingEligible()), 
				"InstAttribValueNegativeStrikeEligible", BCON_INT32(is.instAttribValue().negativeStrikeEligible()), 
				"InstAttribValueNegativePriceOutrightEligible", BCON_INT32(is.instAttribValue().negativePriceOutrightEligible()), 
				"InstAttribValueIsFractional", BCON_INT32(is.instAttribValue().isFractional()), 
				"InstAttribValueVolatilityQuotedOption", BCON_INT32(is.instAttribValue().volatilityQuotedOption()), 
				"InstAttribValueRFQCrossEligible", BCON_INT32(is.instAttribValue().rFQCrossEligible()), 
				"InstAttribValueZeroPriceOutrightEligible", BCON_INT32(is.instAttribValue().zeroPriceOutrightEligible()), 
				"InstAttribValueDecayingProductEligibility", BCON_INT32(is.instAttribValue().decayingProductEligibility()), 
				"InstAttribValueVariableProductEligibility", BCON_INT32(is.instAttribValue().variableProductEligibility()), 
				"InstAttribValueDailyProductEligibility", BCON_INT32(is.instAttribValue().dailyProductEligibility()), 
				"InstAttribValueGTOrdersEligibility", BCON_INT32(is.instAttribValue().gTOrdersEligibility()), 
				"InstAttribValueImpliedMatchingEligibility", BCON_INT32(is.instAttribValue().impliedMatchingEligibility())
            );
            BSON_APPEND_DOCUMENT(InstAttrib, std::to_string(index_NoInstAttrib).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoInstAttrib ++;
		}
		BSON_APPEND_ARRAY(document, "NoInstAttrib", InstAttrib);		
        destroyVector.push_back(InstAttrib);
		
		bson_t *LotTypeRules = bson_new();
        mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& ts = m->noLotTypeRules();
        std::uint64_t index_NoLotTypeRules = 0;
		while (ts.hasNext())
		{
			ts.next();
			bson_t *temp_child = BCON_NEW (
				"LotType", BCON_INT32(ts.lotType()), "MinLotSizeMantissa",  BCON_INT32(ts.minLotSize().mantissa()), "MinLotSizeExponent", BCON_INT32(ts.minLotSize().exponent())
            );
            BSON_APPEND_DOCUMENT(LotTypeRules, std::to_string(index_NoLotTypeRules).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoLotTypeRules ++;
		}
		BSON_APPEND_ARRAY(document, "NoLotTypeRules", LotTypeRules);
        destroyVector.push_back(LotTypeRules);		
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND(document, 
		    "type", BCON_UTF8("MDInstrumentDefinitionSpread29"), 
			"totNumReports", BCON_INT32((int)m->totNumReports()),
		    "securityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"lastUpdateTime", BCON_INT64(m->lastUpdateTime()),
			"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
			"ApplID", BCON_INT32(m->applID()),
			"MarketSegmentID", BCON_INT32(m->marketSegmentID()),
			"UnderlyingProduct", BCON_INT32(m->underlyingProduct()), 
			"SecurityExchange", BCON_UTF8(m->getSecurityExchangeAsString().c_str()),
			"SecurityGroup", BCON_UTF8(m->getSecurityGroupAsString().c_str()),
			"Asset", BCON_UTF8(m->getAssetAsString().c_str()),
			"Symbol", BCON_UTF8(m->getSymbolAsString().c_str()),
			"SecurityID", BCON_INT32(m->securityID()),
			"SecurityIDSource", BCON_UTF8(m->securityIDSource()),
			"SecurityType", BCON_UTF8(m->getSecurityTypeAsString().c_str()),
			"CFICode", BCON_UTF8(m->getCFICodeAsString().c_str()),
			"MaturityMonthYear",  BCON_UTF8((std::to_string(m->maturityMonthYear().year()) + "-" 
			                                     + std::to_string(m->maturityMonthYear().month()) + "-" 
												 + std::to_string(m->maturityMonthYear().day())).c_str()),
			"Currency", BCON_UTF8(m->getCurrencyAsString().c_str()),
			"SecuritySubType", BCON_UTF8(m->getSecuritySubTypeAsString().c_str()), 
			"PriceRatioMantissa", BCON_INT64(m->priceRatio().mantissa()),
			"PriceRatioExponent", BCON_INT32(m->priceRatio().exponent()),
			"TickRule", BCON_INT32(m->tickRule()),               
			"MatchAlgorithm", BCON_INT32((int)m->matchAlgorithm()),
			"MinTradeVol", BCON_INT32(m->minTradeVol()),
			"MaxTradeVol", BCON_INT32(m->maxTradeVol()),
			"MinPriceIncrementMantissa", BCON_INT64(m->minPriceIncrement().mantissa()),
			"MinPriceIncrementExponent", BCON_INT32(m->minPriceIncrement().exponent()),
			"DisplayFactorMantissa", BCON_INT64(m->displayFactor().mantissa()),
			"DisplayFactorExponent", BCON_INT32(m->displayFactor().exponent()),
			"MainFraction", BCON_INT32(m->mainFraction()),
			"SubFraction", BCON_INT32(m->subFraction()),
			"PriceDisplayFormat", BCON_INT32(m->priceDisplayFormat()),
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"TradingReferencePriceMantissa", BCON_INT64(m->tradingReferencePrice().mantissa()),
			"TradingReferencePriceExponent", BCON_INT32(m->tradingReferencePrice().exponent()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),
			"OpenInterestQty", BCON_INT32(m->openInterestQty()),
			"ClearedVolume", BCON_INT32(m->clearedVolume()),
			"HighLimitPriceMantissa", BCON_INT64(m->highLimitPrice().mantissa()),
			"HighLimitPriceExponent", BCON_INT32(m->highLimitPrice().exponent()),
			"LowLimitPriceMantissa", BCON_INT64(m->lowLimitPrice().mantissa()),
			"LowLimitPriceExponent", BCON_INT32(m->lowLimitPrice().exponent()),
			"MaxPriceVariationMantissa", BCON_INT64(m->maxPriceVariation().mantissa()),
			"MaxPriceVariationExponent", BCON_INT32(m->maxPriceVariation().exponent()),						
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"tradingReferenceDate", BCON_INT32(m->tradingReferenceDate())             
		);

		bson_t *Events = bson_new();
        mktdata::MDInstrumentDefinitionSpread29::NoEvents& noEvents = m->noEvents();
        std::uint64_t index_noEvents = 0;
		while (noEvents.hasNext())
		{	
			noEvents.next();
            bson_t *temp_child = BCON_NEW (
                    "EventType", BCON_INT32(noEvents.eventType()),
                    "EventTime", BCON_INT64(noEvents.eventTime())
            );
            BSON_APPEND_DOCUMENT(Events, std::to_string(index_noEvents).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_noEvents ++;
		}
		BSON_APPEND_ARRAY(document, "NoEvents", Events);
		destroyVector.push_back(Events);	
		
		bson_t *MDFeedTypes = bson_new();
        mktdata::MDInstrumentDefinitionSpread29::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        std::uint64_t index_NoMDFeedTypes = 0;
		while (noMDFeedTypes.hasNext())
		{
			noMDFeedTypes.next();	
			bson_t *temp_child = BCON_NEW (
				"mdFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"marketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
            );
            BSON_APPEND_DOCUMENT(MDFeedTypes, std::to_string(index_NoMDFeedTypes).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoMDFeedTypes ++;
		}
		BSON_APPEND_ARRAY(document, "NoMDFeedTypes", MDFeedTypes);
		destroyVector.push_back(MDFeedTypes);
		
		bson_t *InstAttrib = bson_new();
        mktdata::MDInstrumentDefinitionSpread29::NoInstAttrib& is = m->noInstAttrib();
        std::uint64_t index_NoInstAttrib = 0;
		while (is.hasNext())
		{
			is.next();
			bson_t *temp_child = BCON_NEW (
				"InstAttribType", BCON_INT32(is.instAttribType()), "InstAttribValueElectronicMatchEligible", BCON_INT32(is.instAttribValue().electronicMatchEligible()), 
				"InstAttribValueOrderCrossEligible", BCON_INT32(is.instAttribValue().orderCrossEligible()), 
				"InstAttribValueBlockTradeEligible", BCON_INT32(is.instAttribValue().blockTradeEligible()), 
				"InstAttribValueEFPEligible", BCON_INT32(is.instAttribValue().eFPEligible()), 
				"InstAttribValueEBFEligible", BCON_INT32(is.instAttribValue().eBFEligible()), 
				"InstAttribValueEFSEligible", BCON_INT32(is.instAttribValue().eFSEligible()), 
				"InstAttribValueEFREligible", BCON_INT32(is.instAttribValue().eFREligible()), 
				"InstAttribValueOTCEligible", BCON_INT32(is.instAttribValue().oTCEligible()), 
				"InstAttribValueiLinkIndicativeMassQuotingEligible", BCON_INT32(is.instAttribValue().iLinkIndicativeMassQuotingEligible()), 
				"InstAttribValueNegativeStrikeEligible", BCON_INT32(is.instAttribValue().negativeStrikeEligible()), 
				"InstAttribValueNegativePriceOutrightEligible", BCON_INT32(is.instAttribValue().negativePriceOutrightEligible()), 
				"InstAttribValueIsFractional", BCON_INT32(is.instAttribValue().isFractional()), 
				"InstAttribValueVolatilityQuotedOption", BCON_INT32(is.instAttribValue().volatilityQuotedOption()), 
				"InstAttribValueRFQCrossEligible", BCON_INT32(is.instAttribValue().rFQCrossEligible()), 
				"InstAttribValueZeroPriceOutrightEligible", BCON_INT32(is.instAttribValue().zeroPriceOutrightEligible()), 
				"InstAttribValueDecayingProductEligibility", BCON_INT32(is.instAttribValue().decayingProductEligibility()), 
				"InstAttribValueVariableProductEligibility", BCON_INT32(is.instAttribValue().variableProductEligibility()), 
				"InstAttribValueDailyProductEligibility", BCON_INT32(is.instAttribValue().dailyProductEligibility()), 
				"InstAttribValueGTOrdersEligibility", BCON_INT32(is.instAttribValue().gTOrdersEligibility()), 
				"InstAttribValueImpliedMatchingEligibility", BCON_INT32(is.instAttribValue().impliedMatchingEligibility())
            );
            BSON_APPEND_DOCUMENT(InstAttrib, std::to_string(index_NoInstAttrib).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoInstAttrib ++;
		}
		BSON_APPEND_ARRAY(document, "NoInstAttrib", InstAttrib);		
        destroyVector.push_back(InstAttrib);
		
		bson_t *LotTypeRules = bson_new();
        mktdata::MDInstrumentDefinitionSpread29::NoLotTypeRules& ts = m->noLotTypeRules();
        std::uint64_t index_NoLotTypeRules = 0;
		while (ts.hasNext())
		{
			ts.next();
			bson_t *temp_child = BCON_NEW (
				"LotType", BCON_INT32(ts.lotType()), "MinLotSizeMantissa",  BCON_INT32(ts.minLotSize().mantissa()), "MinLotSizeExponent", BCON_INT32(ts.minLotSize().exponent())
            );
            BSON_APPEND_DOCUMENT(LotTypeRules, std::to_string(index_NoLotTypeRules).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoLotTypeRules ++;
		}
		BSON_APPEND_ARRAY(document, "NoLotTypeRules", LotTypeRules);	
        destroyVector.push_back(LotTypeRules);		

		bson_t *Legs = bson_new();
        mktdata::MDInstrumentDefinitionSpread29::NoLegs& lgs = m->noLegs();
        std::uint64_t index_NoLegs = 0;
		while (lgs.hasNext())
		{
			ts.next();
			bson_t *temp_child = BCON_NEW (
				"LegSecurityID", BCON_INT32(lgs.legSecurityID()), "LegSecurityIDSource",  BCON_UTF8(lgs.legSecurityIDSource()), "LegSide", BCON_INT32(lgs.legSide()),
				"LegRatioQty", BCON_INT32(lgs.legRatioQty()), "LegPriceMantissa",  BCON_INT64(lgs.legPrice().mantissa()),  "LegPriceExponent",  BCON_INT32(lgs.legPrice().exponent()), 
				"legOptionDeltaMantissa",  BCON_INT64(lgs.legOptionDelta().mantissa()),  "legOptionDeltaExponent",  BCON_INT32(lgs.legOptionDelta().exponent())
            );
            BSON_APPEND_DOCUMENT(Legs, std::to_string(index_NoLotTypeRules).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoLegs++;
		}
		BSON_APPEND_ARRAY(document, "NoLegs", Legs);
		destroyVector.push_back(Legs);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SecurityStatus30 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("SecurityStatus30"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "SecurityGroup", BCON_UTF8 (m->securityGroup()),
                "asset", BCON_UTF8 (m->asset()),
                "securityID", BCON_INT32 (m->securityID()),
                "tradeDate", BCON_INT32(m->tradeDate()),
                "matchEventIndicator", BCON_UTF8(m->matchEventIndicator().buffer()),
                "securityTradingStatus", BCON_INT32(m->securityTradingStatus()),
                "haltReason", BCON_INT32(m->haltReason()),
                "securityTradingEvent", BCON_INT32(m->securityTradingEvent())
        );
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshBook32 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshBook32"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDEntryPx", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
                    "mDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                    "numberOfOrders", BCON_INT32 ((int)noMDEntries.numberOfOrders()),
                    "mDPriceLevel", BCON_INT32((int)noMDEntries.mDPriceLevel()),
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
        destroyVector.push_back(mdEntries);
		
        bson_t *orderIdEntries = bson_new();
        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        std::uint64_t index_order = 0;
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "OrderID", BCON_INT32(noOrderIDEntries.orderID()),
                    "MDOrderPriority", BCON_INT32 ( (int)noOrderIDEntries.mDOrderPriority()),
                    "MDDisplayQty", BCON_INT32 ((int)noOrderIDEntries.mDDisplayQty()),
                    "ReferenceID", BCON_INT32 ((int)noOrderIDEntries.referenceID()),
                    "OrderUpdateAction", BCON_INT32 ((int)noOrderIDEntries.orderUpdateAction())
            );
            BSON_APPEND_DOCUMENT(orderIdEntries, std::to_string(index_order).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_order ++;
        }
        BSON_APPEND_ARRAY(document, "noOrderIDEntries", orderIdEntries);
        destroyVector.push_back(orderIdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshDailyStatistics33 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshDailyStatistics33"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );
           
        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDEntryPx", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
                    "mDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"tradingReferenceDate", BCON_INT32 ((int)noMDEntries.tradingReferenceDate()),
					"SettlPriceTypeFinal", BCON_INT32((int)noMDEntries.settlPriceType().finalrc()),
					"SettlPriceTypeActual", BCON_INT32(noMDEntries.settlPriceType().actual()),
					"SettlPriceTypeRounded", BCON_INT32(noMDEntries.settlPriceType().rounded()),
					"SettlPriceTypeIntraday", BCON_INT32(noMDEntries.settlPriceType().intraday()),
					"SettlPriceTypeReservedBits", BCON_INT32(noMDEntries.settlPriceType().reservedBits()),
					"SettlPriceTypeNullValue", BCON_INT32(noMDEntries.settlPriceType().nullValue()),
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshLimitsBanding34"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "HighLimitPriceMantissa", BCON_INT64(noMDEntries.highLimitPrice().mantissa()),
					"HighLimitPriceExponent", BCON_INT32(noMDEntries.highLimitPrice().exponent()),
                    "LowLimitPriceMantissa", BCON_INT32 (noMDEntries.lowLimitPrice().mantissa()),
					"LowLimitPriceExponent", BCON_INT32 (noMDEntries.lowLimitPrice().exponent()),
					"MaxPriceVariationMantissa", BCON_INT64(noMDEntries.maxPriceVariation().mantissa()),
			        "MaxPriceVariationExponent", BCON_INT32(noMDEntries.maxPriceVariation().exponent()),	
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_UTF8(noMDEntries.mDEntryType())    
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshSessionStatistics35 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshSessionStatistics35"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDEntryPxMantissa", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
					"mDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"securityID", BCON_INT32 ((int)noMDEntries.securityID()),
					"rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                    "OpenCloseSettlFlag", BCON_INT32((int)noMDEntries.openCloseSettlFlag()),
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType()),
                    "mDEntrySize", BCON_INT32((int)noMDEntries.mDEntrySize())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshTrade36 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshDailyStatistics33"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshTrade36::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDEntryPx", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
                    "mDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"numberOfOrders", BCON_INT32 ((int)noMDEntries.numberOfOrders()),
					"TradeID", BCON_INT32 ((int)noMDEntries.tradeIDId()),
					"AggressorSide", BCON_INT32((int)noMDEntries.aggressorSide()),
					"mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_UTF8(noMDEntries.mDEntryType())        
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshVolume37 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshVolume37"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
			    "mDEntrySize", BCON_INT32((int)noMDEntries.mDEntrySize()),
				"securityID", BCON_INT32 ((int)noMDEntries.securityID()),
				"rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                "mDEntryType", BCON_UTF8(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SnapshotFullRefresh38 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("SnapshotFullRefresh38"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "LastMsgSeqNumProcessed", BCON_INT32 (m->lastMsgSeqNumProcessed()),
				"TotNumReports", BCON_INT32 (m->totNumReports()),
				"SecurityID", BCON_INT32 (m->securityID()),
				"rptSeq", BCON_INT32 (m->rptSeq()),
				"lastUpdateTime", BCON_INT64(m->lastUpdateTime()),
				 "tradeDate", BCON_INT32(m->tradeDate()),
				"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
				"LowLimitPriceMantissa", BCON_INT64(m->lowLimitPrice().mantissa()),
				"LowLimitPriceExponent", BCON_INT32(m->lowLimitPrice().exponent()),
				"HighLimitPriceMantissa", BCON_INT64(m->highLimitPrice().mantissa()),
				"HighLimitPriceExponent", BCON_INT32(m->highLimitPrice().exponent()),
				"MaxPriceVariationMantissa", BCON_INT64(m->maxPriceVariation().mantissa())
        );

        bson_t *mdEntries = bson_new();
        mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
			        "mDEntryPxMantissa", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
					"mDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"mDEntrySize", BCON_INT32((int)noMDEntries.mDEntrySize()),
					"numberOfOrders", BCON_INT32 ((int)noMDEntries.numberOfOrders()),
					"mDPriceLevel", BCON_INT32((int)noMDEntries.mDPriceLevel()),	
					"tradingReferenceDate", BCON_INT32(noMDEntries.tradingReferenceDate()),
					"OpenCloseSettlFlag", BCON_INT32((int)noMDEntries.openCloseSettlFlag()),
					"SettlPriceTypeFinal", BCON_INT32((int)noMDEntries.settlPriceType().finalrc()),
					"SettlPriceTypeActual", BCON_INT32(noMDEntries.settlPriceType().actual()),
					"SettlPriceTypeRounded", BCON_INT32(noMDEntries.settlPriceType().rounded()),
					"SettlPriceTypeIntraday", BCON_INT32(noMDEntries.settlPriceType().intraday()),
					"SettlPriceTypeReservedBits", BCON_INT32(noMDEntries.settlPriceType().reservedBits()),
					"SettlPriceTypeNullValue", BCON_INT32(noMDEntries.settlPriceType().nullValue()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::QuoteRequest39 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("QuoteRequest39"),
                "transactTime", BCON_INT64 (m->transactTime()),
				"QuoteReqID", BCON_UTF8 (m->quoteReqID()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *relatedSym = bson_new();
        mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym = m->noRelatedSym();
        std::uint64_t index_md = 0;
        while (noRelatedSym.hasNext())
        {
            noRelatedSym.next();
            bson_t *temp_child = BCON_NEW (
			        "Symbol", BCON_UTF8(noRelatedSym.getSymbolAsString().c_str()),
					"securityID", BCON_INT32 ((int)noRelatedSym.securityID()),
					"OrderQty", BCON_INT32(noRelatedSym.orderQty()),
                    "QuoteType", BCON_INT32(noRelatedSym.quoteType()),
					"Side", BCON_INT32(noRelatedSym.side())
            );
            BSON_APPEND_DOCUMENT(relatedSym, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;
        }
        BSON_APPEND_ARRAY(document, "NoRelatedSym", relatedSym);
		destroyVector.push_back(relatedSym);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND(document, 
		    "type", BCON_UTF8("MDInstrumentDefinitionOption41"), 
			"totNumReports", BCON_INT32((int)m->totNumReports()),
		    "securityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"lastUpdateTime", BCON_INT64(m->lastUpdateTime()),
			"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
			"ApplID", BCON_INT32(m->applID()),
			"MarketSegmentID", BCON_INT32(m->marketSegmentID()),
			"UnderlyingProduct", BCON_INT32(m->underlyingProduct()), 
			"SecurityExchange", BCON_UTF8(m->getSecurityExchangeAsString().c_str()),
			"SecurityGroup", BCON_UTF8(m->getSecurityGroupAsString().c_str()),
			"Asset", BCON_UTF8(m->getAssetAsString().c_str()),
			"Symbol", BCON_UTF8(m->getSymbolAsString().c_str()),
			"SecurityID", BCON_INT32(m->securityID()),
			"SecurityIDSource", BCON_UTF8(m->securityIDSource()),
			"SecurityType", BCON_UTF8(m->getSecurityTypeAsString().c_str()),
			"CFICode", BCON_UTF8(m->getCFICodeAsString().c_str()),
			"PutOrCall", BCON_INT32(m->putOrCall()),
			"MaturityMonthYear",  BCON_UTF8((std::to_string(m->maturityMonthYear().year()) + "-" 
			                                     + std::to_string(m->maturityMonthYear().month()) + "-" 
												 + std::to_string(m->maturityMonthYear().day())).c_str()),
			"Currency", BCON_UTF8(m->getCurrencyAsString().c_str()),
			"StrikePriceMantissa", BCON_INT64(m->strikePrice().mantissa()),
			"StrikePriceExponent", BCON_INT32(m->strikePrice().exponent()),
			"StrikeCurrency", BCON_UTF8(m->getStrikeCurrencyAsString().c_str()),
			"SettlCurrency", BCON_UTF8(m->getSettlCurrencyAsString().c_str()),
			"MinCabPriceMantissa", BCON_INT64(m->minCabPrice().mantissa()),
			"MinCabPriceExponent", BCON_INT32(m->minCabPrice().exponent()),
			"MatchAlgorithm", BCON_INT32((int)m->matchAlgorithm()),
			"MinTradeVol", BCON_INT32(m->minTradeVol()),
			"MaxTradeVol", BCON_INT32(m->maxTradeVol()),
			"MinPriceIncrementMantissa", BCON_INT64(m->minPriceIncrement().mantissa()),
			"MinPriceIncrementExponent", BCON_INT32(m->minPriceIncrement().exponent()),
			"MinPriceIncrementAmountMantissa", BCON_INT64(m->minPriceIncrementAmount().mantissa()),
			"MinPriceIncrementAmountExponent", BCON_INT32(m->minPriceIncrementAmount().exponent()),				
			"DisplayFactorMantissa", BCON_INT64(m->displayFactor().mantissa()),
			"DisplayFactorExponent", BCON_INT32(m->displayFactor().exponent()),
			"TickRule", BCON_INT32(m->tickRule()),   
			"MainFraction", BCON_INT32(m->mainFraction()),
			"SubFraction", BCON_INT32(m->subFraction()),
			"PriceDisplayFormat", BCON_INT32(m->priceDisplayFormat()),
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"UnitOfMeasureQtyMantissa", BCON_INT64(m->unitOfMeasureQty().mantissa()),
			"UnitOfMeasureQtyExponent", BCON_INT32(m->unitOfMeasureQty().exponent()),
			"TradingReferencePriceMantissa", BCON_INT64(m->tradingReferencePrice().mantissa()),
			"TradingReferencePriceExponent", BCON_INT32(m->tradingReferencePrice().exponent()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),
			"OpenInterestQty", BCON_INT32(m->openInterestQty()),
			"LowLimitPriceMantissa", BCON_INT64(m->lowLimitPrice().mantissa()),
			"LowLimitPriceExponent", BCON_INT32(m->lowLimitPrice().exponent()),
			"HighLimitPriceMantissa", BCON_INT64(m->highLimitPrice().mantissa()),
			"HighLimitPriceExponent", BCON_INT32(m->highLimitPrice().exponent()),
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"tradingReferenceDate", BCON_INT32(m->tradingReferenceDate())            
		);
		
		bson_t *Events = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoEvents& noEvents = m->noEvents();
        std::uint64_t index_noEvents = 0;
		while (noEvents.hasNext())
		{	
			noEvents.next();
            bson_t *temp_child = BCON_NEW (
                    "EventType", BCON_INT32(noEvents.eventType()),
                    "EventTime", BCON_INT64(noEvents.eventTime())
            );
            BSON_APPEND_DOCUMENT(Events, std::to_string(index_noEvents).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_noEvents++;
		}
		BSON_APPEND_ARRAY(document, "NoEvents", Events);
		destroyVector.push_back(Events);
		
		bson_t *MDFeedTypes = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoMDFeedTypes& noMDFeedTypes = m->noMDFeedTypes();
        std::uint64_t index_NoMDFeedTypes = 0;
		while (noMDFeedTypes.hasNext())
		{
			noMDFeedTypes.next();	
			bson_t *temp_child = BCON_NEW (
				"mdFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"marketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
            );
            BSON_APPEND_DOCUMENT(MDFeedTypes, std::to_string(index_NoMDFeedTypes).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoMDFeedTypes++;
		}
		BSON_APPEND_ARRAY(document, "NoMDFeedTypes", MDFeedTypes);
		destroyVector.push_back(MDFeedTypes);
		
		bson_t *InstAttrib = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoInstAttrib& is = m->noInstAttrib();
        std::uint64_t index_NoInstAttrib = 0;
		while (is.hasNext())
		{
			is.next();
			bson_t *temp_child = BCON_NEW (
				"InstAttribType", BCON_INT32(is.instAttribType()), "InstAttribValueElectronicMatchEligible", BCON_INT32(is.instAttribValue().electronicMatchEligible()), 
				"InstAttribValueOrderCrossEligible", BCON_INT32(is.instAttribValue().orderCrossEligible()), 
				"InstAttribValueBlockTradeEligible", BCON_INT32(is.instAttribValue().blockTradeEligible()), 
				"InstAttribValueEFPEligible", BCON_INT32(is.instAttribValue().eFPEligible()), 
				"InstAttribValueEBFEligible", BCON_INT32(is.instAttribValue().eBFEligible()), 
				"InstAttribValueEFSEligible", BCON_INT32(is.instAttribValue().eFSEligible()), 
				"InstAttribValueEFREligible", BCON_INT32(is.instAttribValue().eFREligible()), 
				"InstAttribValueOTCEligible", BCON_INT32(is.instAttribValue().oTCEligible()), 
				"InstAttribValueiLinkIndicativeMassQuotingEligible", BCON_INT32(is.instAttribValue().iLinkIndicativeMassQuotingEligible()), 
				"InstAttribValueNegativeStrikeEligible", BCON_INT32(is.instAttribValue().negativeStrikeEligible()), 
				"InstAttribValueNegativePriceOutrightEligible", BCON_INT32(is.instAttribValue().negativePriceOutrightEligible()), 
				"InstAttribValueIsFractional", BCON_INT32(is.instAttribValue().isFractional()), 
				"InstAttribValueVolatilityQuotedOption", BCON_INT32(is.instAttribValue().volatilityQuotedOption()), 
				"InstAttribValueRFQCrossEligible", BCON_INT32(is.instAttribValue().rFQCrossEligible()), 
				"InstAttribValueZeroPriceOutrightEligible", BCON_INT32(is.instAttribValue().zeroPriceOutrightEligible()), 
				"InstAttribValueDecayingProductEligibility", BCON_INT32(is.instAttribValue().decayingProductEligibility()), 
				"InstAttribValueVariableProductEligibility", BCON_INT32(is.instAttribValue().variableProductEligibility()), 
				"InstAttribValueDailyProductEligibility", BCON_INT32(is.instAttribValue().dailyProductEligibility()), 
				"InstAttribValueGTOrdersEligibility", BCON_INT32(is.instAttribValue().gTOrdersEligibility()), 
				"InstAttribValueImpliedMatchingEligibility", BCON_INT32(is.instAttribValue().impliedMatchingEligibility())
            );
            BSON_APPEND_DOCUMENT(InstAttrib, std::to_string(index_NoInstAttrib).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoInstAttrib++;
		}
		BSON_APPEND_ARRAY(document, "NoInstAttrib", InstAttrib);	
		destroyVector.push_back(InstAttrib);		

		bson_t *LotTypeRules = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoLotTypeRules& ts = m->noLotTypeRules();
        std::uint64_t index_NoLotTypeRules = 0;
		while (ts.hasNext())
		{
			ts.next();
			bson_t *temp_child = BCON_NEW (
				"LotType", BCON_INT32(ts.lotType()), "MinLotSizeMantissa",  BCON_INT32(ts.minLotSize().mantissa()), "MinLotSizeExponent", BCON_INT32(ts.minLotSize().exponent())
            );
            BSON_APPEND_DOCUMENT(LotTypeRules, std::to_string(index_NoLotTypeRules).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoLotTypeRules ++;
		}
		BSON_APPEND_ARRAY(document, "NoLotTypeRules", LotTypeRules);	
		destroyVector.push_back(LotTypeRules);
		
		bson_t *Underlyings = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& under = m->noUnderlyings();
        std::uint64_t index_NoUnderlyings = 0;
		while (under.hasNext())
		{
			under.next();
			bson_t *temp_child = BCON_NEW (
				"UnderlyingSecurityID", BCON_INT32(under.underlyingSecurityID()), "UnderlyingSecurityIDSource",  BCON_UTF8(under.underlyingSecurityIDSource()), "UnderlyingSymbol", BCON_UTF8(under.underlyingSymbol())
            );
            BSON_APPEND_DOCUMENT(Underlyings, std::to_string(index_NoUnderlyings).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoUnderlyings ++;
		}
		BSON_APPEND_ARRAY(document, "NoUnderlyings", Underlyings);	
        destroyVector.push_back(Underlyings);		
		
		bson_t *RelatedInstruments = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoRelatedInstruments& ri = m->noRelatedInstruments();
        std::uint64_t index_RelatedInstruments = 0;
		while (ri.hasNext())
		{
			ri.next();
			bson_t *temp_child = BCON_NEW (
				"RelatedSecurityID", BCON_INT32(ri.relatedSecurityID()), "RelatedSecurityIDSource",  BCON_UTF8(ri.relatedSecurityIDSource()), "RelatedSymbol", BCON_UTF8(ri.relatedSymbol())
            );
            BSON_APPEND_DOCUMENT(RelatedInstruments, std::to_string(index_RelatedInstruments).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_RelatedInstruments ++;
		}
		BSON_APPEND_ARRAY(document, "NoUnderlyings", RelatedInstruments);	
		destroyVector.push_back(RelatedInstruments);		
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshTradeSummary42"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "mDEntryPxMantissa", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
					"mDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
                    "mDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "rptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                    "numberOfOrders", BCON_INT32 ((int)noMDEntries.numberOfOrders()),
					"AggressorSide", BCON_INT32((int)noMDEntries.aggressorSide()),
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_UTF8(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
        destroyVector.push_back(mdEntries);	
		
        bson_t *orderIdEntries = bson_new();
        mktdata::MDIncrementalRefreshTradeSummary42::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        std::uint64_t index_order = 0;
        while (noOrderIDEntries.hasNext())
        {
            noOrderIDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "OrderID", BCON_INT32(noOrderIDEntries.orderID()),
                    "lastQty", BCON_INT32(noOrderIDEntries.lastQty())
            );
            BSON_APPEND_DOCUMENT(orderIdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_order++;
        }
        BSON_APPEND_ARRAY(document, "noOrderIDEntries", orderIdEntries);
		destroyVector.push_back(orderIdEntries);	
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshOrderBook43"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
			        "OrderID", BCON_INT32(noMDEntries.orderID()),
					"mDOrderPriority", BCON_INT64(noMDEntries.mDOrderPriority()),
                    "mDEntryPxMantissa", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
					"mDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"mDDisplayQty", BCON_INT32(noMDEntries.mDDisplayQty()),
                    "securityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "mDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SnapshotFullRefreshOrderBook44 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("SnapshotFullRefreshOrderBook44"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "LastMsgSeqNumProcessed", BCON_INT32 (m->lastMsgSeqNumProcessed()),
				"TotNumReports", BCON_INT32 (m->totNumReports()),
				"SecurityID", BCON_INT32 (m->securityID()),
				"NoChunks", BCON_INT32 (m->noChunks()),
				"CurrentChunk", BCON_INT32 (m->currentChunk())
        );

        bson_t *mdEntries = bson_new();
        mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
			        "OrderID", BCON_INT32(noMDEntries.orderID()),
					"mDOrderPriority", BCON_INT64(noMDEntries.mDOrderPriority()),
                    "mDEntryPxMantissa", BCON_INT32(noMDEntries.mDEntryPx().mantissa()),
					"mDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"mDDisplayQty", BCON_INT32(noMDEntries.mDDisplayQty()),
                    "mDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

} // namespace persist
} // namespace market
} // namespace cme
} // namespace fh
