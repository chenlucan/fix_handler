
#ifndef __FH_CME_MARKET_PERSIST_MDP_RECEIVER_H__
#define __FH_CME_MARKET_PERSIST_MDP_RECEIVER_H__

#include <libbson-1.0/bcon.h>
#include <libbson-1.0/bson.h>
#include <libmongoc-1.0/mongoc.h>
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
            void AppendData(bson_t *document, mktdata::ChannelReset4 *m);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m);
            void AppendData(bson_t *document, mktdata::SecurityStatus30 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshBook32 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshDailyStatistics33 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshSessionStatistics35 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshTrade36 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshVolume37 *m);
            void AppendData(bson_t *document, mktdata::SnapshotFullRefresh38 *m);
            void AppendData(bson_t *document, mktdata::QuoteRequest39 *m);
            void AppendData(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42 *m);
            void AppendData(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43 *m);
            void AppendData(bson_t *document, mktdata::SnapshotFullRefreshOrderBook44 *m);

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
