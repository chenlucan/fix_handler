
#ifndef __FH_CME_MARKET_PERSIST_MDP_RECEIVER_H__
#define __FH_CME_MARKET_PERSIST_MDP_RECEIVER_H__

#include <libbson-1.0/bcon.h>
#include <libbson-1.0/bson.h>
#include <libmongoc-1.0/mongoc.h>
#include <iostream>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "core/global.h"
#include "core/zmq/zmq_receiver.h"
#include "cme/market/message/mktdata.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace persist
{
    class MdpReceiver : public fh::core::zmq::ZmqReceiver
    {
        public:
            MdpReceiver(
                    const std::string &receive_url,
                    const std::string &db_url,
                    const std::string &db_name,
                    const std::string &collection_name);
            virtual ~MdpReceiver();

        public:
            virtual void Save(char *data, size_t size);

        private:
            void AppendData(bson_t *document, mktdata::ChannelReset4 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::SecurityStatus30 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshBook32 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshDailyStatistics33 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshSessionStatistics35 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshTrade36 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshVolume37 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::SnapshotFullRefresh38 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::QuoteRequest39 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43 *m, std::vector<bson_t *> &destroyVector);
            void AppendData(bson_t *document, mktdata::SnapshotFullRefreshOrderBook44 *m, std::vector<bson_t *> &destroyVector);
            void DestroyBsonVector(const std::vector<bson_t *> &valList);
			
			// Int32NULL
			template <typename T> 
			void MDEntrySizeIsNull(bson_t *document, T& noMDEntries);
			
			template <typename T> 
			void OpenInterestQtyIsNull(bson_t *document, T *m);
			
			template <typename T> 
			void ClearedVolumeIsNull(bson_t *document, T *m);
			
			void DecayQuantityIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
			void OriginalContractSizeIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
			void ContractMultiplierIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
			void SecurityIDIsNull(bson_t *document, mktdata::SecurityStatus30 *m);
			
			template <typename T> 
			void NumberOfOrdersIsNull(bson_t *document, T& noMDEntries);
			
			template <typename T> 
			void MDDisplayQtyIsNull(bson_t *document, T& noMDEntries);
			
			void OrderQtyIsNull(bson_t *document, mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym);
			
			// uInt32NULL
			template <typename T> 
			void TotNumReportsIsNull(bson_t *document, T *m);
			
			void MDTradeEntryIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries);
			
			// Int8NULL
			template <typename T> 
			void TickRuleIsNull(bson_t *document, T *m);
			
			void ContractMultiplierUnitIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
			void FlowScheduleTypeIsNull(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
			void MDPriceLevelIsNull(bson_t *document, mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries);
			void SideIsNull(bson_t *document, mktdata::QuoteRequest39::NoRelatedSym& noRelatedSym);
			
			// uInt8NULL
			template <typename T> 
			void MainFractionIsNull(bson_t *document, T *m);
			
			template <typename T> 
			void SubFractionIsNull(bson_t *document, T *m);
			
			template <typename T> 
			void PriceDisplayFormatIsNull(bson_t *document, T *m);
			
			void UnderlyingProductIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m);
			
			void ReferenceIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries);
			
			// uInt64NULL
			template <typename T> 
			void MDOrderPriorityIsNull(bson_t *document, T& noOrderIDEntries);
			
			void OrderIDIsNull(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries);
			
			// priceNULL
			template <typename T> 
			void UnitOfMeasureQtyIsNull(bson_t *document, T *m);
			
			template <typename T> 
			void TradingReferencePriceIsNull(bson_t *document, T *m);
			
			template <typename T> 
			void HighLimitPriceIsNull(bson_t *document, T *m);
			void HighLimitPrice34IsNull(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries);
			
			template <typename T> 
			void LowLimitPriceIsNull(bson_t *document, T *m);
			void LowLimitPrice34IsNull(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries);
			
			template <typename T> 
			void MinPriceIncrementAmountIsNull(bson_t *document, T *m);
			
			void PriceRatioIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m);
			void LegPriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionSpread29::NoLegs& lgs);
			
			template <typename T> 
			void MDEntryPxIsNull(bson_t *document, T& noMDEntries);
			
			template <typename T> 
			void MaxPriceVariationIsNull(bson_t *document, T *m);
			void MaxPriceVariation34IsNull(bson_t *document,  mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries);
			
			void StrikePriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m);
			void MinCabPriceIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m);
			void MinPriceIncrementIsNull(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m);			
			
        private:
            std::string m_db_url;
            std::string m_db_name;
            std::string m_collection_name;
            mongoc_client_t *m_client;
		    mongoc_collection_t *m_collection;

        private:
			DISALLOW_COPY_AND_ASSIGN(MdpReceiver);
    };
} // namespace persist
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_PERSIST_MDP_RECEIVER_H__
