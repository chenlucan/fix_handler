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
            case 12: // AdminHeartbeat12
                LOG_INFO("message is Admin Heartbeat(12), ignore.");
                break;
            case 16: // AdminLogout16
                LOG_WARN("message is Admin Logout(16), strange! ignore.");
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

		size_t  length;
		char *str = bson_as_json (document, &length);
		LOG_TRACE("message: ", length == 0 ? "length == 0" : "(" + std::to_string(length) + ")" + str);

		if(length != 0)
		{
		    bson_error_t error;
		    if (mongoc_collection_insert (m_collection, MONGOC_INSERT_NONE, document, nullptr, &error))
            {
                LOG_TRACE("message inserted.");
            }
            else
            {
                LOG_ERROR("message insert error: ", error.message);
            }
		}

		bson_free (str);
		DestroyBsonVector(arraydestroy);
        bson_destroy (document);
    }

	template <typename T> 
	void MdpReceiver::MDEntrySizeIsNull(bson_t *document, T& noMDEntries)
	{
		std::int32_t size = noMDEntries.mDEntrySizeNullValue();
		std::int32_t entrySize = noMDEntries.mDEntrySize(); 
		if(entrySize != size)
			BCON_APPEND(document, "MDEntrySize", BCON_INT32(entrySize));	
	}

	template <typename T> 
    void MdpReceiver::OpenInterestQtyIsNull(bson_t *document, T *m)	
	{
		std::int32_t size = m->openInterestQtyNullValue();
		std::int32_t opensize = m->openInterestQty();
		if(opensize != size)
			BCON_APPEND(document, "OpenInterestQty", BCON_INT32(opensize));		
	}	

	template <typename T> 
    void MdpReceiver::ClearedVolumeIsNull(bson_t *document, T *m)
    {
		std::int32_t size = m->openInterestQtyNullValue();
		std::int32_t clearedsize = m->clearedVolume();
		if(clearedsize != size)
			BCON_APPEND(document, "ClearedVolume", BCON_INT32(clearedsize));			
	}	

	void MdpReceiver::DecayQuantityIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
	{
		std::int32_t size = m->decayQuantityNullValue();
		std::int32_t decaysize = m->decayQuantity();
		if(decaysize != size)
			BCON_APPEND(document, "DecayQuantity", BCON_INT32(decaysize));				
	}	
	
	void MdpReceiver::OriginalContractSizeIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
    {
		std::int32_t size = m->originalContractSizeNullValue();
		std::int32_t originalsize = m->originalContractSize();
		if(originalsize != size)
			BCON_APPEND(document, "OriginalContractSize", BCON_INT32(originalsize));		
	}

	void MdpReceiver::ContractMultiplierIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
	{
		std::int32_t size = m->contractMultiplierNullValue();
		std::int32_t contractsize = m->contractMultiplier();
		if(contractsize != size)
			BCON_APPEND(document, "ContractMultiplier", BCON_INT32(contractsize));				
	}
	
	void MdpReceiver::SecurityIDIsNull(bson_t *document, mktdata::SecurityStatus30 *m)	
	{
		std::int32_t size = m->securityIDNullValue();
		std::int32_t securitysize = m->securityID();
		if(securitysize != size)
			BCON_APPEND(document, "SecurityID", BCON_INT32(securitysize));						
	}
	
	template <typename T> 
	void MdpReceiver::NumberOfOrdersIsNull(bson_t *document, T& noMDEntries)
	{
		std::int32_t size = noMDEntries.numberOfOrdersNullValue();
		std::int32_t orderssize = noMDEntries.numberOfOrders();
		if(orderssize != size)
			BCON_APPEND(document, "NumberOfOrders", BCON_INT32(orderssize));							
	}	
	
	template <typename T> 
	void MdpReceiver::MDDisplayQtyIsNull(bson_t *document, T& noOrderIDEntries)
	{
		std::int32_t size = noOrderIDEntries.mDDisplayQtyNullValue();
		std::int32_t OrderIDsize = noOrderIDEntries.mDDisplayQty();
		if(OrderIDsize != size)
			BCON_APPEND(document, "MDDisplayQty", BCON_INT32(OrderIDsize));				
	}
	
	void MdpReceiver::OrderQtyIsNull(bson_t *document, mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym)
	{
		std::int32_t size = noRelatedSym.orderQtyNullValue();
		std::int32_t relatesize = noRelatedSym.orderQty();
		if(relatesize != size)
			BCON_APPEND(document, "OrderQty", BCON_INT32(relatesize));					
	}	
	
	template <typename T> 
	void MdpReceiver::TotNumReportsIsNull(bson_t *document, T *m)
	{
		std::int32_t size = m->totNumReportsNullValue();
		std::int32_t totnumsize = m->totNumReports();
		if(totnumsize != size)
			BCON_APPEND(document, "TotNumReports", BCON_INT32(totnumsize));			
	}
	
	void MdpReceiver::MDTradeEntryIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries)
	{
		std::int32_t size = noMDEntries.mDTradeEntryIDNullValue();
		std::int32_t tradeEntrysize = noMDEntries.mDTradeEntryID();
		if(tradeEntrysize != size)
			BCON_APPEND(document, "MDTradeEntryID", BCON_INT32(tradeEntrysize));		
	}
	
	template <typename T> 
	void MdpReceiver::TickRuleIsNull(bson_t *document, T *m)
	{
		std::int32_t size = m->tickRuleNullValue();
		std::int32_t tickrulesize = m->tickRule();
		if(tickrulesize != size)
			BCON_APPEND(document, "TickRule", BCON_INT32(tickrulesize));		
	}
	
	void MdpReceiver::ContractMultiplierUnitIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
	{
		std::int32_t size = m->contractMultiplierUnitNullValue();
		std::int32_t contractsize = m->contractMultiplierUnit();
		if(contractsize != size)
			BCON_APPEND(document, "ContractMultiplierUnit", BCON_INT32(contractsize));			
	}
	
    void MdpReceiver::FlowScheduleTypeIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
	{
		std::int32_t size = m->flowScheduleTypeNullValue();
		std::int32_t flowsize = m->flowScheduleType();
		if(flowsize != size)
			BCON_APPEND(document, "FlowScheduleType", BCON_INT32(flowsize));			
	}
	
    void MdpReceiver::MDPriceLevelIsNull(bson_t *document, mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries)
	{
		std::int32_t size = noMDEntries.mDPriceLevelNullValue();
		std::int32_t pricesize = noMDEntries.mDPriceLevel();
		if(pricesize != size)
			BCON_APPEND(document, "MDPriceLevel", BCON_INT32(pricesize));			
	}
	
    void MdpReceiver::SideIsNull(bson_t *document, mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym)
	{
		std::int32_t size = noRelatedSym.sideNullValue();
		std::int32_t sidesize = noRelatedSym.side();
		if(sidesize != size)
			BCON_APPEND(document, "Side", BCON_INT32(sidesize));			
	}
	
    template <typename T> 
	void MdpReceiver::MainFractionIsNull(bson_t *document, T *m)
	{
		std::int32_t size = m->mainFractionNullValue();
		std::int32_t mainsize = m->mainFraction();
		if(mainsize != size)
			BCON_APPEND(document, "MainFraction", BCON_INT32(mainsize));			
	}
	
	template <typename T> 
	void MdpReceiver::SubFractionIsNull(bson_t *document, T *m)
	{
		std::int32_t size = m->subFractionNullValue();
		std::int32_t subsize = m->subFraction();
		if(subsize != size)
			BCON_APPEND(document, "SubFraction", BCON_INT32(subsize));			
	}
	
	template <typename T> 
	void MdpReceiver::PriceDisplayFormatIsNull(bson_t *document, T *m)
	{
		std::int32_t size = m->priceDisplayFormatNullValue();
		std::int32_t pricesize = m->priceDisplayFormat();
		if(pricesize != size)
			BCON_APPEND(document, "PriceDisplayFormat", BCON_INT32(pricesize));			
	}	
	
	void MdpReceiver::UnderlyingProductIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m)
	{
		std::int32_t size = m->underlyingProductNullValue();
		std::int32_t undersize = m->underlyingProduct();
		if(undersize != size)
			BCON_APPEND(document, "UnderlyingProduct", BCON_INT32(undersize));			
	}
	
	void MdpReceiver::ReferenceIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries)
	{
		std::int32_t size = noOrderIDEntries.referenceIDNullValue();
		std::int32_t referencesize = noOrderIDEntries.referenceID();
		if(referencesize != size)
			BCON_APPEND(document, "ReferenceID", BCON_INT32(referencesize));		
	}	
	
    template <typename T> 
    void MdpReceiver::MDOrderPriorityIsNull(bson_t *document, T& noOrderIDEntries)	
	{
		std::int32_t size = noOrderIDEntries.mDOrderPriorityNullValue();
		std::int32_t ordersize = noOrderIDEntries.mDOrderPriority();
		if(ordersize != size)
			BCON_APPEND(document, "MDOrderPriority", BCON_INT32(ordersize));			
	}
			
	void MdpReceiver::OrderIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries)
    {
		std::int32_t size = noMDEntries.orderIDNullValue();
		std::int32_t ordersize = noMDEntries.orderID();
		if(ordersize != size)
			BCON_APPEND(document, "OrderID", BCON_INT32(ordersize));		
	}	
	
	template <typename T> 
	void MdpReceiver::UnitOfMeasureQtyIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->unitOfMeasureQty().mantissaNullValue();
		std::int64_t mantissa = m->unitOfMeasureQty().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "UnitOfMeasureQtyMantissa", BCON_INT64(mantissa),
		                          "UnitOfMeasureQtyExponent", BCON_INT32(m->unitOfMeasureQty().exponent()));			
	}

	template <typename T> 
	void MdpReceiver::TradingReferencePriceIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->tradingReferencePrice().mantissaNullValue();
		std::int64_t mantissa = m->tradingReferencePrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "TradingReferencePriceMantissa", BCON_INT64(mantissa),
		                          "TradingReferencePriceExponent", BCON_INT32(m->tradingReferencePrice().exponent()));				
	}
	
	template <typename T> 
	void MdpReceiver::HighLimitPriceIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->highLimitPrice().mantissaNullValue();
		std::int64_t mantissa = m->highLimitPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "HighLimitPriceMantissa", BCON_INT64(mantissa),
		                          "HighLimitPriceExponent", BCON_INT32(m->highLimitPrice().exponent()));				
	}
	
	void MdpReceiver::HighLimitPrice34IsNull(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries)
	{
		std::int64_t price = noMDEntries.highLimitPrice().mantissaNullValue();
		std::int64_t mantissa = noMDEntries.highLimitPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "HighLimitPriceMantissa", BCON_INT64(mantissa),
		                          "HighLimitPriceExponent", BCON_INT32(noMDEntries.highLimitPrice().exponent()));		
	}
	
	template <typename T> 
	void MdpReceiver::LowLimitPriceIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->lowLimitPrice().mantissaNullValue();
		std::int64_t mantissa = m->lowLimitPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "LowLimitPriceMantissa", BCON_INT64(mantissa),
		                          "LowLimitPriceExponent", BCON_INT32(m->lowLimitPrice().exponent()));			
	}
	
	void MdpReceiver::LowLimitPrice34IsNull(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries)
	{
		std::int64_t price = noMDEntries.lowLimitPrice().mantissaNullValue();
		std::int64_t mantissa = noMDEntries.lowLimitPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "LowLimitPriceMantissa", BCON_INT64(mantissa),
		                          "LowLimitPriceExponent", BCON_INT32(noMDEntries.lowLimitPrice().exponent()));			
	}
	
	template <typename T> 
	void MdpReceiver::MinPriceIncrementAmountIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->minPriceIncrementAmount().mantissaNullValue();
		std::int64_t mantissa = m->minPriceIncrementAmount().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MinPriceIncrementAmountMantissa", BCON_INT64(mantissa),
		                          "MinPriceIncrementAmountExponent", BCON_INT32(m->minPriceIncrementAmount().exponent()));		
	}
	
	void MdpReceiver::PriceRatioIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m)
	{
		std::int64_t price = m->priceRatio().mantissaNullValue();
		std::int64_t mantissa = m->priceRatio().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "PriceRatioMantissa", BCON_INT64(mantissa),
		                          "PriceRatioExponent", BCON_INT32(m->priceRatio().exponent()));					
	}
	
	void MdpReceiver::LegPriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29::NoLegs& lgs)
	{
		std::int64_t price = lgs.legPrice().mantissaNullValue();
		std::int64_t mantissa = lgs.legPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "LegPriceMantissa", BCON_INT64(mantissa),
		                          "LegPriceExponent", BCON_INT32(lgs.legPrice().exponent()));				
	}
	
	template <typename T> 
	void MdpReceiver::MDEntryPxIsNull(bson_t *document, T& noMDEntries)
	{
		std::int64_t price = noMDEntries.mDEntryPx().mantissaNullValue();
		std::int64_t mantissa = noMDEntries.mDEntryPx().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MDEntryPxMantissa", BCON_INT64(mantissa),
		                          "MDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()));				
	}
	
	template <typename T> 
	void MdpReceiver::MaxPriceVariationIsNull(bson_t *document, T *m)
	{
		std::int64_t price = m->maxPriceVariation().mantissaNullValue();
		std::int64_t mantissa = m->maxPriceVariation().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MaxPriceVariationMantissa", BCON_INT64(mantissa),
		                          "MaxPriceVariationExponent", BCON_INT32(m->maxPriceVariation().exponent()));				
	}
	
	void MdpReceiver::MaxPriceVariation34IsNull(bson_t *document,  mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries)
	{
		std::int64_t price = noMDEntries.maxPriceVariation().mantissaNullValue();
		std::int64_t mantissa = noMDEntries.maxPriceVariation().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MaxPriceVariationMantissa", BCON_INT64(mantissa),
		                          "MaxPriceVariationExponent", BCON_INT32(noMDEntries.maxPriceVariation().exponent()));				
	}
	
	void MdpReceiver::StrikePriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m)
	{
		std::int64_t price = m->strikePrice().mantissaNullValue();
		std::int64_t mantissa = m->strikePrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "StrikePriceMantissa", BCON_INT64(mantissa),
		                          "StrikePriceExponent", BCON_INT32(m->strikePrice().exponent()));				
	}
	
	void MdpReceiver::MinCabPriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m)
	{
		std::int64_t price = m->minCabPrice().mantissaNullValue();
		std::int64_t mantissa = m->minCabPrice().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MinCabPriceMantissa", BCON_INT64(mantissa),
		                          "MinCabPriceExponent", BCON_INT32(m->minCabPrice().exponent()));				
	}
	
	void MdpReceiver::MinPriceIncrementIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m)
    {
		std::int64_t price = m->minPriceIncrement().mantissaNullValue();
		std::int64_t mantissa = m->minPriceIncrement().mantissa();
		if(mantissa != price)
			BCON_APPEND(document, "MinPriceIncrementMantissa", BCON_INT64(mantissa),
		                          "MinPriceIncrementExponent", BCON_INT32(m->minPriceIncrement().exponent()));				
	}	
	
    void MdpReceiver::AppendData(bson_t *document, mktdata::ChannelReset4 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "Type", BCON_UTF8("ChannelReset4"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );
		
        bson_t *mdEntries = bson_new();
        mktdata::ChannelReset4::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType(0)),
                    "ApplID", BCON_INT32((int)noMDEntries.applID())
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
		char SeIDSource[10] = {0};
		m->getSecurityIDSource(SeIDSource, 10);
        BCON_APPEND (document,
                "Type", BCON_UTF8("MDInstrumentDefinitionFuture27"),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		
        BCON_APPEND(document, 
		    "SecurityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"LastUpdateTime", BCON_INT64(m->lastUpdateTime()),
			"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
			"ApplID", BCON_INT32(m->applID()),
			"MarketSegmentID", BCON_INT32(m->marketSegmentID()),
			"UnderlyingProduct", BCON_INT32(m->underlyingProduct()), 
			"SecurityExchange", BCON_UTF8(m->getSecurityExchangeAsString().c_str()),
			"SecurityGroup", BCON_UTF8(m->getSecurityGroupAsString().c_str()),
			"Asset", BCON_UTF8(m->getAssetAsString().c_str()),
			"Symbol", BCON_UTF8(m->getSymbolAsString().c_str()),
			"SecurityID", BCON_INT32(m->securityID()),
			"SecurityIDSource", BCON_UTF8(SeIDSource),
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
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),
			"MaxPriceVariationMantissa", BCON_INT64(m->maxPriceVariation().mantissa()),
			"MaxPriceVariationExponent", BCON_INT32(m->maxPriceVariation().exponent()),			
			"DecayStartDate", BCON_INT32(m->decayStartDate()),		
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"TradingReferenceDate", BCON_INT32(m->tradingReferenceDate())             
		);
		
		OpenInterestQtyIsNull(document, m);
		ClearedVolumeIsNull(document, m);	
		DecayQuantityIsNull(document, m);
		OriginalContractSizeIsNull(document, m);
        ContractMultiplierIsNull(document,m);					
		TotNumReportsIsNull(document, m);		
		ContractMultiplierUnitIsNull(document, m);
		FlowScheduleTypeIsNull(document, m);		
		MainFractionIsNull(document, m);
	    SubFractionIsNull(document, m);	
	    PriceDisplayFormatIsNull(document, m);		
		UnitOfMeasureQtyIsNull(document, m);
		TradingReferencePriceIsNull(document, m);
		HighLimitPriceIsNull(document, m);
		LowLimitPriceIsNull(document, m);
		MinPriceIncrementAmountIsNull(document, m);
		
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
				"MDFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"MarketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
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
        BCON_APPEND (document,
                "Type", BCON_UTF8("MDInstrumentDefinitionSpread29"),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		
		
        BCON_APPEND(document, 
		    "SecurityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"LastUpdateTime", BCON_INT64(m->lastUpdateTime()),
			"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus()),
			"ApplID", BCON_INT32(m->applID()),
			"MarketSegmentID", BCON_INT32(m->marketSegmentID()), 
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
			"MatchAlgorithm", BCON_INT32((int)m->matchAlgorithm()),
			"MinTradeVol", BCON_INT32(m->minTradeVol()),
			"MaxTradeVol", BCON_INT32(m->maxTradeVol()),
			"MinPriceIncrementMantissa", BCON_INT64(m->minPriceIncrement().mantissa()),
			"MinPriceIncrementExponent", BCON_INT32(m->minPriceIncrement().exponent()),
			"DisplayFactorMantissa", BCON_INT64(m->displayFactor().mantissa()),
			"DisplayFactorExponent", BCON_INT32(m->displayFactor().exponent()),
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),				
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"TradingReferenceDate", BCON_INT32(m->tradingReferenceDate())             
		);

		OpenInterestQtyIsNull(document, m);
        ClearedVolumeIsNull(document, m);		
		TotNumReportsIsNull(document, m);		
		TickRuleIsNull(document, m);		
		MainFractionIsNull(document, m);
	    SubFractionIsNull(document, m);	
	    PriceDisplayFormatIsNull(document, m);
        UnderlyingProductIsNull(document, m);				
		PriceRatioIsNull(document, m);
		TradingReferencePriceIsNull(document, m);
		HighLimitPriceIsNull(document, m);
		LowLimitPriceIsNull(document, m);
		MaxPriceVariationIsNull(document, m);		
		
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
				"MDFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"MarketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
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
			lgs.next();
			bson_t *temp_child = BCON_NEW (
				"LegSecurityID", BCON_INT32(lgs.legSecurityID()), "LegSecurityIDSource",  BCON_UTF8(lgs.legSecurityIDSource()), "LegSide", BCON_INT32(lgs.legSide()),
				"LegRatioQty", BCON_INT32(lgs.legRatioQty()),
				"LegOptionDeltaMantissa",  BCON_INT32(lgs.legOptionDelta().mantissa()),  
				"LegOptionDeltaExponent",  BCON_INT32(lgs.legOptionDelta().exponent())
            );
			LegPriceIsNull(temp_child, lgs);
            BSON_APPEND_DOCUMENT(Legs, std::to_string(index_NoLegs).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_NoLegs++;
		}
		BSON_APPEND_ARRAY(document, "NoLegs", Legs);
		destroyVector.push_back(Legs);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SecurityStatus30 *m, std::vector<bson_t *> &destroyVector)
    {		
        BCON_APPEND (document,
                "Type", BCON_UTF8("SecurityStatus30"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "SecurityGroup", BCON_UTF8 (m->securityGroup()),
                "Asset", BCON_UTF8 (m->asset()),
                "TradeDate", BCON_INT32(m->tradeDate()),
                "MatchEventIndicator", BCON_UTF8(m->matchEventIndicator().buffer()),
                "SecurityTradingStatus", BCON_INT32(m->securityTradingStatus()),
                "HaltReason", BCON_INT32(m->haltReason()),
                "SecurityTradingEvent", BCON_INT32(m->securityTradingEvent())
        );
		
		SecurityIDIsNull(document, m);			
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshBook32 *m, std::vector<bson_t *> &destroyVector)
    {
		BCON_APPEND (document,
			"Type", BCON_UTF8("MDIncrementalRefreshBook32"),
			"TransactTime", BCON_INT64 (m->transactTime()),
			"MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
			"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
			"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
			"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
			"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
			"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
			"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
			"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                    "MDPriceLevel", BCON_INT32((int)noMDEntries.mDPriceLevel()),
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
			MDEntrySizeIsNull(temp_child, noMDEntries);
			NumberOfOrdersIsNull(temp_child, noMDEntries);
			MDEntryPxIsNull(temp_child, noMDEntries);
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
                    "OrderUpdateAction", BCON_INT32 ((int)noOrderIDEntries.orderUpdateAction())
            );
			MDDisplayQtyIsNull(temp_child, noOrderIDEntries);
			ReferenceIDIsNull(temp_child, noOrderIDEntries);
			MDOrderPriorityIsNull(temp_child, noOrderIDEntries);
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
                "Type", BCON_UTF8("MDIncrementalRefreshDailyStatistics33"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		   
		
		   
        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"TradingReferenceDate", BCON_INT32 ((int)noMDEntries.tradingReferenceDate()),
					"SettlPriceTypeFinal", BCON_INT32((int)noMDEntries.settlPriceType().finalrc()),
					"SettlPriceTypeActual", BCON_INT32(noMDEntries.settlPriceType().actual()),
					"SettlPriceTypeRounded", BCON_INT32(noMDEntries.settlPriceType().rounded()),
					"SettlPriceTypeIntraday", BCON_INT32(noMDEntries.settlPriceType().intraday()),
					"SettlPriceTypeReservedBits", BCON_INT32(noMDEntries.settlPriceType().reservedBits()),
					"SettlPriceTypeNullValue", BCON_INT32(noMDEntries.settlPriceType().nullValue()),
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType()),
					"MDEntrySize", BCON_INT32(noMDEntries.mDEntrySize())
            );
			MDEntrySizeIsNull(temp_child, noMDEntries);
			MDEntryPxIsNull(temp_child, noMDEntries);
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
		"Type", BCON_UTF8("MDIncrementalRefreshLimitsBanding34"),
		"TransactTime", BCON_INT64 (m->transactTime()),
		"MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
		"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
		"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
		"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
		"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
		"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
		"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
		"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (	
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType(0))    
            );
			HighLimitPrice34IsNull(temp_child, noMDEntries);
		    LowLimitPrice34IsNull(temp_child, noMDEntries);
			MaxPriceVariation34IsNull(temp_child, noMDEntries);	
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshSessionStatistics35 *m, std::vector<bson_t *> &destroyVector)
    {
		boost::property_tree::ptree pt_document;
		pt_document.put("Type", "MDIncrementalRefreshSessionStatistics35");
		pt_document.put("TransactTime", m->transactTime());
		pt_document.put("MatchEventIndicatorLastTradeMsg", m->matchEventIndicator().lastTradeMsg());
		pt_document.put("MatchEventIndicatorLastVolumeMsg", m->matchEventIndicator().lastVolumeMsg());
		pt_document.put("MatchEventIndicatorLastQuoteMsg", m->matchEventIndicator().lastQuoteMsg());
		pt_document.put("MatchEventIndicatorLastStatsMsg", m->matchEventIndicator().lastStatsMsg());
		pt_document.put("MatchEventIndicatorLastImpliedMsg", m->matchEventIndicator().lastImpliedMsg());
		pt_document.put("MatchEventIndicatorRecoveryMsg", m->matchEventIndicator().recoveryMsg());
		pt_document.put("MatchEventIndicatorReserved", m->matchEventIndicator().reserved());
		pt_document.put("MatchEventIndicatorEndOfEvent", m->matchEventIndicator().endOfEvent());
		
        BCON_APPEND (document,
                "Type", BCON_UTF8("MDIncrementalRefreshSessionStatistics35"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		
        
		boost::property_tree::ptree pt_mdEntries;
		
        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
			boost::property_tree::ptree pt_child;
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "MDEntryPxMantissa", BCON_INT64(noMDEntries.mDEntryPx().mantissa()),
					"MDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
					"RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                    "OpenCloseSettlFlag", BCON_INT32((int)noMDEntries.openCloseSettlFlag()),
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
			
			pt_child.put("MDEntryPxMantissa", noMDEntries.mDEntryPx().mantissa());
			pt_child.put("MDEntryPxExponent", noMDEntries.mDEntryPx().exponent());
			pt_child.put("SecurityID", noMDEntries.securityID());
			pt_child.put("RptSeq", noMDEntries.rptSeq());
			pt_child.put("OpenCloseSettlFlag", noMDEntries.openCloseSettlFlag());
			pt_child.put("MDUpdateAction", noMDEntries.mDUpdateAction());
			pt_child.put("MDEntryType", noMDEntries.mDEntryType());
			pt_mdEntries.push_back(make_pair("",pt_child));
			
			MDEntrySizeIsNull(temp_child, noMDEntries);
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md ++;
        }
		
		pt_document.put_child("noMDEntries", pt_mdEntries);  
		std::ostringstream buf; 
		boost::property_tree::write_json (buf, pt_document, false);
		std::string json = buf.str();
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
        LOG_INFO(json);
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshTrade36 *m, std::vector<bson_t *> &destroyVector)
    {
        BCON_APPEND (document,
                "Type", BCON_UTF8("MDIncrementalRefreshTrade36"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshTrade36::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "MDEntryPx", BCON_INT64(noMDEntries.mDEntryPx().mantissa()),
                    "MDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"TradeID", BCON_INT32 ((int)noMDEntries.tradeIDId()),
					"AggressorSide", BCON_INT32((int)noMDEntries.aggressorSide()),
					"MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType(0))        
            );
			NumberOfOrdersIsNull(temp_child, noMDEntries);
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
                "Type", BCON_UTF8("MDIncrementalRefreshVolume37"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
			    "MDEntrySize", BCON_INT32((int)noMDEntries.mDEntrySize()),
				"SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
				"RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
                "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                "MDEntryType", BCON_INT32(noMDEntries.mDEntryType(0))
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
                "Type", BCON_UTF8("SnapshotFullRefresh38"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "LastMsgSeqNumProcessed", BCON_INT32 (m->lastMsgSeqNumProcessed()),
				"TotNumReports", BCON_INT32 (m->totNumReports()),
				"SecurityID", BCON_INT32 (m->securityID()),
				"RptSeq", BCON_INT32 (m->rptSeq()),
				"LastUpdateTime", BCON_INT64(m->lastUpdateTime()),
				"TradeDate", BCON_INT32(m->tradeDate()),
				"MDSecurityTradingStatus", BCON_INT32(m->mDSecurityTradingStatus())
        );
		
		HighLimitPriceIsNull(document, m);
		LowLimitPriceIsNull(document, m);
        MaxPriceVariationIsNull(document, m);
		
        bson_t *mdEntries = bson_new();
        mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
					"TradingReferenceDate", BCON_INT32(noMDEntries.tradingReferenceDate()),
					"OpenCloseSettlFlag", BCON_INT32((int)noMDEntries.openCloseSettlFlag()),
					"SettlPriceTypeFinal", BCON_INT32((int)noMDEntries.settlPriceType().finalrc()),
					"SettlPriceTypeActual", BCON_INT32(noMDEntries.settlPriceType().actual()),
					"SettlPriceTypeRounded", BCON_INT32(noMDEntries.settlPriceType().rounded()),
					"SettlPriceTypeIntraday", BCON_INT32(noMDEntries.settlPriceType().intraday()),
					"SettlPriceTypeReservedBits", BCON_INT32(noMDEntries.settlPriceType().reservedBits()),
					"SettlPriceTypeNullValue", BCON_INT32(noMDEntries.settlPriceType().nullValue()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
			MDEntrySizeIsNull(temp_child, noMDEntries);
			NumberOfOrdersIsNull(temp_child, noMDEntries);
			MDPriceLevelIsNull(temp_child, noMDEntries);
			MDEntryPxIsNull(temp_child, noMDEntries);
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);
		destroyVector.push_back(mdEntries);
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::QuoteRequest39 *m, std::vector<bson_t *> &destroyVector)
    {
		boost::property_tree::ptree pt_document;
		pt_document.put("Type", "MDIncrementalRefreshSessionStatistics35");
		pt_document.put("QuoteReqID", m->quoteReqID());
		pt_document.put("TransactTime", m->transactTime());
		pt_document.put("MatchEventIndicatorLastTradeMsg", m->matchEventIndicator().lastTradeMsg());
		pt_document.put("MatchEventIndicatorLastVolumeMsg", m->matchEventIndicator().lastVolumeMsg());
		pt_document.put("MatchEventIndicatorLastQuoteMsg", m->matchEventIndicator().lastQuoteMsg());
		pt_document.put("MatchEventIndicatorLastStatsMsg", m->matchEventIndicator().lastStatsMsg());
		pt_document.put("MatchEventIndicatorLastImpliedMsg", m->matchEventIndicator().lastImpliedMsg());
		pt_document.put("MatchEventIndicatorRecoveryMsg", m->matchEventIndicator().recoveryMsg());
		pt_document.put("MatchEventIndicatorReserved", m->matchEventIndicator().reserved());
		pt_document.put("MatchEventIndicatorEndOfEvent", m->matchEventIndicator().endOfEvent());
		
        BCON_APPEND (document,
                "Type", BCON_UTF8("QuoteRequest39"),
				"QuoteReqID", BCON_UTF8 (m->quoteReqID()),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		
		
		boost::property_tree::ptree pt_relatedSym;
		
        bson_t *relatedSym = bson_new();
        mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym = m->noRelatedSym();
        std::uint64_t index_md = 0;
        while (noRelatedSym.hasNext())
        {
			boost::property_tree::ptree pt_child;
			
            noRelatedSym.next();
            bson_t *temp_child = BCON_NEW (
			        "Symbol", BCON_UTF8(noRelatedSym.getSymbolAsString().c_str()),
					"SecurityID", BCON_INT32 ((int)noRelatedSym.securityID()),
                    "QuoteType", BCON_INT32(noRelatedSym.quoteType())
            );
			
			pt_child.put("Symbol", noRelatedSym.getSymbolAsString().c_str());
			pt_child.put("SecurityID", noRelatedSym.securityID());
			pt_child.put("QuoteType", noRelatedSym.quoteType());
			pt_relatedSym.push_back(make_pair("",pt_child));			
			
			OrderQtyIsNull(temp_child, noRelatedSym);
			SideIsNull(temp_child, noRelatedSym);
            BSON_APPEND_DOCUMENT(relatedSym, std::to_string(index_md).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_md++;			
			
        }
        BSON_APPEND_ARRAY(document, "NoRelatedSym", relatedSym);
		destroyVector.push_back(relatedSym);
		
		pt_document.put_child("NoRelatedSym", pt_relatedSym);  
		std::ostringstream buf; 
		boost::property_tree::write_json (buf, pt_document, false);
		std::string json = buf.str();
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
        LOG_INFO(json);
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		LOG_INFO("#####################################################################################");
		
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m, std::vector<bson_t *> &destroyVector)
    {
		BCON_APPEND (document,
			"Type", BCON_UTF8("MDInstrumentDefinitionOption41"),
			"MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
			"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
			"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
			"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
			"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
			"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
			"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
			"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );
		
        BCON_APPEND(document,  
		    "SecurityUpdateAction", BCON_INT32((int)m->securityUpdateAction()),
			"LastUpdateTime", BCON_INT64(m->lastUpdateTime()),
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
			"StrikeCurrency", BCON_UTF8(m->getStrikeCurrencyAsString().c_str()),
			"SettlCurrency", BCON_UTF8(m->getSettlCurrencyAsString().c_str()),
			"MatchAlgorithm", BCON_INT32((int)m->matchAlgorithm()),
			"MinTradeVol", BCON_INT32(m->minTradeVol()),
			"MaxTradeVol", BCON_INT32(m->maxTradeVol()),			
			"DisplayFactorMantissa", BCON_INT64(m->displayFactor().mantissa()),
			"DisplayFactorExponent", BCON_INT32(m->displayFactor().exponent()),
			"UnitOfMeasure", BCON_UTF8(m->getUnitOfMeasureAsString().c_str()),
			"SettlPriceTypeFinal", BCON_INT32((int)m->settlPriceType().finalrc()),
			"SettlPriceTypeActual", BCON_INT32(m->settlPriceType().actual()),
			"SettlPriceTypeRounded", BCON_INT32(m->settlPriceType().rounded()),
			"SettlPriceTypeIntraday", BCON_INT32(m->settlPriceType().intraday()),
			"SettlPriceTypeReservedBits", BCON_INT32(m->settlPriceType().reservedBits()),
			"SettlPriceTypeNullValue", BCON_INT32(m->settlPriceType().nullValue()),
			"UserDefinedInstrument", BCON_INT32(m->userDefinedInstrument()),
			"TradingReferenceDate", BCON_INT32(m->tradingReferenceDate())            
		);
		
		OpenInterestQtyIsNull(document, m);
		ClearedVolumeIsNull(document, m);		
		TotNumReportsIsNull(document, m);		
		TickRuleIsNull(document, m);		
		MainFractionIsNull(document, m);
	    SubFractionIsNull(document, m);	
	    PriceDisplayFormatIsNull(document, m);		
		StrikePriceIsNull(document, m);	
		MinCabPriceIsNull(document, m);	
		MinPriceIncrementIsNull(document, m);			
		UnitOfMeasureQtyIsNull(document, m);
		TradingReferencePriceIsNull(document, m);
		HighLimitPriceIsNull(document, m);
		LowLimitPriceIsNull(document, m);
		MinPriceIncrementAmountIsNull(document, m);
		
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
				"MDFeedType", BCON_UTF8(noMDFeedTypes.getMDFeedTypeAsString().c_str()),
				"MarketDepth", BCON_INT32((int)noMDFeedTypes.marketDepth())
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
		
		char unSymbol[10] = {0};
		char unSeIDSource[10] = {0};
		bson_t *Underlyings = bson_new();
        mktdata::MDInstrumentDefinitionOption41::NoUnderlyings& under = m->noUnderlyings();
        std::uint64_t index_NoUnderlyings = 0;
		while (under.hasNext())
		{
			under.next();
			memset(unSymbol, 0, sizeof(unSymbol));
			memset(unSeIDSource, 0, sizeof(unSeIDSource));
			under.getUnderlyingSymbol(unSymbol, 10);
			under.getUnderlyingSecurityIDSource(unSeIDSource, 10);
			bson_t *temp_child = BCON_NEW (
				"UnderlyingSecurityID", BCON_INT32(under.underlyingSecurityID()), "UnderlyingSecurityIDSource",  BCON_UTF8(unSeIDSource), "UnderlyingSymbol", BCON_UTF8(under.underlyingSymbol())
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
                "Type", BCON_UTF8("MDIncrementalRefreshTradeSummary42"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );		
		
        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "MDEntryPxMantissa", BCON_INT64(noMDEntries.mDEntryPx().mantissa()),
					"MDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
                    "MDEntrySize", BCON_INT32 ((int)noMDEntries.mDEntrySize()),
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "RptSeq", BCON_INT32 ((int)noMDEntries.rptSeq()),
					"AggressorSide", BCON_INT32((int)noMDEntries.aggressorSide()),
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType(0))		
            );
			NumberOfOrdersIsNull(temp_child, noMDEntries);
			MDTradeEntryIDIsNull(temp_child, noMDEntries);
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
                    "OrderID", BCON_INT64(noOrderIDEntries.orderID()),
                    "LastQty", BCON_INT32(noOrderIDEntries.lastQty())
            );
            BSON_APPEND_DOCUMENT(orderIdEntries, std::to_string(index_order).c_str(), temp_child);
		    destroyVector.push_back(temp_child);
            index_order++;
        }
        BSON_APPEND_ARRAY(document, "noOrderIDEntries", orderIdEntries);
		destroyVector.push_back(orderIdEntries);	
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43 *m, std::vector<bson_t *> &destroyVector)
    {	
        BCON_APPEND (document,
                "Type", BCON_UTF8("MDIncrementalRefreshOrderBook43"),
                "TransactTime", BCON_INT64 (m->transactTime()),
                "MatchEventIndicatorLastTradeMsg", BCON_INT32(m->matchEventIndicator().lastTradeMsg()),
				"MatchEventIndicatorLastVolumeMsg", BCON_INT32(m->matchEventIndicator().lastVolumeMsg()),
				"MatchEventIndicatorLastQuoteMsg", BCON_INT32(m->matchEventIndicator().lastQuoteMsg()),
				"MatchEventIndicatorLastStatsMsg", BCON_INT32(m->matchEventIndicator().lastStatsMsg()),
				"MatchEventIndicatorLastImpliedMsg", BCON_INT32(m->matchEventIndicator().lastImpliedMsg()),
				"MatchEventIndicatorRecoveryMsg", BCON_INT32(m->matchEventIndicator().recoveryMsg()),
				"MatchEventIndicatorReserved", BCON_INT32(m->matchEventIndicator().reserved()),
				"MatchEventIndicatorEndOfEvent", BCON_INT32(m->matchEventIndicator().endOfEvent())
        );			

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries = m->noMDEntries();
        std::uint64_t index_md = 0;
        while (noMDEntries.hasNext())
        {
            noMDEntries.next();
            bson_t *temp_child = BCON_NEW (
                    "SecurityID", BCON_INT32 ((int)noMDEntries.securityID()),
                    "MDUpdateAction", BCON_INT32((int)noMDEntries.mDUpdateAction()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
			MDDisplayQtyIsNull(temp_child, noMDEntries);
			MDOrderPriorityIsNull(temp_child, noMDEntries);
			OrderIDIsNull(temp_child, noMDEntries);
			MDEntryPxIsNull(temp_child, noMDEntries);
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
                "Type", BCON_UTF8("SnapshotFullRefreshOrderBook44"),
                "TransactTime", BCON_INT64 (m->transactTime()),
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
			        "OrderID", BCON_INT64(noMDEntries.orderID()),
                    "MDEntryPxMantissa", BCON_INT64(noMDEntries.mDEntryPx().mantissa()),
					"MDEntryPxExponent", BCON_INT32(noMDEntries.mDEntryPx().exponent()),
					"MDDisplayQty", BCON_INT32(noMDEntries.mDDisplayQty()),
                    "MDEntryType", BCON_INT32(noMDEntries.mDEntryType())
            );
			MDOrderPriorityIsNull(temp_child, noMDEntries);
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
