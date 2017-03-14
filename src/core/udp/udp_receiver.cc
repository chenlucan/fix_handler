
#include <boost/bind.hpp>
#include "core/udp/udp_receiver.h"
#include "core/assist/logger.h"

namespace fh
{
namespace core
{
namespace udp
{

    UDPReceiver::UDPReceiver(
        const boost::asio::ip::address &listen_address,
        const std::uint16_t listen_port
    ) : m_io_service(), m_socket(m_io_service)
    {
        this->Initialize_socket(listen_address, listen_port);
    }

    // receive data from udp feed, and process it with processor
    // the processor will access buffer directly
    void UDPReceiver::Start_receive(std::function<void(char *, const size_t)> processor)
    {
        this->Async_receive_from(processor);
        m_socket.get_io_service().run();
    }

    // stop this receiver
    void UDPReceiver::Stop()
    {
        m_socket.cancel();
        m_io_service.stop();
        LOG_DEBUG("(udp receiver stopped)");
    }

    void UDPReceiver::Initialize_socket(
        const boost::asio::ip::address &listen_address,
        const std::uint16_t listen_port
    )
    {
        // Initial the socket so that multiple may be bound to the same address.
        boost::asio::ip::udp::endpoint listen_endpoint(listen_address, listen_port);
        m_socket.open(listen_endpoint.protocol());
        m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        m_socket.bind(listen_endpoint);
    }

    void UDPReceiver::Async_receive_from(std::function<void(char *, const size_t)> processor)
    {
        m_socket.async_receive_from(
            boost::asio::buffer(m_buffer), 
            m_sender_endpoint,
            boost::bind(
                &UDPReceiver::Handle_receive_from, 
                this,
                processor,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    void UDPReceiver::Handle_receive_from(
        std::function<void(char *, const size_t)> processor,
        const boost::system::error_code &error_code,
        const size_t bytes_recieved
    )
    {
        if (error_code && error_code != boost::asio::error::message_size)
        {
            return;
        }

        processor(m_buffer, bytes_recieved);

        this->Async_receive_from(processor);
    }

    UDPReceiver::~UDPReceiver()
    {
        // noop
    }

} // namespace udp
} // namespace core
} // namespace fh
