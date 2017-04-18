
#include <boost/lexical_cast.hpp>
#include "cme/market/message/mktdata.h"
#include "cme/market/message/sbe_encoder.h"
#include "cme/market/message/sbe_decoder.h"
#include "cme/market/message/mdp_message.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"

void printSecurityStatus30(mktdata::SecurityStatus30 &message)
{
    LOG_INFO("message.type=SecurityStatus30");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.transactTime=", message.transactTime());
    LOG_INFO("message.securityGroup=", message.getSecurityGroupAsString());
    LOG_INFO("message.asset=", message.getAssetAsString());
    LOG_INFO("message.securityID=", message.securityID());
    LOG_INFO("message.tradeDate=", message.tradeDate());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    LOG_INFO("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    LOG_INFO("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    LOG_INFO("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    LOG_INFO("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    LOG_INFO("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    LOG_INFO("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    LOG_INFO("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    LOG_INFO("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());

    LOG_INFO("message.securityTradingStatus=", message.securityTradingStatus());
    LOG_INFO("message.haltReason=", message.haltReason());
    LOG_INFO("message.securityTradingEvent=", message.securityTradingEvent());
    LOG_INFO("");
}

void printMDIncrementalRefreshVolume32(mktdata::MDIncrementalRefreshBook32 &message)
{
    LOG_INFO("message.type=MDIncrementalRefreshBook32");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.transactTime=", message.transactTime());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    LOG_INFO("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    LOG_INFO("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    LOG_INFO("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    LOG_INFO("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    LOG_INFO("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    LOG_INFO("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    LOG_INFO("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    LOG_INFO("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());

    mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = message.noMDEntries();
    while (noMDEntries.hasNext())
    {
        noMDEntries.next();
        LOG_INFO("message.noMDEntries.mDEntryPx=", noMDEntries.mDEntryPx().mantissa());
        LOG_INFO("message.noMDEntries.mDEntrySize=", (int)noMDEntries.mDEntrySize());
        LOG_INFO("message.noMDEntries.securityID=", (int)noMDEntries.securityID());
        LOG_INFO("message.noMDEntries.rptSeq=", (int)noMDEntries.rptSeq());
        LOG_INFO("message.noMDEntries.numberOfOrders=", (int)noMDEntries.numberOfOrders());
        LOG_INFO("message.noMDEntries.mDPriceLevel=", (int)noMDEntries.mDPriceLevel());
        LOG_INFO("message.noMDEntries.mDUpdateAction=", (int)noMDEntries.mDUpdateAction());
        LOG_INFO("message.noMDEntries.mDEntryType=", noMDEntries.mDEntryType());
    }

    mktdata::MDIncrementalRefreshBook32::NoOrderIDEntries& noOrderIDEntries = message.noOrderIDEntries();
    while (noOrderIDEntries.hasNext())
    {
        noOrderIDEntries.next();
        LOG_INFO("message.noOrderIDEntries.orderID=", noOrderIDEntries.orderID());
        LOG_INFO("message.noOrderIDEntries.mDOrderPriority=", (int)noOrderIDEntries.mDOrderPriority());
        LOG_INFO("message.noOrderIDEntries.mDDisplayQty=", (int)noOrderIDEntries.mDDisplayQty());
        LOG_INFO("message.noOrderIDEntries.referenceID=", (int)noOrderIDEntries.referenceID());
        LOG_INFO("message.noOrderIDEntries.orderUpdateAction=", (int)noOrderIDEntries.orderUpdateAction());
    }
    LOG_INFO("");
}

void printMDIncrementalRefreshVolume37(mktdata::MDIncrementalRefreshVolume37 &message)
{
    LOG_INFO("message.type=MDIncrementalRefreshVolume37");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.transactTime=", message.transactTime());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    LOG_INFO("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    LOG_INFO("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    LOG_INFO("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    LOG_INFO("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    LOG_INFO("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    LOG_INFO("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    LOG_INFO("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    LOG_INFO("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());

    mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = message.noMDEntries();
    while (noMDEntries.hasNext())
    {
        noMDEntries.next();
        LOG_INFO("message.noMDEntries.mDEntrySize=", (int)noMDEntries.mDEntrySize());
        LOG_INFO("message.noMDEntries.securityID=", (int)noMDEntries.securityID());
        LOG_INFO("message.noMDEntries.rptSeq=", (int)noMDEntries.rptSeq());
        LOG_INFO("message.noMDEntries.mDUpdateAction=", (int)noMDEntries.mDUpdateAction());
        LOG_INFO("message.noMDEntries.mDEntryType=", noMDEntries.mDEntryType()[0]);
    }
    LOG_INFO("");
}

void printMDInstrumentDefinitionFuture27(mktdata::MDInstrumentDefinitionFuture27 &message)
{
    LOG_INFO("message.type=MDInstrumentDefinitionFuture27");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    LOG_INFO("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    LOG_INFO("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    LOG_INFO("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    LOG_INFO("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    LOG_INFO("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    LOG_INFO("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    LOG_INFO("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    LOG_INFO("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());

    LOG_INFO("message.totNumReports=", message.totNumReports());
    LOG_INFO("message.securityUpdateAction=", message.securityUpdateAction());
    LOG_INFO("message.lastUpdateTime=", message.lastUpdateTime());
    LOG_INFO("message.tradingReferenceDate=", message.tradingReferenceDate());

    mktdata::MDInstrumentDefinitionFuture27::NoEvents& noEvents = message.noEvents();
    while (noEvents.hasNext())
    {
        noEvents.next();
        LOG_INFO("message.noEvents.eventType=", noEvents.eventType());
        LOG_INFO("message.noEvents.eventTime=", noEvents.eventTime());
    }

    mktdata::MDInstrumentDefinitionFuture27::NoMDFeedTypes& noMDFeedTypes = message.noMDFeedTypes();
    LOG_INFO("message.noMDFeedTypes.count=", noMDFeedTypes.count());
    while (noMDFeedTypes.hasNext())
    {
        noMDFeedTypes.next();
        LOG_INFO("message.noMDFeedTypes.mdFeedType=", noMDFeedTypes.getMDFeedTypeAsString());
        LOG_INFO("message.noMDFeedTypes.marketDepth=", (int)noMDFeedTypes.marketDepth());
    }

    mktdata::MDInstrumentDefinitionFuture27::NoInstAttrib& is = message.noInstAttrib();
    while (is.hasNext())
    {
        is.next();
          LOG_INFO("message.NoInstAttrib.instAttribValue=", is.instAttribValue().electronicMatchEligible());
    }

    mktdata::MDInstrumentDefinitionFuture27::NoLotTypeRules& ts = message.noLotTypeRules();
    while (ts.hasNext())
    {
        ts.next();
          LOG_INFO("message.NoLotTypeRules.lotType=", ts.lotType());
          LOG_INFO("message.NoLotTypeRules.minLotSize.mantissa=", ts.minLotSize().mantissa());
    }

    LOG_INFO("");
}

void printSnapshotFullRefresh38(mktdata::SnapshotFullRefresh38 &message)
{
    LOG_INFO("message.type=SnapshotFullRefresh38");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.lastMsgSeqNumProcessed=", message.lastMsgSeqNumProcessed());
    LOG_INFO("message.totNumReports=", message.totNumReports());
    LOG_INFO("message.securityID=", message.securityID());
    LOG_INFO("message.rptSeq=", message.rptSeq());
    LOG_INFO("message.transactTime=", message.transactTime());
    LOG_INFO("message.lastUpdateTime=", message.lastUpdateTime());

    mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = message.noMDEntries();
    while (noMDEntries.hasNext())
    {
        noMDEntries.next();
        LOG_INFO("message.noMDEntries.mDEntryPx=", noMDEntries.mDEntryPx().mantissa());
        LOG_INFO("message.noMDEntries.mDEntrySize=", (int)noMDEntries.mDEntrySize());
        LOG_INFO("message.noMDEntries.numberOfOrders=", (int)noMDEntries.numberOfOrders());
        LOG_INFO("message.noMDEntries.mDPriceLevel=", (int)noMDEntries.mDPriceLevel());
        LOG_INFO("message.noMDEntries.tradingReferenceDate=", (int)noMDEntries.tradingReferenceDate());
        LOG_INFO("message.noMDEntries.mDEntryType=", noMDEntries.mDEntryType());
    }

    LOG_INFO("");
}

void printAdminLogin15(mktdata::AdminLogin15 &message)
{
    LOG_INFO("message.type=AdminLogin15");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.heartBtInt=", (int)message.heartBtInt());

    LOG_INFO("");
}

void printAdminLogout16(mktdata::AdminLogout16 &message)
{
    LOG_INFO("message.type=AdminLogout16");
    LOG_INFO("message.sbeSemanticType=", message.sbeSemanticType());
    LOG_INFO("message.text=", message.text());

    LOG_INFO("");
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            LOG_ERROR("Usage: sbe_test <template_id>");
            LOG_ERROR("Ex:    sbe_test 30");

            return 1;
        }

        fh::cme::market::message::SBEEncoder encoder;
        encoder.Start_encode(boost::lexical_cast<std::uint16_t>(argv[1]));
        LOG_INFO(encoder.Encoded_hex_str());

        fh::core::assist::TimeMeasurer t;

        auto encoded = encoder.Encoded_buffer();
        fh::cme::market::message::SBEDecoder decoder(encoded.first, encoded.second);
        std::pair<std::shared_ptr<void>, std::shared_ptr<void>> message_info = decoder.Start_decode();

        auto header = static_cast<mktdata::MessageHeader*>(message_info.first.get());
        std::uint16_t templateId = header->templateId();

        void *message = message_info.second.get();
        if(templateId == 30)    // SecurityStatus30
        {
            auto ss30 = static_cast<mktdata::SecurityStatus30*>(message);
            printSecurityStatus30(*ss30);
        }
        else if(templateId == 32)    // MDIncrementalRefreshBook32
        {
            auto md32 = static_cast<mktdata::MDIncrementalRefreshBook32*>(message);
            printMDIncrementalRefreshVolume32(*md32);
        }
        else if(templateId == 37)    // MDIncrementalRefreshVolume37
        {
            auto md37 = static_cast<mktdata::MDIncrementalRefreshVolume37*>(message);
            printMDIncrementalRefreshVolume37(*md37);
        }
        else if(templateId == 27)    // MDInstrumentDefinitionFuture27
        {
            auto md27 = static_cast<mktdata::MDInstrumentDefinitionFuture27*>(message);
            printMDInstrumentDefinitionFuture27(*md27);
        }
        else if(templateId == 38)    // SnapshotFullRefresh38
        {
            auto md38 = static_cast<mktdata::SnapshotFullRefresh38*>(message);
            printSnapshotFullRefresh38(*md38);
        }
        else if(templateId == 15)    // AdminLogin15
        {
            auto md15 = static_cast<mktdata::AdminLogin15*>(message);
            printAdminLogin15(*md15);
        }
        else if(templateId == 16)    // AdminLogout16
        {
            auto md16 = static_cast<mktdata::AdminLogout16*>(message);
            printAdminLogout16(*md16);
        }

        LOG_INFO(t.Elapsed_nanoseconds(), "ns");
        LOG_INFO("");

        fh::cme::market::message::MdpMessage mdp(encoded.first, encoded.second, 1000, 1, 12345678);
        t.Restart();
        std::string m = mdp.Serialize();
        auto time = t.Elapsed_nanoseconds();
        LOG_INFO(m);
        LOG_INFO(time, "ns");
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./sbe_test 30
