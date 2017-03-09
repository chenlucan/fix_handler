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

    void MdpReceiver::Save(char *data, size_t size)
    {
        //        8 bytes : m_received_time
        //        2 bytes : m_packet_length
        //        4 bytes : m_packet_seq_num
        //        8 bytes : m_packet_sending_time
        //        2 bytes : m_message_length
        //        (m_message_length) bytes : m_buffer

        std::uint64_t received_time = *(std::uint64_t *)data;
        //std::uint16_t packet_length = *(std::uint16_t *)(data + 8);     // 这个域不保存
        std::uint32_t packet_seq_num = *(std::uint32_t *)(data + 8 + 2);
        std::uint64_t packet_sending_time = *(std::uint64_t *)(data + 8 + 2 + 4);
        //std::uint16_t message_length = *(std::uint16_t *)(data + 8 + 2 + 4 + 8);     // 这个域不保存

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
                this->AppendData(document, static_cast<mktdata::ChannelReset4*>(mdp_message));
                break;
            case 27:     // MDInstrumentDefinitionFuture27
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionFuture27*>(mdp_message));
                break;
            case 29:     // MDInstrumentDefinitionSpread29
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionSpread29*>(mdp_message));
                break;
            case 30:     // SecurityStatus30
                this->AppendData(document, static_cast<mktdata::SecurityStatus30*>(mdp_message));
                break;
            case 32:     // MDIncrementalRefreshBook32
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshBook32*>(mdp_message));
                break;
            case 33:     // MDIncrementalRefreshDailyStatistics33
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshDailyStatistics33*>(mdp_message));
                break;
            case 34:     // MDIncrementalRefreshLimitsBanding34
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshLimitsBanding34*>(mdp_message));
                break;
            case 35:     // MDIncrementalRefreshSessionStatistics35
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshSessionStatistics35*>(mdp_message));
                break;
            case 36:     // MDIncrementalRefreshTrade36
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshTrade36*>(mdp_message));
                break;
            case 37:     // MDIncrementalRefreshVolume37
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshVolume37*>(mdp_message));
                break;
            case 38:     // SnapshotFullRefresh38
                this->AppendData(document, static_cast<mktdata::SnapshotFullRefresh38*>(mdp_message));
                break;
            case 39:     // QuoteRequest39
                this->AppendData(document, static_cast<mktdata::QuoteRequest39*>(mdp_message));
                break;
            case 41:     // MDInstrumentDefinitionOption41
                this->AppendData(document, static_cast<mktdata::MDInstrumentDefinitionOption41*>(mdp_message));
                break;
            case 42:     // MDIncrementalRefreshTradeSummary42
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshTradeSummary42*>(mdp_message));
                break;
            case 43:     // MDIncrementalRefreshOrderBook43
                this->AppendData(document, static_cast<mktdata::MDIncrementalRefreshOrderBook43*>(mdp_message));
                break;
            case 44:     // SnapshotFullRefreshOrderBook44
                this->AppendData(document, static_cast<mktdata::SnapshotFullRefreshOrderBook44*>(mdp_message));
                break;
            default:
                LOG_WARN("invalid message template id: ", mdp_header->templateId());
                break;
        }

        bson_error_t error;
        if (mongoc_collection_insert (m_collection, MONGOC_INSERT_NONE, document, nullptr, &error))
        {
            char *str = bson_as_json (document, nullptr);
            LOG_TRACE("message inserted: ", str);
            bson_free (str);
        }
        else
        {
            LOG_ERROR("db insert error: ", error.message);
        }

        bson_destroy (document);
    }


    void MdpReceiver::AppendData(bson_t *document, mktdata::ChannelReset4 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionFuture27 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionSpread29 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SecurityStatus30 *m)
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

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshBook32 *m)
    {
        BCON_APPEND (document,
                "type", BCON_UTF8("MDIncrementalRefreshBook32"),
                "transactTime", BCON_INT64 (m->transactTime()),
                "matchEventIndicator", BCON_UTF8 (m->matchEventIndicator().buffer())
        );

        bson_t *mdEntries = bson_new();
        mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = m->noMDEntries();
        std::vector<bson_t *> children_of_md;
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
            BSON_APPEND_DOCUMENT(mdEntries, std::to_string(index_md).data(), temp_child);
            children_of_md.push_back(temp_child);
            index_md ++;
        }
        BSON_APPEND_ARRAY(document, "noMDEntries", mdEntries);

        bson_t *orderIdEntries = bson_new();
        mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries = m->noOrderIDEntries();
        std::vector<bson_t *> children_of_order;
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
            BSON_APPEND_DOCUMENT(orderIdEntries, std::to_string(index_order).data(), temp_child);
            children_of_order.push_back(temp_child);
            index_order ++;
        }
        BSON_APPEND_ARRAY(document, "noOrderIDEntries", orderIdEntries);

        // TODO 用到的 bson_t * 要传递出去，以便在数据插入到库后能够销毁
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshDailyStatistics33 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshLimitsBanding34 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshSessionStatistics35 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshTrade36 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshVolume37 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SnapshotFullRefresh38 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::QuoteRequest39 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDInstrumentDefinitionOption41 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshTradeSummary42 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::MDIncrementalRefreshOrderBook43 *m)
    {
        // TODO
    }

    void MdpReceiver::AppendData(bson_t *document, mktdata::SnapshotFullRefreshOrderBook44 *m)
    {
        // TODO
    }

} // namespace persist
} // namespace market
} // namespace cme
} // namespace fh
