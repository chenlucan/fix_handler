
#ifndef __FH_CORE_UDP_UDP_RECEIVER_H__
#define __FH_CORE_UDP_UDP_RECEIVER_H__

#include <boost/asio.hpp>
#include <unordered_map> 
#include <set>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace udp
{
    typedef boost::shared_ptr<boost::asio::ip::udp::socket> socket_ptr;

    class UDPReceiver
    {
        public:
            UDPReceiver(
                const boost::asio::ip::address &listen_address,
                const std::uint16_t listen_port
            );

            UDPReceiver(
                const std::vector< std::pair<const boost::asio::ip::address, const std::uint16_t> > &socket_datas
            );
            virtual ~UDPReceiver();

        public:
            // receive data from udp feed, and process it with processor
            // the processor will access buffer directly
            void Start_receive(std::function<void(char *, const size_t)> processor);
            // stop this receiver
            void Stop();

        private:
            void Initialize_socket(
                const boost::asio::ip::address &listen_address,
                const std::uint16_t listen_port
            );


            void Initialize_socket(
                const std::vector<std::pair<const boost::asio::ip::address, 
                const std::uint16_t>> &socket_datas
            );

	
            void Async_receive_from(std::function<void(char *, const size_t)> processor, socket_ptr socket);


            void Handle_receive_from(
                std::function<void(char *, const size_t)> processer,
                const boost::system::error_code &error_code,
                const size_t bytes_recieved, 
                socket_ptr socket
            );

        private:
            boost::asio::io_service m_io_service;
            std::vector<socket_ptr> m_sockets;
            
            boost::asio::ip::udp::endpoint m_sender_endpoint;

            boost::asio::ip::udp::endpoint m_recv_endpoint1;

            std::atomic_bool m_is_stopped;
            
            std::uint32_t m_seqNum;
            std::uint32_t m_nextSeqNum;

            char m_buffer1[BUFFER_MAX_LENGTH*8];  // Master
            char m_buffer2[BUFFER_MAX_LENGTH*8];  // Slaver


        private:
            DISALLOW_COPY_AND_ASSIGN(UDPReceiver);
    };
} // namespace udp
} // namespace core
} // namespace fh

#endif // __FH_CORE_UDP_UDP_RECEIVER_H__
