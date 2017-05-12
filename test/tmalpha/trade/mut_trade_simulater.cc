#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>
#include "tmalpha/trade/mock_trade_market_listener.h"
#include "tmalpha/trade/mock_trade_exchange_listener.h"
#include "tmalpha/trade/trade_algorithm_simple.h"
#include "tmalpha/trade/trade_simulater.h"

TEST(TradeSimulaterTest, Test001_SendContracts)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,100,1000,1000000";
    contracts["CON-2"] = "1,2,30,4000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater(&market_listener, &exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start(std::vector<::pb::ems::Order>());

    // 此时 market listener 应该收到 2 条合约定义信息
    auto &reveived_contracts = market_listener.Contracts();
    EXPECT_EQ(reveived_contracts.size(), 2);

    EXPECT_EQ(reveived_contracts[0].name(), "CON-1");

    EXPECT_EQ(reveived_contracts[1].name(), "CON-2");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test002_AddInvalidOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater(&market_listener, &exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start(std::vector<::pb::ems::Order>());

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("AAA");

    trade_simulater.Add(order);

    // 此时由于订单的合约不存在，会被拒绝
    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].message(), "contract not exist");

    trade_simulater.Stop();
}
