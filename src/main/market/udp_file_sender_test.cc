
#include <thread>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "cme/market/message/message_utility.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 5 )
        {
            LOG_ERROR("Usage: udp_sender_test <packet_file_name> <send_address> <send_port> <send_interval_ms>");
            LOG_ERROR("Ex:    udp_file_sender_test /home/fix/deve/packet.txt 224.0.28.123 30001 500");

            return 1;
        }

        auto filename = std::string(argv[1]);
        auto address = boost::asio::ip::address::from_string(argv[2]);
        auto port = boost::lexical_cast<std::uint16_t>(argv[3]);
        auto send_interval_ms = boost::lexical_cast<std::uint16_t>(argv[4]);

        boost::asio::io_service ios;
        boost::asio::ip::udp::endpoint send_ep(address, port);
        boost::asio::ip::udp::socket sock(ios);
        sock.open(boost::asio::ip::udp::v4());

        std::vector<std::pair<std::string, std::string>> packets = fh::cme::market::message::utility::Read_packets(filename);
        LOG_INFO("total packets count: ", packets.size());
        LOG_INFO("packets will start to be sent after 2 seconds.");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        LOG_INFO("Start send");
        for(const auto &p : packets)
        {
            sock.send_to(boost::asio::buffer(p.first), send_ep);

            std::vector<fh::cme::market::message::MdpMessage> messages;
            std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(p.first.data(), p.first.size(), messages);
            LOG_INFO(">>>>>SENT packet: seq=", seq, ", message count=", messages.size());
            for(const auto &m : messages)
            {
                LOG_INFO(m.Serialize(), "\n");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(send_interval_ms));
        }
        LOG_INFO("End send");
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// udp_file_sender_test /home/fix/deve/packet.txt 224.0.28.123 30001 500

