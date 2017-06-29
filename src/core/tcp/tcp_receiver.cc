
#include <boost/bind.hpp>
#include "core/tcp/tcp_receiver.h"
#include "core/assist/utility.h"
#include "core/assist/logger.h"

namespace fh
{
namespace core
{
namespace tcp
{

    TCPReceiver::TCPReceiver(const boost::asio::ip::tcp::endpoint &endpoint)
     : m_io_service(), m_socket(m_io_service)
    {
        boost::asio::ip::tcp::resolver resolver(m_io_service);
        boost::asio::connect(m_socket, resolver.resolve(endpoint));

        LOG_DEBUG("(tcp receiver started)");
    }

    void TCPReceiver::Send(std::function<void(char *, const size_t)> processor, const std::string &message)
    {
        boost::asio::write(m_socket, boost::asio::buffer(message));
        this->Receive(processor);
        LOG_DEBUG("tcp sent : ", message);
    }

    void TCPReceiver::Start_receive()
    {
        m_io_service.run();
    }

    void TCPReceiver::Receive(std::function<void(char *, const size_t)> processor)
    {
        m_socket.async_receive(boost::asio::buffer(m_buffer),
              boost::bind(&TCPReceiver::On_received, this, processor,
                          boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void TCPReceiver::On_received(
            std::function<void(char *, const size_t)> processor,
            const boost::system::error_code &error, std::size_t bytes_transferred)
    {
        if (error && error != boost::asio::error::message_size)
         {
             LOG_ERROR("tcp error: ", error);
             this->Stop();
             return;
         }

        LOG_DEBUG("tcp received : length=", bytes_transferred, ", data=", fh::core::assist::utility::Hex_str(m_buffer, bytes_transferred));
        processor(m_buffer, bytes_transferred);
    }

    void TCPReceiver::Stop()
    {
        m_socket.close();
        m_io_service.stop();
        LOG_DEBUG("(tcp receiver stopped)");
    }

    TCPReceiver::~TCPReceiver()
    {
        // noop
    }

} // namespace tcp
} // namespace core
} // namespace fh
