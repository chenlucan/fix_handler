
#include "sbe_encoder.h"
#include "sbe_decoder.h"

void printSecurityStatus30(mktdata::SecurityStatus30 &message)
{
    std::cout << "message.type=SecurityStatus30" << std::endl;
    std::cout << "message.sbeSemanticType=" << message.sbeSemanticType() << std::endl;
    std::cout << "message.transactTime=" << message.transactTime() << std::endl;
    std::cout << "message.securityGroup=" << message.getSecurityGroupAsString() << std::endl;
    std::cout << "message.asset=" << message.getAssetAsString() << std::endl;
    std::cout << "message.securityID=" << message.securityID() << std::endl;
    std::cout << "message.tradeDate=" << message.tradeDate() << std::endl;

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    std::cout << "message.matchEventIndicator.lastTradeMsg=" << matchEventIndicator.lastTradeMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastVolumeMsg=" << matchEventIndicator.lastVolumeMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastQuoteMsg=" << matchEventIndicator.lastQuoteMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastStatsMsg=" << matchEventIndicator.lastStatsMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastImpliedMsg=" << matchEventIndicator.lastImpliedMsg() << std::endl;
    std::cout << "message.matchEventIndicator.recoveryMsg=" << matchEventIndicator.recoveryMsg() << std::endl;
    std::cout << "message.matchEventIndicator.reserved=" << matchEventIndicator.reserved() << std::endl;
    std::cout << "message.matchEventIndicator.endOfEvent=" << matchEventIndicator.endOfEvent() << std::endl;
    
    std::cout << "message.securityTradingStatus=" << message.securityTradingStatus() << std::endl;
    std::cout << "message.haltReason=" << message.haltReason() << std::endl;
    std::cout << "message.securityTradingEvent=" << message.securityTradingEvent() << std::endl;
    std::cout << std::endl;
}

void printMDIncrementalRefreshVolume37(mktdata::MDIncrementalRefreshVolume37 &message)
{
    std::cout << "message.type=MDIncrementalRefreshVolume37" << std::endl;
    std::cout << "message.sbeSemanticType=" << message.sbeSemanticType() << std::endl;
    std::cout << "message.transactTime=" << message.transactTime() << std::endl;

    mktdata::MatchEventIndicator& matchEventIndicator = message.matchEventIndicator();
    std::cout << "message.matchEventIndicator.lastTradeMsg=" << matchEventIndicator.lastTradeMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastVolumeMsg=" << matchEventIndicator.lastVolumeMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastQuoteMsg=" << matchEventIndicator.lastQuoteMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastStatsMsg=" << matchEventIndicator.lastStatsMsg() << std::endl;
    std::cout << "message.matchEventIndicator.lastImpliedMsg=" << matchEventIndicator.lastImpliedMsg() << std::endl;
    std::cout << "message.matchEventIndicator.recoveryMsg=" << matchEventIndicator.recoveryMsg() << std::endl;
    std::cout << "message.matchEventIndicator.reserved=" << matchEventIndicator.reserved() << std::endl;
    std::cout << "message.matchEventIndicator.endOfEvent=" << matchEventIndicator.endOfEvent() << std::endl;
    
    mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = message.noMDEntries();
    while (noMDEntries.hasNext())
    {
        noMDEntries.next();
        std::cout << "message.noMDEntries.mDEntrySize=" << (int)noMDEntries.mDEntrySize() << std::endl;
        std::cout << "message.noMDEntries.securityID=" << (int)noMDEntries.securityID() << std::endl;
        std::cout << "message.noMDEntries.rptSeq=" << (int)noMDEntries.rptSeq() << std::endl;
        std::cout << "message.noMDEntries.mDUpdateAction=" << (int)noMDEntries.mDUpdateAction() << std::endl;
        std::cout << "message.noMDEntries.mDEntryType=" << noMDEntries.mDEntryType()[0] << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: sbe_test <template_id>\n";
            std::cerr << "Ex:    sbe_test 30\n";
            
            return 1;
        }

        rczg::SBEEncoder encoder;
        encoder.Start_encode(boost::lexical_cast<std::uint16_t>(argv[1]));
        std::cout << encoder.Encoded_hex_str() << std::endl;
        
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
        
        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

// ./sbe_test
