
#ifndef __FH_CORE_UDP_UDP_RECEIVER_H__
#define __FH_CORE_UDP_UDP_RECEIVER_H__

#include <boost/asio.hpp>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace udp
{
    class UDPReceiver
    {
        public:
            UDPReceiver(
                const boost::asio::ip::address &listen_address,
                const std::uint16_t listen_port
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

            void Async_receive_from(std::function<void(char *, const size_t)> processor);

            void Handle_receive_from(
                std::function<void(char *, const size_t)> processer,
                const boost::system::error_code &error_code,
                const size_t bytes_recieved
            );

        private:
            boost::asio::io_service m_io_service;
            boost::asio::ip::udp::socket m_socket;
            boost::asio::ip::udp::endpoint m_sender_endpoint;
            char m_buffer[BUFFER_MAX_LENGTH];

        private:
            DISALLOW_COPY_AND_ASSIGN(UDPReceiver);
    };
} // namespace udp
} // namespace core
} // namespace fh

#endif // __FH_CORE_UDP_UDP_RECEIVER_H__
