
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
#if 0
TEST(ExchangeSimulaterTest, Test001_Market)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 10, false, false));

    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().at(0).status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().at(0).exchange_order_id(), "Order-1");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test002_NoContract)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T9", 100, true, true));

    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().at(0).status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().at(0).exchange_order_id(), "Order-1");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test003_Working)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(100.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 21.00, true, true));

    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->orders().at(0).status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().at(0).exchange_order_id(), "Order-1");

    exchange_simulater->Add(make_order("C2", "T2", 100, false, true));

    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    EXPECT_EQ(result_listener->orders().at(1).client_order_id(), "C2");
    EXPECT_EQ(result_listener->orders().at(1).status(), ::pb::ems::OrderStatus::OS_Working);
    EXPECT_EQ(result_listener->orders().at(1).exchange_order_id(), "Order-2");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test004_Fill)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(100.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 11.11000, false, true));

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 1);

    EXPECT_EQ(result_listener->fills().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->fills().at(0).exchange_order_id(), "Order-1");
    EXPECT_EQ(result_listener->fills().at(0).fill_id(), "Fill-1");

    exchange_simulater->Add(make_order("C2", "T2", 203.3, true, true));

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 2);

    EXPECT_EQ(result_listener->fills().at(1).client_order_id(), "C2");
    EXPECT_EQ(result_listener->fills().at(1).exchange_order_id(), "Order-2");
    EXPECT_EQ(result_listener->fills().at(1).fill_id(), "Fill-2");

    delete exchange_simulater;
    delete result_listener;
}

TEST(ExchangeSimulaterTest, Test005_RematchNone)
{
    fh::tmalpha::exchange::MockExchangeListener *result_listener = new fh::tmalpha::exchange::MockExchangeListener();
    fh::tmalpha::exchange::ExchangeSimulater *exchange_simulater = new fh::tmalpha::exchange::ExchangeSimulater(nullptr, result_listener);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(100.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(1000);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    exchange_simulater->On_state_changed(t3);

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 11.2, false, true));

    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C2", "T3", 900.20, true, true));

    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->On_state_changed(t1);
    exchange_simulater->On_state_changed(t2);
    exchange_simulater->On_state_changed(t3);

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
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.11);  bid->set_size(11); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(12.21);  bid->set_size(12); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(20.10);  ask->set_size(20); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(21.20);  ask->set_size(21); }
    { pb::dms::DataPoint *ask = t1.add_offer(); ask->set_price(22.30);  ask->set_size(22); }

    exchange_simulater->On_state_changed(t1);

    pb::dms::L2 t2;
    t2.set_contract("T2");
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(100.01);  bid->set_size(100); }
    { pb::dms::DataPoint *bid = t2.add_bid(); bid->set_price(101.02);  bid->set_size(101); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(201.10);  ask->set_size(201); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(202.20);  ask->set_size(202); }
    { pb::dms::DataPoint *ask = t2.add_offer(); ask->set_price(203.30);  ask->set_size(203); }

    exchange_simulater->On_state_changed(t2);

    pb::dms::L2 t3;
    t3.set_contract("T3");
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(1000);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.50);  ask->set_size(900); }

    exchange_simulater->On_state_changed(t3);
    }

    EXPECT_EQ(result_listener->orders().size(), 0);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C1", "T1", 11.2, false, true));

    EXPECT_EQ(result_listener->orders().size(), 1);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C2", "T2", 203.35, true, true));

    EXPECT_EQ(result_listener->orders().size(), 2);
    EXPECT_EQ(result_listener->fills().size(), 0);

    exchange_simulater->Add(make_order("C3", "T3", 900, true, true));

    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 0);

    pb::dms::L2 t1;
    t1.set_contract("T1");
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(10.01);  bid->set_size(10); }
    { pb::dms::DataPoint *bid = t1.add_bid(); bid->set_price(11.20);  bid->set_size(7); }
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
    { pb::dms::DataPoint *bid = t3.add_bid(); bid->set_price(1000);  bid->set_size(1000); }
    { pb::dms::DataPoint *ask = t3.add_offer(); ask->set_price(900.00);  ask->set_size(123); }

    exchange_simulater->On_state_changed(t3);

    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 2);

    EXPECT_EQ(result_listener->fills().at(0).client_order_id(), "C1");
    EXPECT_EQ(result_listener->fills().at(0).exchange_order_id(), "Order-1");
    EXPECT_EQ(result_listener->fills().at(0).fill_id(), "Fill-1");
    EXPECT_EQ(std::stod(result_listener->fills().at(0).fill_price()), 11.2);
    EXPECT_EQ(result_listener->fills().at(0).fill_quantity(), 20);

    EXPECT_EQ(result_listener->fills().at(1).client_order_id(), "C3");
    EXPECT_EQ(result_listener->fills().at(1).exchange_order_id(), "Order-3");
    EXPECT_EQ(result_listener->fills().at(1).fill_id(), "Fill-2");
    EXPECT_EQ(std::stod(result_listener->fills().at(1).fill_price()), 900);
    EXPECT_EQ(result_listener->fills().at(1).fill_quantity(), 20);

    exchange_simulater->On_state_changed(t1);
    exchange_simulater->On_state_changed(t2);
    exchange_simulater->On_state_changed(t3);

    EXPECT_EQ(result_listener->orders().size(), 3);
    EXPECT_EQ(result_listener->fills().size(), 2);

    delete exchange_simulater;
    delete result_listener;
}
#endif
