
#include <string>
#include "core/assist/logger.h"
#include "cme/market/message/message_utility.h"

void decode_message(const std::string &file)
{
    std::vector<std::string> packets = fh::cme::market::message::utility::Read_packets(file);
    for(const std::string &p : packets)
    {
        std::vector<fh::cme::market::message::MdpMessage> message;
        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(p.data(), p.size(), message);

        LOG_INFO("seq=", seq, ", message count=", message.size());

        std::for_each(message.cbegin(), message.cend(), [](const fh::cme::market::message::MdpMessage &m){
            std::string  s = m.Serialize();
            LOG_INFO(s);
        });
    }
}

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

        decode_message(argv[1]);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./packet_test /home/fix/deve/packet.txt
