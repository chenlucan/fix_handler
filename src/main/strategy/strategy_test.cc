
#include <boost/lexical_cast.hpp>
#include "core/global.h"
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/zmq/zmq_receiver.h"
#include "core/zmq/zmq_sender.h"
#include "pb/ems/ems.pb.h"

class BookReceiver : public fh::core::zmq::ZmqReceiver
{
    public:
        explicit BookReceiver(const std::string &url) : fh::core::zmq::ZmqReceiver(url) {}
        virtual ~BookReceiver() {}

    public:
        virtual void Save(char *data, size_t size)
        {
            char type = data[0];
            if(type == 'L')
            {
                pb::dms::L2 l2;
                if(l2.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received L2: ", fh::core::assist::utility::Format_pb_message(l2));
                }
                else
                {
                    LOG_ERROR("received L2: parse error: ", type);
                }
            }
            else if(type == 'B')
            {
                pb::dms::BBO bbo;
                if(bbo.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received BBO: ", fh::core::assist::utility::Format_pb_message(bbo));
                }
                else
                {
                    LOG_ERROR("received BBO: parse error: ", type);
                }
            }
            else if(type == 'O')
            {
                pb::dms::Offer offer;
                if(offer.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received Offer: ", fh::core::assist::utility::Format_pb_message(offer));
                }
                else
                {
                    LOG_ERROR("received Offer: parse error: ", type);
                }
            }
            else if(type == 'D')
            {
                pb::dms::Bid bid;
                if(bid.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received Bid: ", fh::core::assist::utility::Format_pb_message(bid));
                }
                else
                {
                    LOG_ERROR("received Bid: parse error: ", type);
                }
            }
            else if(type == 'T')
            {
                pb::dms::Trade trade;
                if(trade.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received Trade: ", fh::core::assist::utility::Format_pb_message(trade));
                }
                else
                {
                    LOG_ERROR("received Trade: parse error: ", type);
                }
            }
            else if(type == 'C')
            {
                pb::dms::Contract contract;
                if(contract.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received Contract: ", fh::core::assist::utility::Format_pb_message(contract));
                }
                else
                {
                    LOG_ERROR("received Contract: parse error: ", type);
                }
            }
            else
            {
                LOG_ERROR("received book result: invalid type: ", type);
            }
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(BookReceiver);
};

class OrderResultReceiver : public fh::core::zmq::ZmqReceiver
{
    public:
        explicit OrderResultReceiver(const std::string &url) : fh::core::zmq::ZmqReceiver(url) {}
        virtual ~OrderResultReceiver() {}

    public:
        virtual void Save(char *data, size_t size)
        {
            char type = data[0];
            if(type == 'F')
            {
                pb::ems::Fill fill;
                if(fill.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received order result: ", fh::core::assist::utility::Format_pb_message(fill));
                }
                else
                {
                    LOG_ERROR("received order result: parse error: ", type);
                }
            }
            else if(type == 'O')
            {
                pb::ems::Order order;
                if(order.ParseFromArray(data + 1, size - 1))
                {
                    LOG_INFO("received order result: ", fh::core::assist::utility::Format_pb_message(order));
                }
                else
                {
                    LOG_ERROR("received order result: parse error: ", type);
                }
            }
            else
            {
                LOG_ERROR("received order result: invalid type: ", type);
            }
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(OrderResultReceiver);
};

std::string make_order()
{
    std::uint32_t r = fh::core::assist::utility::Random_number(0, 100);
    char type = '1';     // 1:D 2:F 3:G 4:H 5:AF 6:CA
    if(r < 10) type += 5;
    else if(r < 15) type += 4;
    else if(r < 25) type += 3;
    else if(r < 30) type += 2;
    else if(r < 35) type += 1;

    // AF, CA
    if(type >= '5')
    {
        std::string id = "X-" + std::to_string(fh::core::assist::utility::Current_time_ns());
        std::string order_type = "3";   // 1: Instrument  3: Instrument Group  7: All Orders
        std::string name = "07";
        std::string mass_order = id + order_type + name;
        LOG_INFO("send mass order:  (", type, ")", mass_order);
        return mass_order.insert(0, 1, type);
    }

    pb::ems::Order *order = new pb::ems::Order;
    static int id = 0;
    if(id < 5) type = '1';  // first 5 orders is new order

    if(type == '1')
    {
        // D
        int bsflag = fh::core::assist::utility::Random_number(0, 1);
        int price = fh::core::assist::utility::Random_number(1, 15);
        order->set_client_order_id("COI-" + std::to_string(++id));
        order->set_buy_sell(bsflag == 0 ? pb::ems::BuySell::BS_Buy : pb::ems::BuySell::BS_Sell);
        order->set_price(price < 5 ? "0" : std::to_string((bsflag == 0 ? 8 : 13) + price + (id%6 == 1 ? 0.5 : 0)));
        order->set_quantity(price < 4 ? price + 5 : price);
        order->set_tif(price % 5 == 0 ? pb::ems::TimeInForce::TIF_FOK :
                (price % 5 == 3 ? pb::ems::TimeInForce::TIF_FAK : pb::ems::TimeInForce::TIF_GFD));
        order->set_order_type(price < 4 ? pb::ems::OrderType::OT_Market : pb::ems::OrderType::OT_Limit);
        //order->set_exchange_order_id("9923898474");
        //order->set_status(pb::ems::OrderStatus::OS_Pending);
        //order->set_working_price("398");
        //order->set_working_quantity(r / 2+ 10);
        //order->set_filled_quantity(r - r / 2 + 10);
        //order->set_message("cme test order");
        fh::core::assist::utility::To_pb_time(order->mutable_submit_time(),
                fh::core::assist::utility::Current_time_str("%Y%m%d-%H:%M:%S.%f").substr(0, 21));
    }
    else
    {
        // F, G, H
        int bsflag = fh::core::assist::utility::Random_number(0, 1);
        int price = fh::core::assist::utility::Random_number(1, 15);
        int target_id = fh::core::assist::utility::Random_number(1, id);
        order->set_client_order_id("COI-" + std::to_string(target_id));
        order->set_buy_sell(bsflag == 0 ? pb::ems::BuySell::BS_Buy : pb::ems::BuySell::BS_Sell);
        order->set_price(price < 5 ? "0" : std::to_string((bsflag == 0 ? 8 : 13) + price + (id%6 == 1 ? 0.5 : 0)));
        order->set_quantity(price);
        order->set_tif(price % 5 == 0 ? pb::ems::TimeInForce::TIF_FOK :
                (price % 5 == 3 ? pb::ems::TimeInForce::TIF_FAK : pb::ems::TimeInForce::TIF_GFD));
        order->set_order_type(price < 4 ? pb::ems::OrderType::OT_Market : pb::ems::OrderType::OT_Limit);
    }

    //order->set_account("YYC");   // unuse
    order->set_contract("CON-" + std::to_string(id%2 + 1));

    LOG_INFO("send order:  (", type, ")", fh::core::assist::utility::Format_pb_message(*order));
    std::string str = order->SerializeAsString();
    delete order;
    return str.insert(0, 1, type);
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 5)
        {
            LOG_ERROR("Usage: strategy_test <send_port> <result_receive_port> <book_receive_port> <send_interval_ms>");
            LOG_ERROR("Ex:       strategy_test 6001 6002 5558 1000");

            return 1;
        }

        std::thread book_receiver([argv]{
            LOG_INFO("start receiver book on port ", argv[3]);
            std::string host = std::string("tcp://localhost:") + argv[3];
            BookReceiver r(host);
            r.Start_receive();
        });

        std::thread result_receiver([argv]{
               LOG_INFO("start result receiver on port ", argv[2]);
               std::string host = std::string("tcp://localhost:") + argv[2];
            OrderResultReceiver r(host);
            r.Start_receive();
        });

        auto send_interval_ms = boost::lexical_cast<std::uint16_t>(argv[4]);
        if(send_interval_ms == 0)
        {
            book_receiver.join();
        }

        LOG_INFO("start sender on port ", argv[1]);
        std::string host = std::string("tcp://*:") + argv[1];
        fh::core::zmq::ZmqSender sender(host);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        while(true)
        {
            std::string order = make_order();
            sender.Send(order);
            std::this_thread::sleep_for(std::chrono::milliseconds(send_interval_ms));
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./strategy_test 6001 6002 5558 1000
