
#include "sbe_encoder.h"
#include "utility.h"

namespace rczg
{
namespace utility
{

    // get current time in nanoseconds since epoch
    std::uint64_t Current_time_ns()
    {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    }
    
    // translate each char in buffer to hex format for display
    // ex: "abc" -> "61 62 63 "
    std::string Hex_str(const char *buffer, std::size_t length)
    {
        std::ostringstream ss;

        for (std::size_t i = 0; i < length; ++i)
        {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)(unsigned char)buffer[i] << " ";
        }

        return ss.str();
    }

    // generate random number between min and max
    std::uint32_t Random_number(std::uint32_t min, std::uint32_t max)
    {
        static std::default_random_engine g;
        std::uniform_int_distribution<std::uint32_t> r(min, max);  
        return r(g);
    }

    // make a mdp message for test
    std::size_t make_message(char *position)
    {
        rczg::SBEEncoder encoder;
        encoder.Start_encode(rczg::utility::Random_number(0, 1) ? 30 : 37);
        std::pair<const char*, std::size_t> encoded = encoder.Encoded_buffer();
        
        std::size_t message_size = encoded.second;
        std::memcpy(position, &message_size, 2);
        std::memcpy(position + 2, encoded.first, message_size);
        
        //std::cout << "   size=" << std::setw(3) << message_size;
        //std::cout << "   msg=" << rczg::utility::Hex_str(encoded.first, message_size);
        //std::cout << std::endl;
        
        return message_size + 2;
    }

    // make a mdp packet for test
    std::size_t make_packet(char *buffer, std::uint32_t packet_seq_num)
    {
        // data layout:
        //  MsgSeqNum : 4 bytes
        //  SendingTime : 8 bytes
        //  Message :
        //   MsgSize : 2 bytes
        //   SBE data

        auto time = rczg::utility::Current_time_ns();
        std::memcpy(buffer, &packet_seq_num, 4);
        std::memcpy(buffer + 4, &time, 8);
        
        char *position = buffer + 4 + 8;
        std::uint32_t message_num = rczg::utility::Random_number(1, 3);
        //std::cout << ">>>seq=" << std::setw(6) << packet_seq_num;
        //std::cout << "   time=" << std::setw(20) << time;
        //std::cout << "   num=" << message_num << std::endl;
        for(std::uint32_t i=0;i<message_num;++i)
        {
            std::size_t message_total_size = make_message(position);
            position = position + message_total_size;
        }
        
        return position - buffer;
    }
    
}
}