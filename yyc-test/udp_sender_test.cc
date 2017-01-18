
#include "utility.h"


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 5)
        {
            std::cerr << "Usage: udp_sender_test <send_address> <send_port> <send_interval_ms> <lost_on>" << std::endl;
            std::cerr << "Ex:    udp_sender_test 0.0.0.0 30001 500 13" << std::endl;
            
            return 1;
        }

        auto address = boost::asio::ip::address::from_string(argv[1]);
        auto port = boost::lexical_cast<std::uint16_t>(argv[2]);
        auto send_interval_ms = boost::lexical_cast<std::uint16_t>(argv[3]);
        auto lost_on = boost::lexical_cast<std::uint16_t>(argv[4]);

        boost::asio::io_service ios;
        boost::asio::ip::udp::endpoint send_ep(address,port);
        boost::asio::ip::udp::socket sock(ios);
        sock.open(boost::asio::ip::udp::v4());
        
        for(std::uint32_t i = 1; i <= std::numeric_limits<std::uint32_t>::max(); ++i)
        {
            if(i % lost_on < 3) continue;
            
            char buf[200];
            std::uint16_t len = rczg::utility::make_packet(buf, i);
            sock.send_to(boost::asio::buffer(buf, len), send_ep);

            std::cout << "sent : seq=" << std::setw(6) << i;
            std::cout << " len=" << std::setw(3) << len;
            //std::cout << rczg::utility::Hex_str(buf, len) << std::endl;
            std::cout << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(send_interval_ms));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}

// ./udp_sender_test 192.168.1.185 30001 500
