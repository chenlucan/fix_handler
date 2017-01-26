
#include "sbe_encoder.h"
#include "sbe_decoder.h"
#include "logger.h"

void printSecurityStatus30(mktdata::SecurityStatus30 &message)
{
    rczg::Logger::Info("message.type=SecurityStatus30");
    rczg::Logger::Info("message.sbeSemanticType=", message.sbeSemanticType());
    rczg::Logger::Info("message.transactTime=", message.transactTime());
    rczg::Logger::Info("message.securityGroup=", message.getSecurityGroupAsString());
    rczg::Logger::Info("message.asset=", message.getAssetAsString());
    rczg::Logger::Info("message.securityID=", message.securityID());
    rczg::Logger::Info("message.tradeDate=", message.tradeDate());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    rczg::Logger::Info("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    rczg::Logger::Info("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    rczg::Logger::Info("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    rczg::Logger::Info("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());
    
    rczg::Logger::Info("message.securityTradingStatus=", message.securityTradingStatus());
    rczg::Logger::Info("message.haltReason=", message.haltReason());
    rczg::Logger::Info("message.securityTradingEvent=", message.securityTradingEvent());
    rczg::Logger::Info("");
}

void printMDIncrementalRefreshVolume37(mktdata::MDIncrementalRefreshVolume37 &message)
{
    rczg::Logger::Info("message.type=MDIncrementalRefreshVolume37");
    rczg::Logger::Info("message.sbeSemanticType=", message.sbeSemanticType());
    rczg::Logger::Info("message.transactTime=", message.transactTime());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    rczg::Logger::Info("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    rczg::Logger::Info("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    rczg::Logger::Info("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    rczg::Logger::Info("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());
    
    mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = message.noMDEntries();
    while (noMDEntries.hasNext())
    {
        noMDEntries.next();
        rczg::Logger::Info("message.noMDEntries.mDEntrySize=", (int)noMDEntries.mDEntrySize());
        rczg::Logger::Info("message.noMDEntries.securityID=", (int)noMDEntries.securityID());
        rczg::Logger::Info("message.noMDEntries.rptSeq=", (int)noMDEntries.rptSeq());
        rczg::Logger::Info("message.noMDEntries.mDUpdateAction=", (int)noMDEntries.mDUpdateAction());
        rczg::Logger::Info("message.noMDEntries.mDEntryType=", noMDEntries.mDEntryType()[0]);
    }
    rczg::Logger::Info("");
}

void printMDInstrumentDefinitionFuture27(mktdata::MDInstrumentDefinitionFuture27 &message)
{
    rczg::Logger::Info("message.type=MDInstrumentDefinitionFuture27");
    rczg::Logger::Info("message.sbeSemanticType=", message.sbeSemanticType());

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    rczg::Logger::Info("message.matchEventIndicator.lastTradeMsg=", matchEventIndicator.lastTradeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastVolumeMsg=", matchEventIndicator.lastVolumeMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastQuoteMsg=", matchEventIndicator.lastQuoteMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastStatsMsg=", matchEventIndicator.lastStatsMsg());
    rczg::Logger::Info("message.matchEventIndicator.lastImpliedMsg=", matchEventIndicator.lastImpliedMsg());
    rczg::Logger::Info("message.matchEventIndicator.recoveryMsg=", matchEventIndicator.recoveryMsg());
    rczg::Logger::Info("message.matchEventIndicator.reserved=", matchEventIndicator.reserved());
    rczg::Logger::Info("message.matchEventIndicator.endOfEvent=", matchEventIndicator.endOfEvent());
        
    rczg::Logger::Info("message.totNumReports=", message.totNumReports());
    rczg::Logger::Info("message.securityUpdateAction=", message.securityUpdateAction());
    rczg::Logger::Info("message.lastUpdateTime=", message.lastUpdateTime());

    rczg::Logger::Info("");
}

void printSnapshotFullRefresh38(mktdata::SnapshotFullRefresh38 &message)
{
    rczg::Logger::Info("message.type=SnapshotFullRefresh38");
    rczg::Logger::Info("message.sbeSemanticType=", message.sbeSemanticType());
    rczg::Logger::Info("message.lastMsgSeqNumProcessed=", message.lastMsgSeqNumProcessed());
    rczg::Logger::Info("message.totNumReports=", message.totNumReports());
    rczg::Logger::Info("message.securityID=", message.securityID());
    rczg::Logger::Info("message.rptSeq=", message.rptSeq());
    rczg::Logger::Info("message.transactTime=", message.transactTime());
    rczg::Logger::Info("message.lastUpdateTime=", message.lastUpdateTime());

    rczg::Logger::Info("");
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            rczg::Logger::Error("Usage: sbe_test <template_id>");
            rczg::Logger::Error("Ex:    sbe_test 30");
            
            return 1;
        }

        rczg::SBEEncoder encoder;
        encoder.Start_encode(boost::lexical_cast<std::uint16_t>(argv[1]));
        rczg::Logger::Info(encoder.Encoded_hex_str());
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto encoded = encoder.Encoded_buffer();
        rczg::SBEDecoder decoder(encoded.first, encoded.second);
        std::pair<std::shared_ptr<void>, std::shared_ptr<void>> message_info = decoder.Start_decode();
        
        auto header = static_cast<mktdata::MessageHeader*>(message_info.first.get());
        std::uint16_t templateId = header->templateId();
        
        void *message = message_info.second.get();
        if(templateId == 30)    // SecurityStatus30
        {
            auto ss30 = static_cast<mktdata::SecurityStatus30*>(message);
            printSecurityStatus30(*ss30);
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
        
        auto finish = std::chrono::high_resolution_clock::now();
        rczg::Logger::Info(std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count(), "ns");
    }
    catch (std::exception& e)
    {
        rczg::Logger::Error("Exception: ", e.what());
    }

    return 0;
}

// ./sbe_test 30
