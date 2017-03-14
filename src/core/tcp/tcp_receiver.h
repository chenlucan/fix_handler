
#ifndef __FH_CORE_TCP_TCP_RECEIVER_H__
#define __FH_CORE_TCP_TCP_RECEIVER_H__

#include <string>
#include <boost/asio.hpp>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace tcp
{
    class TCPReceiver
    {
        public:
            explicit TCPReceiver(const boost::asio::ip::tcp::endpoint &endpoint);
            virtual ~TCPReceiver();

        public:
            void Send(std::function<void(char *, const size_t)> processor, const std::string &message);
            void Start_receive();
            void Receive(std::function<void(char *, const size_t)> processor);
            void Stop();

        private:
            void On_received(
                    std::function<void(char *, const size_t)> processor,
                    const boost::system::error_code &error, std::size_t bytes_transferred);

        private:
            boost::asio::io_service m_io_service;
            boost::asio::ip::tcp::socket m_socket;
            char m_buffer[BUFFER_MAX_LENGTH];

        private:
            DISALLOW_COPY_AND_ASSIGN(TCPReceiver);
    };
} // namespace tcp
} // namespace core
} // namespace fh

#endif // __FH_CORE_TCP_TCP_RECEIVER_H__
