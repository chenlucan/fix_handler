
#include <unordered_map>
#include <gtest/gtest.h>
#include "pb/dms/dms.pb.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/utility.h"
#include "tmalpha/replay/mock_replay_data_provider.h"
#include "tmalpha/replay/mock_replay_exchange_listener.h"
#include "tmalpha/replay/mock_replay_market_listener.h"
#include "tmalpha/replay/replay_simulater.h"


pb::ems::Order make_test_order(const std::string &cid, const std::string &contract, double price, bool is_buy, bool is_limit)
{
    pb::ems::Order order;
    order.set_client_order_id(cid);
    order.set_account("YYC");
    order.set_contract(contract);
    order.set_buy_sell(is_buy ? pb::ems::BuySell::BS_Buy : pb::ems::BuySell::BS_Sell);
    order.set_price(std::to_string(price));
    order.set_quantity(20);
    order.set_tif(pb::ems::TimeInForce::TIF_GFD);
    order.set_order_type(is_limit ? pb::ems::OrderType::OT_Limit : pb::ems::OrderType::OT_Market);

    return order;
}

TEST(ReplaySimulaterTest, Test001_Market)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T1", 0, true, false));

    // 此时新加的订单没有对方报价，不能匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    replay_simulater->Add(make_test_order("C2", "T1", 0, false, false));

    // 此时 market 订单成交
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-2");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-1");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test002_NoContract)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T9", 100, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test003_Working)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    replay_simulater->On_state_changed(t2);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T1", 21.00, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);
    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    replay_simulater->Add(make_test_order("C2", "T2", 103, false, true));

    // 此时两个订单都没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);
    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-2");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test004_Fill)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    replay_simulater->On_state_changed(t2);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T1", 11.11000, false, true));

    // 此时该订单成交
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 1);

    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-1");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-1");

    replay_simulater->Add(make_test_order("C2", "T2", 203.3, true, true));

    // 此时第二个订单也成交
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 2);

    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-2");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-2");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test005_RematchNone)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    replay_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(800);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    replay_simulater->On_state_changed(t3);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T1", 20.20, false, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C2", "T3", 800, true, true));

    // 此时订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->On_state_changed(t1);
    replay_simulater->On_state_changed(t2);
    replay_simulater->On_state_changed(t3);

    // 此时订单还是没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test006_Rematch)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    replay_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(800);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    replay_simulater->On_state_changed(t3);
    }

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C1", "T1", 13.2, false, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C2", "T2", 200, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    replay_simulater->Add(make_test_order("C3", "T3", 900, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 0);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(14.20);  bid->set_size(7); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(200.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(203.35);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    replay_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(880);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.00);  ask->set_size(123); }

    replay_simulater->On_state_changed(t3);

    // 此时有两个订单产生匹配
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 2);

    EXPECT_EQ(result_listener->fills().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->fills().at(0).exchange_order_id(), "Order-1");
    EXPECT_EQ(result_listener->fills().at(0).fill_id(), "Fill-1");
    EXPECT_EQ(std::stod(result_listener->fills().at(0).fill_price()), 14.2);
    EXPECT_EQ(result_listener->fills().at(0).fill_quantity(), 20);

    EXPECT_EQ(result_listener->fills().at(1).client_order_id(), "C3");
    EXPECT_EQ(result_listener->fills().at(1).exchange_order_id(), "Order-3");
    EXPECT_EQ(result_listener->fills().at(1).fill_id(), "Fill-2");
    EXPECT_EQ(std::stod(result_listener->fills().at(1).fill_price()), 900);
    EXPECT_EQ(result_listener->fills().at(1).fill_quantity(), 20);

    replay_simulater->On_state_changed(t1);
    replay_simulater->On_state_changed(t2);
    replay_simulater->On_state_changed(t3);

    // 此时没有新的订单匹配发生
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 2);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test007_ChangeOrder)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    replay_simulater->Change(make_test_order("C1", "T1", 11.2, false, true));

    // 订单不存在，无法修改
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    replay_simulater->Add(make_test_order("C1", "T1", 11.2, false, true));

    // 添加新处理中订单
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 11.2);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1");

    replay_simulater->Change(make_test_order("C1", "T1-2", 99.09, true, true));

    // 订单修改成功
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 99.09);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1-2");

    pb::dms::L2 t1;
    t1.set_contract("T1-2");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(99.09);  ask->set_size(1); }

    replay_simulater->On_state_changed(t1);

    // 修改过的订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 99.09);

    replay_simulater->Change(make_test_order("C1", "T1-3", 33.3, false, true));

    // 订单已被匹配，无法修改
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is filled");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test008_DeleteOrder)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    replay_simulater->Delete(make_test_order("C1", "T1", 11.2, true, true));

    // 订单不存在，无法删除
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(11.20);  ask->set_size(1); }

    replay_simulater->On_state_changed(t1);

    replay_simulater->Add(make_test_order("C1", "T1", 11.2, true, true));

    // 订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 11.20);

    replay_simulater->Delete(make_test_order("C1", "T1", 11.2, false, true));

    // 订单已成交，无法删除
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is filled");

    replay_simulater->Add(make_test_order("C2", "T1", 11.2, false, true));

    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(result_listener->orders().size(), 3);

    replay_simulater->Delete(make_test_order("C2", "T1", 11.2, false, true));

    // 订单删除成功
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    replay_simulater->Delete(make_test_order("C2", "T1", 11.2, false, true));

    // 订单已经被删除，无法再次删除
    EXPECT_EQ(result_listener->orders().size(), 5);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is canceled");

    replay_simulater->Change(make_test_order("C2", "T1", 33.3, false, true));

    // 订单已经被删除，无法修改
    EXPECT_EQ(result_listener->orders().size(), 6);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is canceled");

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test009_QueryOrder)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr);
    replay_simulater->Set_exchange_listener(result_listener);

    replay_simulater->Query(make_test_order("C1", "T1", 11.2, true, true));

    // 订单不存在
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    replay_simulater->Add(make_test_order("C1", "T1", 61.00, false, true));

    // 新加处理中订单
    EXPECT_EQ(result_listener->orders().size(), 2);

    replay_simulater->Query(make_test_order("C1", "T1", 61.00, false, true));

    // 订单在处理中
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 61);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1");

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(61.00);  bid->set_size(100); }

    replay_simulater->On_state_changed(t1);

    // 订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 61);

    replay_simulater->Query(make_test_order("C1", "T1", 61.00, false, true));

    // 订单已成交
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Filled);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 61);
    EXPECT_EQ(result_listener->orders().back().filled_quantity(), 20);

    replay_simulater->Add(make_test_order("C2", "T1", 110.2, false, true));

    // 添加新处理中订单
    EXPECT_EQ(result_listener->orders().size(), 5);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);

    replay_simulater->Delete(make_test_order("C2", "T1", 110.2, false, true));

    // 订单删除成功
    EXPECT_EQ(result_listener->orders().size(), 6);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    replay_simulater->Query(make_test_order("C2", "T1", 110.20, false, true));

    // 订单已删除
    EXPECT_EQ(result_listener->orders().size(), 7);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test010_NoPricesOnTradeRate30)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 5, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 13.0, false, true));

    replay_simulater->On_state_changed(t1, 5, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test011_PriceNotBestOnTradeRate30)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 50, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 11.2, true, true));

    replay_simulater->On_state_changed(t1, 50, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test012_OnTradeRate1000)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 1000);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 5, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 12.01, true, true));

    replay_simulater->On_state_changed(t1, 5, 10);

    // 此时该订单不能成交（已成交数量 5 < 订单位置 10 * 100%）
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test013_NotReachOnTradeRate20)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 20);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 5, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 12.01, true, true));  // 计算出订单位置：position = size1 * 20% = 4

    replay_simulater->On_state_changed(t1, 4, 10);    // size2=4

    // 此时该订单不能成交: size2 = 4 不大于 position = 4
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test014_ReachedOnTradeRate20)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 20);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 25, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 12.01, true, true)); // 计算出订单位置：position = size1 * 20% = 4

    // 此时不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);

    replay_simulater->On_state_changed(t1, 5, 10);    // size2=5

    // 此时该订单能成交: size2 = 5 大于 position = 4
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test015_NoTurnoverOnTradeRate10)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 10);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 0, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 12.01, true, true)); // 计算出订单位置：position = size1 * 10% = 2

    replay_simulater->On_state_changed(t1, 0, 10);    // size2=0

    // 此时该订单不能成交: size2 = 0 不大于 position = 2
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test016_NoTurnoverOnTradeRate0)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 0);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    replay_simulater->On_state_changed(t1, 0, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 12.01, true, true));  // 计算出订单位置：position = size1 * 0% = 0

    replay_simulater->On_state_changed(t1, 0, 10);    // size2=0

    // 此时该订单不能成交: size2 = 0 不大于 position = 0
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test017_PriceNotBestOnTradeRateOfSell)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1, 50, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 21.2, false, true));

    replay_simulater->On_state_changed(t1, 50, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test018_ReachedOnTradeRateOfSell)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }    // size1=20
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    replay_simulater->On_state_changed(t1, 2, 10);

    replay_simulater->Add(make_test_order("C1", "T1", 20.10, false, true)); // 计算出订单位置：position = size1 * 30% = 6

    // 此时不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);

    replay_simulater->On_state_changed(t1, 2, 10);   // size2=10

    // 此时该订单能成交: size2 = 10 大于 position = 6
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test019_BetterThanBestPriceOnRematch)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 2, 10);
    }

    replay_simulater->Add(make_test_order("C1", "T1", 12.10, true, true)); // 和最优价不一样，无法计算出订单位置

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 1, 10);    // 最优价上有成交：1
    }

    // 此时该订单能成交: 订单价格超过最优价
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test020_AccumulateTurnover)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 2, 10);
    }

    replay_simulater->Add(make_test_order("C1", "T1", 12.00, true, true)); // 订单价格等于最优价低，计算出订单位置：position = size1 * 30% = 6

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 1, 10);    // 订单价格比最优价低，不累加已成交数量
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); } // size1=80
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 3, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 3
    }

    // 此时该订单不能成交：size2 = 3 < position = 6
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 1, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 3 + 1 = 4
    }

    // 此时该订单不能成交：size2 = 5 < position = 6
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.99);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 1, 10);    // 订单价格优于最优价
    }

    // 此时该订单能成交：订单价格优于最优价
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete replay_simulater;
    delete result_listener;
}

TEST(ReplaySimulaterTest, Test021_AccumulateToMatchPosition)
{
    fh::tmalpha::replay::MockReplayExchangeListener *result_listener = new fh::tmalpha::replay::MockReplayExchangeListener();;
    fh::tmalpha::replay::ReplaySimulater *replay_simulater = new fh::tmalpha::replay::ReplaySimulater(nullptr, 30);
    replay_simulater->Set_exchange_listener(result_listener);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 2, 10);
    }

    replay_simulater->Add(make_test_order("C1", "T1", 12.00, true, true)); // 和最优价不一样，无法计算出订单位置

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 1, 10);    // 订单价格比最优价低，无法计算出订单位置
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); } // size1=80
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 100, 10);    // 订单价格等于最优价，计算出订单位置：position = size1 * 30% = 24
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.10);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 100, 10);    // 订单价格比最优价低，不累加已成交数量
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 12, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 12
    }

    // 此时该订单不能成交：size2 = 12 < position = 24
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 8, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 12 + 8 = 20
    }

    // 此时该订单不能成交：size2 = 20 < position = 24
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    replay_simulater->On_state_changed(t1, 5, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 20 + 5 = 25
    }

    // 此时该订单能成交: size2 = 25 > position = 24
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete replay_simulater;
    delete result_listener;
}

std::vector<std::string> messages {
    "{ \"insertTime\" : \"1493010903830000000\", \"sendingTime\" : \"1493010000000000000\", \"market\" : \"FEMAS\", \"type\" : \"trade\", \"message\" : { \"contract\" : \"con-2\", \"last\" : { \"price\" : \"110.02\", \"size\" : \"3\" }}}",
    "{ \"insertTime\" : \"1493010903831000000\", \"sendingTime\" : \"1493010005000000000\", \"market\" : \"FEMAS\", \"type\" : \"contract\", \"message\" : { \"name\" : \"con-1\", \"tick_size\" : \"2\", \"tick_value\" : \"1.36\", \"yesterday_close_price\" : \"123.45\", \"upper_limit\" : \"999.88\", \"lower_limit\" : \"0.05\"}}",
    "{ \"insertTime\" : \"1493010903832000000\", \"sendingTime\" : \"1493010010000000000\", \"market\" : \"FEMAS\", \"type\" : \"bbo\", \"message\" : { \"contract\" : \"con-1\", \"bid\" : { \"price\" : \"100.00\", \"size\" : \"10\" }, \"offer\" : { \"price\" : \"20000.00\", \"size\" : \"2000\" }}}",
    "{ \"insertTime\" : \"1493010903833000000\", \"sendingTime\" : \"1493010012000000000\", \"market\" : \"FEMAS\", \"type\" : \"offer\", \"message\" : { \"contract\" : \"con-1\", \"offer\" : { \"price\" : \"1.0\", \"size\" : \"999\" }}}",
    "{ \"insertTime\" : \"1493010903834000000\", \"sendingTime\" : \"1493010022000000000\", \"market\" : \"FEMAS\", \"type\" : \"l2\", \"message\" : { \"contract\" : \"con-1\", \"bid_turnover\" : \"7\", \"offer_turnover\" : \"9\", \"bid\" : [{ \"price\" : \"100.00\", \"size\" : \"10\" }, { \"price\" : \"200.00\", \"size\" : \"20\" }], \"offer\" : [{ \"price\" : \"1\", \"size\" : \"2\" }]}}",
    "{ \"insertTime\" : \"1493010903835000000\", \"sendingTime\" : \"1493010023000000000\", \"market\" : \"FEMAS\", \"type\" : \"trade\", \"message\" : { \"contract\" : \"con-1\", \"last\" : { \"price\" : \"100.00\", \"size\" : \"10\" }}}",
    "{ \"insertTime\" : \"1493010903836000000\", \"sendingTime\" : \"1493010033000000000\", \"market\" : \"FEMAS\", \"type\" : \"bbo\", \"message\" : { \"contract\" : \"con-1\", \"bid\" : { \"price\" : \"20.20\", \"size\" : \"2\" }, \"offer\" : { \"price\" : \"200.6\", \"size\" : \"88\" }}}",
    "{ \"insertTime\" : \"1493010903837000000\", \"sendingTime\" : \"1493010035000000000\", \"market\" : \"FEMAS\", \"type\" : \"bid\", \"message\" : { \"contract\" : \"con-1\", \"bid\" : { \"price\" : \"0.20\", \"size\" : \"12\" }}}",
    "{ \"insertTime\" : \"1493010903838000000\", \"sendingTime\" : \"1493010040000000000\", \"market\" : \"FEMAS\", \"type\" : \"trade\", \"message\" : { \"contract\" : \"con-3\", \"last\" : { \"price\" : \"120\", \"size\" : \"61\" }}}",
    "{ \"insertTime\" : \"1493010903839000000\", \"sendingTime\" : \"1493010042000000000\", \"market\" : \"FEMAS\", \"type\" : \"bid\", \"message\" : { \"contract\" : \"con-1\", \"bid\" : { \"price\" : \"10.2\", \"size\" : \"2\" }}}",
    "{ \"insertTime\" : \"1493010903840000000\", \"sendingTime\" : \"1493010045000000000\", \"market\" : \"FEMAS\", \"type\" : \"offer\", \"message\" : { \"contract\" : \"con-1\", \"offer\" : { \"price\" : \"200\", \"size\" : \"88\" }}}",
    "{ \"insertTime\" : \"1493010903841000000\", \"sendingTime\" : \"1493010055000000000\", \"market\" : \"FEMAS\", \"type\" : \"l2\", \"message\" : { \"contract\" : \"con-1\", \"bid_turnover\" : \"7\", \"offer_turnover\" : \"9\", \"bid\" : [], \"offer\" : []}}",
    "{ \"insertTime\" : \"1493010903842000000\", \"sendingTime\" : \"1493010060000000000\", \"market\" : \"FEMAS\", \"type\" : \"l2\", \"message\" : { \"contract\" : \"con-1\", \"bid_turnover\" : \"7\", \"offer_turnover\" : \"9\", \"bid\" : [{ \"price\" : \"100.00\", \"size\" : \"10\" }, { \"price\" : \"200.00\", \"size\" : \"20\" }, { \"price\" : \"300.00\", \"size\" : \"30\" }], \"offer\" : [{ \"price\" : \"1\", \"size\" : \"2\" }, { \"price\" : \"9.0\", \"size\" : \"100\" }]}}",
    "{ \"insertTime\" : \"1493010903843000000\", \"sendingTime\" : \"1493010063000000000\", \"market\" : \"FEMAS\", \"type\" : \"contract\", \"message\" : { \"name\" : \"con-2\", \"tick_size\" : \"124\", \"tick_value\" : \"777.0\", \"yesterday_close_price\" : \"678\", \"upper_limit\" : \"100000\", \"lower_limit\" : \"1\"}}",
    "{ \"insertTime\" : \"1493010903844000000\", \"sendingTime\" : \"1493010070000000000\", \"market\" : \"FEMAS\", \"type\" : \"l2\", \"message\" : { \"contract\" : \"con-2\", \"bid_turnover\" : \"7\", \"offer_turnover\" : \"9\", \"bid\" : [], \"offer\" : [{ \"price\" : \"11.1\", \"size\" : \"3\" }]}}",
};

TEST(ReplaySimulaterTest, Test022_ReadOnce)
{
    std::uint32_t page_size = 15;
    fh::tmalpha::replay::MockReplayDataProvider *provider = new fh::tmalpha::replay::MockReplayDataProvider(messages, page_size);
    fh::tmalpha::replay::MockReplayMarketListener *listener = new fh::tmalpha::replay::MockReplayMarketListener();

    fh::tmalpha::replay::ReplaySimulater *simulater = new fh::tmalpha::replay::ReplaySimulater(provider);
    simulater->Set_market_listener(listener);

    simulater->Start();
    simulater->Join();

    auto cs = listener->Contracts();

    EXPECT_EQ(cs.size(), 2);

    auto l2s = listener->L2s();
    int size = l2s.size();

    EXPECT_EQ(size, 4);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(l2s[size - 2]), "contract=con-1, bid=[price=100.000000, size=10][price=200.000000, size=20][price=300.000000, size=30], offer=[price=1.000000, size=2][price=9.000000, size=100]");
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(l2s[size - 1]), "contract=con-2, bid=, offer=[price=11.100000, size=3]");

    delete simulater;
    delete listener;
    delete provider;
}

TEST(ReplaySimulaterTest, Test023_ReadMultiple)
{
    std::uint32_t page_size = 5;
    fh::tmalpha::replay::MockReplayDataProvider *provider = new fh::tmalpha::replay::MockReplayDataProvider(messages, page_size);
    fh::tmalpha::replay::MockReplayMarketListener *listener = new fh::tmalpha::replay::MockReplayMarketListener();

    fh::tmalpha::replay::ReplaySimulater *simulater = new fh::tmalpha::replay::ReplaySimulater(provider);
    simulater->Set_market_listener(listener);

    simulater->Start();
    simulater->Join();

    auto trades = listener->Trades();

    EXPECT_EQ(trades.size(), 3);
    EXPECT_EQ(trades[0].contract(), "con-2");
    EXPECT_EQ(trades[0].last().price(), 110.02);
    EXPECT_EQ(trades[0].last().size(), 3);

    delete simulater;
    delete listener;
    delete provider;
}

TEST(ReplaySimulaterTest, Test024_3xSpeed)
{
    std::uint32_t page_size = 15;
    fh::tmalpha::replay::MockReplayDataProvider *provider = new fh::tmalpha::replay::MockReplayDataProvider(messages, page_size);
    fh::tmalpha::replay::MockReplayMarketListener *listener = new fh::tmalpha::replay::MockReplayMarketListener();

    fh::tmalpha::replay::ReplaySimulater *simulater = new fh::tmalpha::replay::ReplaySimulater(provider);
    simulater->Set_market_listener(listener);

    simulater->Speed(3);
    simulater->Start();
    simulater->Join();

    auto bids = listener->Bids();

    EXPECT_EQ(bids.size(), 2);
    EXPECT_EQ(fh::core::assist::utility::Format_pb_message(bids[1]), "contract=con-1, bid=[price=10.200000, size=2]");

    delete simulater;
    delete listener;
    delete provider;
}
