
#include <thread>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "cme/market/message/message_utility.h"

std::uint32_t next_sequence(std::uint32_t index, std::uint16_t lost_on, std::uint32_t total_number)
{
    if(total_number == std::numeric_limits<std::uint32_t>::max())
    {
        // incremental sequence
        return (lost_on != 0 && index % lost_on < 3) ? 0 : index;
    }
    else
    {
        // recovery sequence
        std::uint32_t seq = (index - 1) % total_number + 1;
        return (lost_on != 0 && seq % lost_on == 0 && index / total_number % 3 != 1) ? 0 : seq;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 6 || (strcmp(argv[1], "-d") != 0 && strcmp(argv[1], "-r") != 0 && strcmp(argv[1], "-i") != 0 && strcmp(argv[1], "-a") != 0))
        {
            LOG_ERROR("Usage: udp_sender_test -d|-r|-i|-a <send_address> <send_port> <send_interval_ms> <lost_on>");
            LOG_ERROR("Ex:    udp_sender_test -d 0.0.0.0 30001 500 13");

            return 1;
        }

        auto address = boost::asio::ip::address::from_string(argv[2]);
        auto port = boost::lexical_cast<std::uint16_t>(argv[3]);
        auto send_interval_ms = boost::lexical_cast<std::uint16_t>(argv[4]);
        auto lost_on = boost::lexical_cast<std::uint16_t>(argv[5]);

        std::uint32_t *ids;
        size_t ids_len;
        std::uint32_t total_number;
        if(strcmp(argv[1], "-d") == 0)
        {
            ids = new uint32_t[3]{27, 29, 41};
            ids_len = 3;
            total_number = 10;
        }
        else if(strcmp(argv[1], "-r") == 0)
        {
            ids = new uint32_t[2]{38, 44};
            ids_len = 2;
            total_number = 10;
        }
        else if(strcmp(argv[1], "-i") == 0)
        {
            ids = new uint32_t[11]{4, 30, 32, 33, 34, 35, 36, 37, 39, 42, 43};
            ids_len = 11;
            total_number = std::numeric_limits<std::uint32_t>::max();
        }
        else if(strcmp(argv[1], "-a") == 0)
        {
            ids = new uint32_t[16]{4, 27, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44};
            ids_len = 16;
            total_number = std::numeric_limits<std::uint32_t>::max();
        }

        boost::asio::io_service ios;
        boost::asio::ip::udp::endpoint send_ep(address, port);
        boost::asio::ip::udp::socket sock(ios);
        sock.open(boost::asio::ip::udp::v4());

        for(std::uint32_t i = 1; i <= std::numeric_limits<std::uint32_t>::max(); ++i)
        {
            std::uint32_t seq = next_sequence(i, lost_on, total_number);
            if(seq == 0) continue;

            char buf[BUFFER_MAX_LENGTH];
            std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
            sock.send_to(boost::asio::buffer(buf, len), send_ep);

            LOG_INFO("sent : seq=", seq, " len=", len);
            LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));

            std::this_thread::sleep_for(std::chrono::milliseconds(send_interval_ms));
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./udp_sender_test -a 192.168.1.185 30001 500 10
// ./udp_sender_test -i 192.168.1.185 30001 500 10
// ./udp_sender_test -d 192.168.1.185 30003 500 3
// ./udp_sender_test -r 192.168.1.185 30006 500 4
