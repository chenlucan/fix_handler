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

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    // 此时 market listener 应该收到 2 条合约定义信息
    auto &reveived_contracts = market_listener.Contracts();
    EXPECT_EQ(reveived_contracts.size(), 2);

    EXPECT_EQ(reveived_contracts[0].name(), "CON-1");
    EXPECT_EQ(std::stod(reveived_contracts[0].tick_size()), 1);
    EXPECT_EQ(std::stod(reveived_contracts[0].lower_limit()), 10);
    EXPECT_EQ(std::stod(reveived_contracts[0].upper_limit()), 10000);

    EXPECT_EQ(reveived_contracts[1].name(), "CON-2");
    EXPECT_EQ(std::stod(reveived_contracts[1].tick_size()), 0.02);
    EXPECT_EQ(std::stod(reveived_contracts[1].lower_limit()), 0.3);
    EXPECT_EQ(std::stod(reveived_contracts[1].upper_limit()), 40);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test002_InvalidContract)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("AAA");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::CONTRACT_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test003_PriceTooLow)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_price("9.00");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_PRICE_TOO_LOW);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test004_PriceTooHigh)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_price("10000.01");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_PRICE_TOO_HIGH);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test005_PriceInvalid)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_price("21");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_PRICE_INVALID);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test006_MarketOrderPriceInvalid)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Market);
    order.set_price("30");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::MARKET_ORDER_PRICE_INVALID);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test007_LimitOrderPriceInvalid)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("0");

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::LIMIT_ORDER_PRICE_INVALID);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test008_OrderQuantityInvalid)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("30");
    order.set_quantity(0);

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_QUANTITY_INVALID);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test009_AddOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100");
    order.set_quantity(10);

    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 2);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Pending);
    EXPECT_EQ(received_result[0].exchange_order_id(), "Order-1");
    EXPECT_EQ(received_result[1].status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(received_result[1].exchange_order_id(), "Order-1");
    EXPECT_EQ(received_result[1].filled_quantity(), 0);
    
    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    
    EXPECT_EQ(received_l2.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2[0]), "contract=CON-1, bid=[price=100.000000, size=10], offer=");
    EXPECT_EQ(received_bid.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bid[0]), "contract=CON-1, bid=[price=100.000000, size=10]");
    
    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test010_OrderClientIdDuplicate)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "5,200,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    pb::ems::Order order;
    order.set_client_order_id("test-1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100");
    order.set_quantity(10);

    trade_simulater.Add(order);
    
    pb::ems::Order order2;
    order2.set_client_order_id("test-1");
    order2.set_contract("CON-1");
    order2.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order2.set_order_type(pb::ems::OrderType::OT_Limit);
    order2.set_price("300");
    order2.set_quantity(30);
    
    trade_simulater.Add(order2);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 3);
    EXPECT_EQ(received_result[2].message(), (const char *)fh::tmalpha::trade::Message::CLIENT_ORDER_ID_EXIST);

    trade_simulater.Stop();
}

void addInitOrder(fh::tmalpha::trade::TradeSimulater &trade_simulater)
{
      {
      pb::ems::Order order;
      order.set_client_order_id("test-b1");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Buy);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("101");
      order.set_quantity(10);
      trade_simulater.Add(order);
      }

      {
      pb::ems::Order order;
      order.set_client_order_id("test-b2");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Buy);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("103");
      order.set_quantity(30);
      trade_simulater.Add(order);
      }

      {
      pb::ems::Order order;
      order.set_client_order_id("test-b3");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Buy);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("105");
      order.set_quantity(10);
      trade_simulater.Add(order);
      }

      {
      pb::ems::Order order;
      order.set_client_order_id("test-b4");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Buy);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("102");
      order.set_quantity(20);
      trade_simulater.Add(order);
      }

      {
      pb::ems::Order order;
      order.set_client_order_id("test-s1");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Sell);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("110");
      order.set_quantity(10);
      trade_simulater.Add(order);
      }

      {
      pb::ems::Order order;
      order.set_client_order_id("test-s2");
      order.set_contract("CON-1");
      order.set_buy_sell(pb::ems::BuySell::BS_Sell);
      order.set_order_type(pb::ems::OrderType::OT_Limit);
      order.set_price("106");
      order.set_quantity(10);
      trade_simulater.Add(order);
      }
}

TEST(TradeSimulaterTest, Test011_MultiOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 12);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();

    EXPECT_EQ(received_l2.size(), 6);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2[5]),
            "contract=CON-1, bid=[price=105.000000, size=10][price=103.000000, size=30][price=102.000000, size=20], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(received_bid.size(), 3);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bid[2]),
            "contract=CON-1, bid=[price=105.000000, size=10]");
    EXPECT_EQ(received_bbo.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo[1]),
            "contract=CON-1, bid=[price=105.000000, size=10]offer=[price=106.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test012_PartiallyTrade)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("104");
    order.set_quantity(18);
    trade_simulater.Add(order);

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 14);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 2);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(received_l2.size(), 8);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2[7]),
            "contract=CON-1, bid=[price=103.000000, size=30][price=102.000000, size=20][price=101.000000, size=10], offer=[price=104.000000, size=8][price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(received_bid.size(), 3);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bid[2]),
            "contract=CON-1, bid=[price=105.000000, size=10]");
    EXPECT_EQ(received_bbo.size(), 4);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo[3]),
            "contract=CON-1, bid=[price=103.000000, size=30]offer=[price=104.000000, size=8]");
    EXPECT_EQ(received_trade.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=105.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test013_MatchMulti)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("103");
    order.set_quantity(30);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 4);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 105);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[2].fill_price()), 103);
    EXPECT_EQ(received_fill[2].fill_quantity(), 20);
    EXPECT_EQ(std::stod(received_fill[3].fill_price()), 103);
    EXPECT_EQ(received_fill[3].fill_quantity(), 20);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=103.000000, size=10][price=102.000000, size=20][price=101.000000, size=10], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=103.000000, size=10]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=105.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[1]),
            "contract=CON-1, last=[price=103.000000, size=20]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test014_MultiOrderSamePrice)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b5");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("103");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("103");
    order.set_quantity(49);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 6);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 105);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[2].fill_price()), 103);
    EXPECT_EQ(received_fill[2].fill_quantity(), 30);
    EXPECT_EQ(std::stod(received_fill[3].fill_price()), 103);
    EXPECT_EQ(received_fill[3].fill_quantity(), 30);
    EXPECT_EQ(std::stod(received_fill[4].fill_price()), 103);
    EXPECT_EQ(received_fill[4].fill_quantity(), 9);
    EXPECT_EQ(std::stod(received_fill[5].fill_price()), 103);
    EXPECT_EQ(received_fill[5].fill_quantity(), 9);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=103.000000, size=16][price=102.000000, size=20][price=101.000000, size=10], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=103.000000, size=16]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 3);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=105.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[1]),
            "contract=CON-1, last=[price=103.000000, size=30]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[2]),
            "contract=CON-1, last=[price=103.000000, size=9]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test015_FokFill)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FOK);
    order.set_price("104");
    order.set_quantity(9);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 2);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 9);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 105);
    EXPECT_EQ(received_fill[1].fill_quantity(), 9);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=105.000000, size=1][price=103.000000, size=30][price=102.000000, size=20], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=105.000000, size=1]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=105.000000, size=9]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test016_FokKill)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FOK);
    order.set_price("104");
    order.set_quantity(19);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 0);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=105.000000, size=10][price=103.000000, size=30][price=102.000000, size=20], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=105.000000, size=10]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 0);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test017_FakKill)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FAK);
    order.set_price("106");
    order.set_quantity(19);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 0);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=105.000000, size=10][price=103.000000, size=30][price=102.000000, size=20], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=105.000000, size=10]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 0);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test018_FakFill)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FAK);
    order.set_price("104");
    order.set_quantity(19);
    trade_simulater.Add(order);

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 2);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 105);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 105);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=103.000000, size=30][price=102.000000, size=20][price=101.000000, size=10], offer=[price=106.000000, size=10][price=110.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bbo.back()),
            "contract=CON-1, bid=[price=103.000000, size=30]offer=[price=106.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=105.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test019_AddMarketOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Market);
    order.set_price("0.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Market);
    order.set_price("0");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 0);

    auto received_l2 = market_listener.L2s();
    EXPECT_EQ(received_l2.size(), 0);
    auto received_bid = market_listener.Bids();
    EXPECT_EQ(received_bid.size(), 0);
    auto received_bbo = market_listener.BBOs();
    EXPECT_EQ(received_bbo.size(), 0);
    auto received_trade = market_listener.Trades();
    EXPECT_EQ(received_trade.size(), 0);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test020_MatchMarketOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Market);
    order.set_price("0.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b2");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101.00");
    order.set_quantity(20);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 4);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 100);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 100);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[2].fill_price()), 101);
    EXPECT_EQ(received_fill[2].fill_quantity(), 15);
    EXPECT_EQ(std::stod(received_fill[3].fill_price()), 101);
    EXPECT_EQ(received_fill[3].fill_quantity(), 15);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=101.000000, size=5], offer=");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bid.back()),
            "contract=CON-1, bid=[price=101.000000, size=5]");
    EXPECT_EQ(received_trade.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=100.000000, size=10]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[1]),
            "contract=CON-1, last=[price=101.000000, size=15]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test021_MarketOrderMatch)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Market);
    order.set_price("0");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 2);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 101);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 101);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);

    auto received_l2 = market_listener.L2s();
    auto received_bid = market_listener.Bids();
    auto received_bbo = market_listener.BBOs();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=, offer=");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_bid.back()),
            "contract=CON-1, bid=[price=101.000000, size=10]");
    EXPECT_EQ(received_trade.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=101.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test022_ChangeFilledOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("102.00");
    order.set_quantity(20);
    trade_simulater.Change(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 5);
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result.back().message(), (const char *)fh::tmalpha::trade::Message::WORKING_ORDER_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test023_ChangeOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("102.00");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("103.00");
    order.set_quantity(15);
    trade_simulater.Change(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 7);
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(received_result.back().price()), 103);
    EXPECT_EQ(received_result.back().quantity(), 15);

    auto received_l2 = market_listener.L2s();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=101.000000, size=10], offer=[price=103.000000, size=15]");
    EXPECT_EQ(received_trade.size(), 0);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test024_ChangeOrderAndFill)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Buy);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("101.00");
    order.set_quantity(10);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("102.00");
    order.set_quantity(25);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100.00");
    order.set_quantity(15);
    trade_simulater.Change(order);
    }

    auto received_fill = exchange_listener.fills();
    EXPECT_EQ(received_fill.size(), 2);
    EXPECT_EQ(std::stod(received_fill[0].fill_price()), 101);
    EXPECT_EQ(received_fill[0].fill_quantity(), 10);
    EXPECT_EQ(std::stod(received_fill[1].fill_price()), 101);
    EXPECT_EQ(received_fill[1].fill_quantity(), 10);

    auto received_l2 = market_listener.L2s();
    auto received_trade = market_listener.Trades();

    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=, offer=[price=100.000000, size=5]");
    EXPECT_EQ(received_trade.size(), 1);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_trade[0]),
            "contract=CON-1, last=[price=101.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test025_DeleteContractNotExistOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-XX");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100.00");
    order.set_quantity(15);
    trade_simulater.Delete(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::CONTRACT_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test026_DeleteNotExistOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100.00");
    order.set_quantity(15);
    trade_simulater.Delete(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test027_DeleteOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b3");
    order.set_contract("CON-1");
    trade_simulater.Delete(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Cancelled);
    EXPECT_EQ(received_result.back().client_order_id(), "test-b3");
    EXPECT_EQ(received_result.back().quantity(), 10);
    EXPECT_EQ(std::stod(received_result.back().price()), 105);

    auto received_l2 = market_listener.L2s();
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(received_l2.back()),
            "contract=CON-1, bid=[price=103.000000, size=30][price=102.000000, size=20][price=101.000000, size=10], offer=[price=106.000000, size=10][price=110.000000, size=10]");

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test028_DeleteCancelledOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b3");
    order.set_contract("CON-1");
    trade_simulater.Delete(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b3");
    order.set_contract("CON-1");
    trade_simulater.Delete(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result.back().message(), (const char *)fh::tmalpha::trade::Message::ORDER_ALREADY_CANCELLED);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test029_QueryContractNotExistOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-XX");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("100.00");
    order.set_quantity(15);
    trade_simulater.Query(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::CONTRACT_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test030_QueryNotExistOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s1");
    order.set_contract("CON-1");
    trade_simulater.Query(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.size(), 1);
    EXPECT_EQ(received_result[0].status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(received_result[0].message(), (const char *)fh::tmalpha::trade::Message::ORDER_NOT_FOUND);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test031_QueryFilledOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FAK);
    order.set_price("104");
    order.set_quantity(19);
    trade_simulater.Add(order);
    }

    {
    pb::ems::Order order;
    order.set_client_order_id("test-b3");
    order.set_contract("CON-1");
    trade_simulater.Query(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Filled);
    EXPECT_EQ(received_result.back().client_order_id(), "test-b3");
    EXPECT_EQ(received_result.back().filled_quantity(), 10);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test032_QueryCancelledOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_tif(pb::ems::TimeInForce::TIF_FAK);
    order.set_price("104");
    order.set_quantity(19);
    trade_simulater.Add(order);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    trade_simulater.Query(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Cancelled);
    EXPECT_EQ(received_result.back().client_order_id(), "test-s3");
    EXPECT_EQ(received_result.back().quantity(), 9);
    EXPECT_EQ(std::stod(received_result.back().price()), 104);
    EXPECT_EQ(received_result.back().filled_quantity(), 10);

    trade_simulater.Stop();
}

TEST(TradeSimulaterTest, Test033_QueryWorkingOrder)
{
    std::unordered_map<std::string, std::string> contracts;
    contracts["CON-1"] = "3,100,1000,1000000";

    fh::tmalpha::trade::MockTradeMarketListener market_listener;
    fh::tmalpha::trade::MockTradeExchangeListener exchange_listener;
    fh::tmalpha::trade::TradeAlgorithmSimple algorithm;

    fh::tmalpha::trade::TradeSimulater trade_simulater;
    trade_simulater.Set_market_listener(&market_listener);
    trade_simulater.Set_exchange_listener(&exchange_listener);
    trade_simulater.Load_contracts(contracts);
    trade_simulater.Load_match_algorithm(&algorithm);
    trade_simulater.Start();

    addInitOrder(trade_simulater);

    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    order.set_buy_sell(pb::ems::BuySell::BS_Sell);
    order.set_order_type(pb::ems::OrderType::OT_Limit);
    order.set_price("104");
    order.set_quantity(19);
    trade_simulater.Add(order);

    {
    pb::ems::Order order;
    order.set_client_order_id("test-s3");
    order.set_contract("CON-1");
    trade_simulater.Query(order);
    }

    auto received_result = exchange_listener.orders();
    EXPECT_EQ(received_result.back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(received_result.back().client_order_id(), "test-s3");
    EXPECT_EQ(received_result.back().quantity(), 9);
    EXPECT_EQ(std::stod(received_result.back().price()), 104);
    EXPECT_EQ(received_result.back().filled_quantity(), 10);

    trade_simulater.Stop();
}
