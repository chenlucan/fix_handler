
#include <boost/lexical_cast.hpp>
#include "core/assist/logger.h"
#include "core/zmq/zmq_sender.h"
#include "core/assist/utility.h"


std::string make_json()
{
    static int id = 1;
    id ++;

    if(id%3==0)
    {
        return "{\"market\":\"TEST_contract\", \"InstrumentID\":\"ins-" + std::to_string( id%10) +
                "\", \"VolumeMultiple\":\"" + std::to_string( id+10) +
                "\", \"insertTime\":\"" + std::to_string(fh::core::assist::utility::Current_time_ns()) +
                "\", \"sendingTimeStr\":\"" + fh::core::assist::utility::Current_time_str() + "\"}";
    }

    return "{\"market\":\"TEST\", \"InstrumentID\":\"ins-" + std::to_string( id%10 + 1) +
            "\", \"insertTime\":\"" + std::to_string(fh::core::assist::utility::Current_time_ns()) +
            "\", \"sendingTimeStr\":\"" + fh::core::assist::utility::Current_time_str() + "\"}";
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            LOG_ERROR("Usage: original_sender_test <send_port> <send_interval_ms>");
            LOG_ERROR("Ex:       original_sender_test 5557 1000");

            return 1;
        }

        auto send_interval_ms = boost::lexical_cast<std::uint16_t>(argv[2]);
        std::string host = std::string("tcp://*:") + argv[1];
        fh::core::zmq::ZmqSender sender(host);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        while(true)
        {
            std::string json = make_json();
            sender.Send(json);
            LOG_INFO("sent: ", json);
            std::this_thread::sleep_for(std::chrono::milliseconds(send_interval_ms));
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./original_sender_test 5557 1000
