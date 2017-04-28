
#include <gtest/gtest.h>
#include "core/assist/utility.h"
#include "tmalpha/market/cme_data_consumer.h"
#include "tmalpha/market/mock_market_data_provider.h"
#include "tmalpha/market/mock_market_replay_listener.h"
#include "tmalpha/market/market_simulater.h"

std::vector<std::string> messages_all_type {
    // 新建：security id=1, symbol=CON-1, depth=4
    "{ \"insertTime\" : \"1493010903832910549\", \"sendingTime\" : \"1493010873398873558\", \"packetSeqNum\" : \"1\", \"sbeType\" : \"d\", \"message\" : { \"type\" : \"MDInstrumentDefinitionOption41\", \"securityUpdateAction\" : \"A\", \"SecurityGroup\" : \"efghjk\", \"Asset\" : \"A1234A\", \"Symbol\" : \"CON-1\", \"securityID\" : \"1\", \"SecurityType\" : \"D1234D\", \"totNumReports\" : \"10\", \"highLimitPrice\" : { \"mantissa\" : \"700004\", \"exponent\" : \"-7\" }, \"lowLimitPrice\" : { \"mantissa\" : \"700005\", \"exponent\" : \"-7\" }, \"minPriceIncrement\" : { \"mantissa\" : \"700007\", \"exponent\" : \"-7\" }, \"noMDFeedTypes\" : [ { \"MDFeedType\" : \"GBX\", \"marketDepth\" : \"4\" }, { \"MDFeedType\" : \"GBI\", \"marketDepth\" : \"6\" } ] } }",
    // 新建：security id=3, symbol=CON-3, depth=5
    "{ \"insertTime\" : \"1493010903833937352\", \"sendingTime\" : \"1493010873900292576\", \"packetSeqNum\" : \"2\", \"sbeType\" : \"d\", \"message\" : { \"type\" : \"MDInstrumentDefinitionFuture27\", \"securityUpdateAction\" : \"A\", \"SecurityGroup\" : \"efghjk\", \"Asset\" : \"A1234A\", \"Symbol\" : \"CON-3\", \"securityID\" : \"3\", \"SecurityType\" : \"D1234D\", \"minPriceIncrement\" : { \"mantissa\" : \"700001\", \"exponent\" : \"-7\" }, \"totNumReports\" : \"10\", \"highLimitPrice\" : { \"mantissa\" : \"700004\", \"exponent\" : \"-7\" }, \"lowLimitPrice\" : { \"mantissa\" : \"700005\", \"exponent\" : \"-7\" }, \"minPriceIncrementAmount\" : { \"mantissa\" : \"700007\", \"exponent\" : \"-7\" }, \"noMDFeedTypes\" : [ { \"MDFeedType\" : \"GBX\", \"marketDepth\" : \"5\" }, { \"MDFeedType\" : \"GBI\", \"marketDepth\" : \"7\" } ] } }",
    // 修改：security id=1, symbol=CON-1-NEW, depth=3
    "{ \"insertTime\" : \"1493010903835832139\", \"sendingTime\" : \"1493010873900292576\", \"packetSeqNum\" : \"2\", \"sbeType\" : \"d\", \"message\" : { \"type\" : \"MDInstrumentDefinitionFuture27\", \"securityUpdateAction\" : \"A\", \"SecurityGroup\" : \"efghjk\", \"Asset\" : \"A1234A\", \"Symbol\" : \"CON-1-NEW\", \"securityID\" : \"1\", \"SecurityType\" : \"D1234D\", \"minPriceIncrement\" : { \"mantissa\" : \"700001\", \"exponent\" : \"-7\" }, \"totNumReports\" : \"10\", \"highLimitPrice\" : { \"mantissa\" : \"700004\", \"exponent\" : \"-7\" }, \"lowLimitPrice\" : { \"mantissa\" : \"700005\", \"exponent\" : \"-7\" }, \"minPriceIncrementAmount\" : { \"mantissa\" : \"700007\", \"exponent\" : \"-7\" }, \"noMDFeedTypes\" : [ { \"MDFeedType\" : \"GBX\", \"marketDepth\" : \"3\" }, { \"MDFeedType\" : \"GBI\", \"marketDepth\" : \"10\" } ] } }",
    // 忽略：price level = 0
    "{ \"insertTime\" : \"1493010903843043222\", \"sendingTime\" : \"1493010903293871329\", \"packetSeqNum\" : \"1\", \"sbeType\" : \"W\", \"message\" : { \"type\" : \"SnapshotFullRefreshOrderBook44\", \"transactTime\" : \"1493010903294309347\", \"lastMsgSeqNumProcessed\" : \"15\", \"totNumReports\" : \"5\", \"securityID\" : \"1\", \"noMDEntries\" : [ { \"orderID\" : \"123\", \"mDEntryPx\" : { \"mantissa\" : \"60000000\", \"exponent\" : \"-7\" }, \"mDDisplayQty\" : \"456\", \"mDEntryType\" : \"0\", \"mDOrderPriority\" : \"3678\" }, { \"orderID\" : \"567\", \"mDEntryPx\" : { \"mantissa\" : \"60000000\", \"exponent\" : \"-7\" }, \"mDDisplayQty\" : \"587\", \"mDEntryType\" : \"1\", \"mDOrderPriority\" : \"2345\" }, { \"orderID\" : \"222\", \"mDEntryPx\" : { \"mantissa\" : \"60000000\", \"exponent\" : \"-7\" }, \"mDEntryType\" : \"0\" } ] } }",
    // contract=1/CON-1-NEW, bid=[price=99.000000, size=900], offer=[price=0.000000, size=0][price=0.000000, size=0][price=100.000000, size=700]
    // contract=3/CON-3, bid=, offer=
    "{ \"insertTime\" : \"1493010903844195668\", \"sendingTime\" : \"1493010903293871329\", \"packetSeqNum\" : \"1\", \"sbeType\" : \"W\", \"message\" : { \"type\" : \"SnapshotFullRefresh38\", \"transactTime\" : \"1493010903294998243\", \"lastMsgSeqNumProcessed\" : \"15\", \"totNumReports\" : \"4\", \"securityID\" : \"1\", \"rptSeq\" : \"100\", \"noMDEntries\" : [ { \"mDEntryType\" : \"0\", \"mDEntrySize\" : \"900\", \"numberOfOrders\" : \"90\", \"mDPriceLevel\" : \"1\", \"mDEntryPx\" : { \"mantissa\" : \"990000000\", \"exponent\" : \"-7\" } }, { \"mDEntryType\" : \"1\", \"mDEntrySize\" : \"700\", \"numberOfOrders\" : \"70\", \"mDPriceLevel\" : \"3\", \"mDEntryPx\" : { \"mantissa\" : \"1000000000\", \"exponent\" : \"-7\" } } ] } }",
    // contract=1/CON-1-NEW, bid=contract=CON-1-NEW, bid=[price=99.000000, size=900], offer=[price=0.000000, size=0][price=0.000000, size=0][price=100.000000, size=700]
    // contract=3/CON-3, contract=CON-3, bid=[price=0.000000, size=0][price=200.000000, size=20], offer=[price=0.000000, size=0][price=300.330000, size=50]
    "{ \"insertTime\" : \"1493010903844762643\", \"sendingTime\" : \"1493010903795732045\", \"packetSeqNum\" : \"2\", \"sbeType\" : \"W\", \"message\" : { \"type\" : \"SnapshotFullRefresh38\", \"transactTime\" : \"1493010903294998243\", \"lastMsgSeqNumProcessed\" : \"16\", \"totNumReports\" : \"4\", \"securityID\" : \"3\", \"rptSeq\" : \"100\", \"noMDEntries\" : [ { \"mDEntryType\" : \"0\", \"mDEntrySize\" : \"20\", \"numberOfOrders\" : \"2\", \"mDPriceLevel\" : \"2\", \"mDEntryPx\" : { \"mantissa\" : \"2000000000\", \"exponent\" : \"-7\" } }, { \"mDEntryType\" : \"1\", \"mDEntrySize\" : \"50\", \"numberOfOrders\" : \"5\", \"mDPriceLevel\" : \"2\", \"mDEntryPx\" : { \"mantissa\" : \"3003300000\", \"exponent\" : \"-7\" } } ] } }",
    // contract=1/CON-1-NEW, bid=contract=CON-1-NEW, bid=[price=99.000000, size=900], offer=[price=0.000000, size=0][price=0.000000, size=0][price=100.000000, size=700]
    // contract=3/CON-3, contract=CON-3, bid=[price=0.000000, size=0][price=200.000000, size=20], offer=[price=0.000000, size=0][price=300.330000, size=50]
    "{ \"insertTime\" : \"1493010903848362889\", \"sendingTime\" : \"1493010825946991222\", \"packetSeqNum\" : \"15\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshBook32\", \"transactTime\" : \"1493010825947141569\", \"noMDEntries\" : [ { \"securityID\" : \"1\", \"rptSeq\" : \"73\", \"mDPriceLevel\" : \"7\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"1\", \"mDEntrySize\" : \"40\", \"numberOfOrders\" : \"16\", \"mDEntryPx\" : { \"mantissa\" : \"420000000\", \"exponent\" : \"-7\" } }, { \"securityID\" : \"3\", \"rptSeq\" : \"73\", \"mDPriceLevel\" : \"2\", \"mDUpdateAction\" : \"1\", \"mDEntryType\" : \"1\", \"mDEntrySize\" : \"32\", \"numberOfOrders\" : \"19\", \"mDEntryPx\" : { \"mantissa\" : \"280000000\", \"exponent\" : \"-7\" } } ] } }",
    // contract=1/CON-1-NEW, bid=contract=CON-1-NEW, bid=[price=99.000000, size=900], offer=[price=0.000000, size=0][price=0.000000, size=0][price=100.000000, size=700]
    // contract=3/CON-3, contract=CON-3, bid=[price=0.000000, size=0][price=200.000000, size=20], offer=[price=0.000000, size=0][price=300.330000, size=50]
    "{ \"insertTime\" : \"1493010903851117091\", \"sendingTime\" : \"1493010825946991222\", \"packetSeqNum\" : \"15\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshDailyStatistics33\", \"transactTime\" : \"1493010825947817423\", \"noMDEntries\" : [ { \"securityID\" : \"3\", \"rptSeq\" : \"73\", \"tradingReferenceDate\" : \"18305\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"0\", \"mDEntrySize\" : \"62\", \"mDEntryPx\" : { \"mantissa\" : \"90000000\", \"exponent\" : \"-7\" } } ] } }",
    // contract=1/CON-1-NEW, bid=[price=99.000000, size=900][price=0.000000, size=0][price=123.000000, size=62], offer=[price=0.000000, size=0][price=0.000000, size=0][price=100.000000, size=700]
    // contract=3/CON-3, contract=CON-3, bid=[price=0.000000, size=0][price=200.000000, size=20], offer=[price=0.000000, size=0][price=300.330000, size=50]
    "{ \"insertTime\" : \"1493010903855936087\", \"sendingTime\" : \"1493010826449696972\", \"packetSeqNum\" : \"16\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshVolume37\", \"transactTime\" : \"1493010826449834516\", \"noMDEntries\" : [ { \"securityID\" : \"3\", \"rptSeq\" : \"73\", \"tradingReferenceDate\" : \"18305\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"0\", \"mDEntrySize\" : \"62\", \"mDEntryPx\" : { \"mantissa\" : \"90000000\", \"exponent\" : \"-7\" } }, { \"securityID\" : \"1\", \"rptSeq\" : \"73\", \"tradingReferenceDate\" : \"18305\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"0\", \"mDPriceLevel\" : \"3\",  \"mDEntrySize\" : \"62\", \"mDEntryPx\" : { \"mantissa\" : \"1230000000\", \"exponent\" : \"-7\" } } ] } }",
    // 忽略：message type = R
    "{ \"insertTime\" : \"1493010903856824461\", \"sendingTime\" : \"1493010826950452080\", \"packetSeqNum\" : \"17\", \"sbeType\" : \"R\", \"message\" : { \"type\" : \"QuoteRequest39\", \"quoteReqID\" : \"xx\", \"transactTime\" : \"1493010826950588330\" } }",
    // 忽略：entity type = e
    "{ \"insertTime\" : \"1493010903858508385\", \"sendingTime\" : \"1493010826950452080\", \"packetSeqNum\" : \"17\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshVolume37\", \"transactTime\" : \"1493010826950890457\", \"noMDEntries\" : [ { \"mDEntrySize\" : \"71\", \"securityID\" : \"1\", \"rptSeq\" : \"73\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"e\" }, { \"mDEntrySize\" : \"61\", \"securityID\" : \"3\", \"rptSeq\" : \"63\", \"mDUpdateAction\" : \"1\", \"mDEntryType\" : \"e\" } ] } }",
    // 忽略：entity type = 2
    "{ \"insertTime\" : \"1493010903863585881\", \"sendingTime\" : \"1493010827452165197\", \"packetSeqNum\" : \"18\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshTrade36\", \"transactTime\" : \"1493010827452294374\", \"noMDEntries\" : [ { \"mDEntryPx\" : { \"mantissa\" : \"456000000\", \"exponent\" : \"-7\" }, \"mDEntrySize\" : \"74\", \"securityID\" : \"1\", \"rptSeq\" : \"73\", \"tradeID\" : \"7\", \"aggressorSide\" : \"1\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"2\", \"numberOfOrders\" : \"8\" }, { \"mDEntryPx\" : { \"mantissa\" : \"162000000\", \"exponent\" : \"-7\" }, \"mDEntrySize\" : \"51\", \"securityID\" : \"3\", \"rptSeq\" : \"73\", \"tradeID\" : \"6\", \"aggressorSide\" : \"2\", \"mDUpdateAction\" : \"2\", \"mDEntryType\" : \"2\", \"numberOfOrders\" : \"4\" } ] } }",
    // 忽略：message type = f
    "{ \"insertTime\" : \"1493010903865858063\", \"sendingTime\" : \"1493010827452165197\", \"packetSeqNum\" : \"18\", \"sbeType\" : \"f\", \"message\" : { \"type\" : \"SecurityStatus30\", \"transactTime\" : \"1493010827453048765\", \"SecurityGroup\" : \"ABCDEF\", \"Asset\" : \"xyz\", \"tradeDate\" : \"2017\", \"securityTradingStatus\" : \"17\", \"haltReason\" : \"4\", \"securityTradingEvent\" : \"4\", \"securityID\" : \"72\" } }",
    // contract=1/CON-1-NEW, bid=[price=99.000000, size=900][price=0.000000, size=0][price=123.000000, size=62], offer=[price=0.000000, size=0][price=0.000000, size=0][price=12.000000, size=46]
    // contract=3/CON-3, contract=CON-3, bid=[price=0.000000, size=0][price=49.000000, size=68], offer=[price=0.000000, size=0][price=300.330000, size=50]
    "{ \"insertTime\" : \"1493010903871898063\", \"sendingTime\" : \"1493010827954103879\", \"packetSeqNum\" : \"19\", \"sbeType\" : \"X\", \"message\" : { \"type\" : \"MDIncrementalRefreshBook32\", \"transactTime\" : \"1493010827954243136\", \"noMDEntries\" : [ { \"securityID\" : \"1\", \"rptSeq\" : \"73\", \"mDPriceLevel\" : \"3\", \"mDUpdateAction\" : \"0\", \"mDEntryType\" : \"1\", \"mDEntrySize\" : \"46\", \"numberOfOrders\" : \"18\", \"mDEntryPx\" : { \"mantissa\" : \"120000000\", \"exponent\" : \"-7\" } }, { \"securityID\" : \"3\", \"rptSeq\" : \"73\", \"mDPriceLevel\" : \"2\", \"mDUpdateAction\" : \"1\", \"mDEntryType\" : \"0\", \"mDEntrySize\" : \"68\", \"numberOfOrders\" : \"25\", \"mDEntryPx\" : { \"mantissa\" : \"490000000\", \"exponent\" : \"-7\" } } ] } }"
};

std::vector<std::string> messages_all_type_result{
    "contract=CON-1-NEW, bid=[price=99.000000, size=900][price=0.000000, size=0][price=123.000000, size=62], offer=[price=0.000000, size=0][price=0.000000, size=0][price=12.000000, size=46]",
    "contract=CON-3, bid=[price=0.000000, size=0][price=49.000000, size=68], offer=[price=0.000000, size=0][price=300.330000, size=50]"
};

TEST(MarketSimulaterTest, Test001_ReadOnce)
{
    std::uint32_t page_size = 15;
    fh::tmalpha::market::MockMarketDataProvider *provider = new fh::tmalpha::market::MockMarketDataProvider(messages_all_type, page_size);
    fh::tmalpha::market::DataConsumer *consume = new fh::tmalpha::market::CmeDataConsumer();
    fh::tmalpha::market::MockMarketReplayListener *listener = new fh::tmalpha::market::MockMarketReplayListener();

    fh::tmalpha::market::MarketSimulater *simulater = new fh::tmalpha::market::MarketSimulater(provider, consume);
    simulater->Add_replay_listener(listener);
    simulater->Start();

    std::unordered_map<std::string , pb::dms::L2> states = listener->Get_states();

    EXPECT_EQ(states.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-1-NEW")), messages_all_type_result[0]);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-3")), messages_all_type_result[1]);
}

TEST(MarketSimulaterTest, Test001_ReadMultiple)
{
    std::uint32_t page_size = 5;
    fh::tmalpha::market::MockMarketDataProvider *provider = new fh::tmalpha::market::MockMarketDataProvider(messages_all_type, page_size);
    fh::tmalpha::market::DataConsumer *consume = new fh::tmalpha::market::CmeDataConsumer();
    fh::tmalpha::market::MockMarketReplayListener *listener = new fh::tmalpha::market::MockMarketReplayListener();

    fh::tmalpha::market::MarketSimulater *simulater = new fh::tmalpha::market::MarketSimulater(provider, consume);
    simulater->Add_replay_listener(listener);
    simulater->Start();

    std::unordered_map<std::string , pb::dms::L2> states = listener->Get_states();

    EXPECT_EQ(states.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-1-NEW")), messages_all_type_result[0]);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-3")), messages_all_type_result[1]);
}

TEST(MarketSimulaterTest, Test001_3xSpeed)
{
    std::uint32_t page_size = 15;
    fh::tmalpha::market::MockMarketDataProvider *provider = new fh::tmalpha::market::MockMarketDataProvider(messages_all_type, page_size);
    fh::tmalpha::market::DataConsumer *consume = new fh::tmalpha::market::CmeDataConsumer();
    fh::tmalpha::market::MockMarketReplayListener *listener = new fh::tmalpha::market::MockMarketReplayListener();

    fh::tmalpha::market::MarketSimulater *simulater = new fh::tmalpha::market::MarketSimulater(provider, consume);
    simulater->Add_replay_listener(listener);
    simulater->Speed(3);
    simulater->Start();

    std::unordered_map<std::string , pb::dms::L2> states = listener->Get_states();

    EXPECT_EQ(states.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-1-NEW")), messages_all_type_result[0]);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(states.at("CON-3")), messages_all_type_result[1]);
}
