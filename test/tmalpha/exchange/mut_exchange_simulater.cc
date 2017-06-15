
#include <unordered_map>
#include <gtest/gtest.h>
#include "pb/dms/dms.pb.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/utility.h"
#include "tmalpha/exchange/mock_exchange_listener.h"
#include "tmalpha/exchange/exchange_simulater.h"


pb::ems::Order make_order(const std::string &cid, const std::string &contract, double price, bool is_buy, bool is_limit)
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

TEST(ExchangeSimulaterTest, Test001_Market)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 0, true, false));

    // 此时新加的订单没有对方报价，不能匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    exchange_simulater->Add(make_order("C2", "T1", 0, false, false));

    // 此时 market 订单成交
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-2");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-1");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test002_NoContract)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T9", 100, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test003_Working)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 21.00, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);
    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-1");

    exchange_simulater->Add(make_order("C2", "T2", 103, false, true));

    // 此时两个订单都没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);
    EXPECT_EQ(result_listener->orders().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->orders().back().status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().back().exchange_order_id(), "Order-2");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test004_Fill)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 11.11000, false, true));

    // 此时该订单成交
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 1);

    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C1");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-1");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-1");

    exchange_simulater->Add(make_order("C2", "T2", 203.3, true, true));

    // 此时第二个订单也成交
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 2);

    EXPECT_EQ(result_listener->fills().back().client_order_id(), "C2");
    EXPECT_EQ(result_listener->fills().back().exchange_order_id(), "Order-2");
    EXPECT_EQ(result_listener->fills().back().fill_id(), "Fill-2");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test005_RematchNone)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(800);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    exchange_simulater->On_state_changed(t3);

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 20.20, false, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C2", "T3", 800, true, true));

    // 此时订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->On_state_changed(t1);
    exchange_simulater->On_state_changed(t2);
    exchange_simulater->On_state_changed(t3);

    // 此时订单还是没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test006_Rematch)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(102.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(800);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    exchange_simulater->On_state_changed(t3);
    }

    // 此时还没有订单存在
    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 13.2, false, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C2", "T2", 200, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C3", "T3", 900, true, true));

    // 此时新加的订单没有匹配
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 0);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(14.20);  bid->set_size(7); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(200.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(203.35);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(880);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.00);  ask->set_size(123); }

    exchange_simulater->On_state_changed(t3);

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

    exchange_simulater->On_state_changed(t1);
    exchange_simulater->On_state_changed(t2);
    exchange_simulater->On_state_changed(t3);

    // 此时没有新的订单匹配发生
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 2);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test007_ChangeOrder)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    exchange_simulater->Change(make_order("C1", "T1", 11.2, false, true));

    // 订单不存在，无法修改
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    exchange_simulater->Add(make_order("C1", "T1", 11.2, false, true));

    // 添加新处理中订单
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 11.2);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1");

    exchange_simulater->Change(make_order("C1", "T1-2", 99.09, true, true));

    // 订单修改成功
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 99.09);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1-2");

    pb::dms::L2 t1;
    t1.set_contract("T1-2");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(99.09);  ask->set_size(1); }

    exchange_simulater->On_state_changed(t1);

    // 修改过的订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 99.09);

    exchange_simulater->Change(make_order("C1", "T1-3", 33.3, false, true));

    // 订单已被匹配，无法修改
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is filled");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test008_DeleteOrder)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    exchange_simulater->Delete(make_order("C1", "T1", 11.2, true, true));

    // 订单不存在，无法删除
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(11.20);  ask->set_size(1); }

    exchange_simulater->On_state_changed(t1);

    exchange_simulater->Add(make_order("C1", "T1", 11.2, true, true));

    // 订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 11.20);

    exchange_simulater->Delete(make_order("C1", "T1", 11.2, false, true));

    // 订单已成交，无法删除
    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is filled");

    exchange_simulater->Add(make_order("C2", "T1", 11.2, false, true));

    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(result_listener->orders().size(), 3);

    exchange_simulater->Delete(make_order("C2", "T1", 11.2, false, true));

    // 订单删除成功
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    exchange_simulater->Delete(make_order("C2", "T1", 11.2, false, true));

    // 订单已经被删除，无法再次删除
    EXPECT_EQ(result_listener->orders().size(), 5);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is canceled");

    exchange_simulater->Change(make_order("C2", "T1", 33.3, false, true));

    // 订单已经被删除，无法修改
    EXPECT_EQ(result_listener->orders().size(), 6);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order is canceled");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test009_QueryOrder)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    exchange_simulater->Query(make_order("C1", "T1", 11.2, true, true));

    // 订单不存在
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Rejected);
    EXPECT_EQ(result_listener->orders().back().message(), "order not found");

    exchange_simulater->Add(make_order("C1", "T1", 61.00, false, true));

    // 新加处理中订单
    EXPECT_EQ(result_listener->orders().size(), 2);

    exchange_simulater->Query(make_order("C1", "T1", 61.00, false, true));

    // 订单在处理中
    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 61);
    EXPECT_EQ(result_listener->orders().back().contract(), "T1");

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(61.00);  bid->set_size(100); }

    exchange_simulater->On_state_changed(t1);

    // 订单匹配成功
    EXPECT_EQ(result_listener->fills().size(), 1);
    EXPECT_EQ(std::stod(result_listener->fills().back().fill_price()), 61);

    exchange_simulater->Query(make_order("C1", "T1", 61.00, false, true));

    // 订单已成交
    EXPECT_EQ(result_listener->orders().size(), 4);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Filled);
    EXPECT_EQ(std::stod(result_listener->orders().back().price()), 61);
    EXPECT_EQ(result_listener->orders().back().filled_quantity(), 20);

    exchange_simulater->Add(make_order("C2", "T1", 110.2, false, true));

    // 添加新处理中订单
    EXPECT_EQ(result_listener->orders().size(), 5);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Working);

    exchange_simulater->Delete(make_order("C2", "T1", 110.2, false, true));

    // 订单删除成功
    EXPECT_EQ(result_listener->orders().size(), 6);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    exchange_simulater->Query(make_order("C2", "T1", 110.20, false, true));

    // 订单已删除
    EXPECT_EQ(result_listener->orders().size(), 7);
    EXPECT_EQ(result_listener->orders().back().status(), pb::ems::OrderStatus::OS_Cancelled);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test010_NoPricesOnTradeRate30)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 5, 10);

    exchange_simulater->Add(make_order("C1", "T1", 13.0, false, true));

    exchange_simulater->On_state_changed(t1, 5, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test011_PriceNotBestOnTradeRate30)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 50, 10);

    exchange_simulater->Add(make_order("C1", "T1", 11.2, true, true));

    exchange_simulater->On_state_changed(t1, 50, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test012_OnTradeRate1000)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 1000);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 5, 10);

    exchange_simulater->Add(make_order("C1", "T1", 12.01, true, true));

    exchange_simulater->On_state_changed(t1, 5, 10);

    // 此时该订单不能成交（已成交数量 5 < 订单位置 10 * 100%）
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test013_NotReachOnTradeRate20)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 20);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 5, 10);

    exchange_simulater->Add(make_order("C1", "T1", 12.01, true, true));  // 计算出订单位置：position = size1 * 20% = 4

    exchange_simulater->On_state_changed(t1, 4, 10);    // size2=4

    // 此时该订单不能成交: size2 = 4 不大于 position = 4
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test014_ReachedOnTradeRate20)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 20);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 25, 10);

    exchange_simulater->Add(make_order("C1", "T1", 12.01, true, true)); // 计算出订单位置：position = size1 * 20% = 4

    // 此时不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);

    exchange_simulater->On_state_changed(t1, 5, 10);    // size2=5

    // 此时该订单能成交: size2 = 5 大于 position = 4
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test015_NoTurnoverOnTradeRate10)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 10);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 0, 10);

    exchange_simulater->Add(make_order("C1", "T1", 12.01, true, true)); // 计算出订单位置：position = size1 * 10% = 2

    exchange_simulater->On_state_changed(t1, 0, 10);    // size2=0

    // 此时该订单不能成交: size2 = 0 不大于 position = 2
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test016_NoTurnoverOnTradeRate0)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 0);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.21);  bid->set_size(12); }

    exchange_simulater->On_state_changed(t1, 0, 10);

    exchange_simulater->Add(make_order("C1", "T1", 12.01, true, true));  // 计算出订单位置：position = size1 * 0% = 0

    exchange_simulater->On_state_changed(t1, 0, 10);    // size2=0

    // 此时该订单不能成交: size2 = 0 不大于 position = 0
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test017_PriceNotBestOnTradeRateOfSell)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1, 50, 10);

    exchange_simulater->Add(make_order("C1", "T1", 21.2, false, true));

    exchange_simulater->On_state_changed(t1, 50, 10);

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test018_ReachedOnTradeRateOfSell)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }    // size1=20
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1, 2, 10);

    exchange_simulater->Add(make_order("C1", "T1", 20.10, false, true)); // 计算出订单位置：position = size1 * 30% = 6

    // 此时不能成交
    EXPECT_EQ(result_listener->orders().size(), 1);

    exchange_simulater->On_state_changed(t1, 2, 10);   // size2=10

    // 此时该订单能成交: size2 = 10 大于 position = 6
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test019_BetterThanBestPriceOnRematch)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 2, 10);
    }

    exchange_simulater->Add(make_order("C1", "T1", 12.10, true, true)); // 和最优价不一样，无法计算出订单位置

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 1, 10);    // 最优价上有成交：1
    }

    // 此时该订单能成交: 订单价格超过最优价
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test020_AccumulateTurnover)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 2, 10);
    }

    exchange_simulater->Add(make_order("C1", "T1", 12.00, true, true)); // 订单价格等于最优价低，计算出订单位置：position = size1 * 30% = 6

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 1, 10);    // 订单价格比最优价低，不累加已成交数量
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); } // size1=80
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 3, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 3
    }

    // 此时该订单不能成交：size2 = 3 < position = 6
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 1, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 3 + 1 = 4
    }

    // 此时该订单不能成交：size2 = 5 < position = 6
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.99);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 1, 10);    // 订单价格优于最优价
    }

    // 此时该订单能成交：订单价格优于最优价
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test021_AccumulateToMatchPosition)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener, 30);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); } // size1=20
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 2, 10);
    }

    exchange_simulater->Add(make_order("C1", "T1", 12.00, true, true)); // 和最优价不一样，无法计算出订单位置

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.01);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 1, 10);    // 订单价格比最优价低，无法计算出订单位置
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); } // size1=80
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 100, 10);    // 订单价格等于最优价，计算出订单位置：position = size1 * 30% = 24
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.10);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 100, 10);    // 订单价格比最优价低，不累加已成交数量
    }

    // 此时该订单不能成交
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(80); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 12, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 12
    }

    // 此时该订单不能成交：size2 = 12 < position = 24
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 8, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 12 + 8 = 20
    }

    // 此时该订单不能成交：size2 = 20 < position = 24
    EXPECT_EQ(result_listener->fills().size(), 0);

    {
    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.00);  bid->set_size(20); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }

    exchange_simulater->On_state_changed(t1, 5, 10);    // 订单价格等于最优价，累加已成交数量：size2 = 20 + 5 = 25
    }

    // 此时该订单能成交: size2 = 25 > position = 24
    EXPECT_EQ(result_listener->fills().size(), 1);

    delete exchange_simulater;
    delete result_listener;
}

