
#include <fstream>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "core/assist/logger.h"
#include "cme/market/persist/mdp_receiver.h"
#include "cme/market/message/message_utility.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            LOG_ERROR("Usage: packet_test <input_file>");
            LOG_ERROR("Ex:    packet_test /home/fix/deve/packet.txt");

            return 1;
        }

        std::ifstream input(argv[1]);
        std::ostringstream oss;
        std::string line;
        while (std::getline(input, line))
        {
            oss << line << " ";
        }

        std::vector<std::string> tokens;
        std::istringstream iss(oss.str());
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

        std::vector<char> bytes;
        int index = 0;
        std::ostringstream byte_line;
        std::for_each(tokens.begin(), tokens.end(), [&bytes, &index, &byte_line](std::string &s){
            boost::trim_right(s);
            boost::trim_left(s);
            if(s != "")
            {
                char x = (char) strtol(s.c_str(), nullptr, 16);
                bytes.push_back(x);

                index ++;
                std::string dec;
                if(index % 40 == 0) dec = "\n";
                else if(index % 20 == 0) dec = "    ";
                else if(index % 10 == 0) dec = "  ";
                else dec = " ";
                byte_line << s << dec;
            }
        });

        LOG_INFO("file read ok, size=", bytes.size(), "\n", byte_line.str());

        std::vector<fh::cme::market::message::MdpMessage> message;
        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(bytes.data(), bytes.size(), message);

        LOG_INFO("seq=", seq, ", message count=", message.size());

        auto receiver = new fh::cme::market::persist::MdpReceiver("tcp://localhost:55555", "mongodb://localhost:55556/","a", "b");
        std::for_each(message.cbegin(), message.cend(), [&receiver](const fh::cme::market::message::MdpMessage &m){
            std::string  s = m.Serialize();
            receiver->Save(const_cast<char*>(s.data()), s.length());
        });
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./packet_test /home/fix/deve/packet.txt
