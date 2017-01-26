
#include "tcp_receiver.h"
#include "utility.h"

namespace rczg
{
    
    TCPReceiver::TCPReceiver(const boost::asio::ip::address &server_address, const unsigned short server_port) 
    {
        // TODO
    }

    // request data([begin, end)) from tcp replay, and process it with processor
    void TCPReceiver::Start_receive(std::function<void(char *, const size_t)> processor, std::uint32_t begin, std::uint32_t end)
    {
        // TODO use quickfix to get replay data
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for(std::uint32_t i = begin; i < end; ++i)
        {
            char buffer[BUFFER_MAX_LENGTH];
            std::size_t bytes_recieved = rczg::utility::Make_packet(buffer, i);
            processor(buffer, bytes_recieved);
        }
    }
    
    TCPReceiver::~TCPReceiver()
    {
        // noop
    }
    
} // namespace rczg
    