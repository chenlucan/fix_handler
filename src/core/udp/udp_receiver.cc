
#include <boost/bind.hpp>
#include "core/udp/udp_receiver.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace core
{
namespace udp
{

    UDPReceiver::UDPReceiver(
        const boost::asio::ip::address &listen_address,
        const std::uint16_t listen_port
    ) : m_io_service(), m_sockets(), m_sender_endpoint(), m_recv_endpoint1(), 
        m_is_stopped(false), m_seqNum(0), m_nextSeqNum(0)
    {
        this->Initialize_socket(listen_address, listen_port);
    }

    UDPReceiver::UDPReceiver(
        const std::vector<std::pair<const boost::asio::ip::address, 
        const std::uint16_t>> &socket_datas
    ) : m_io_service(), m_sockets(), m_sender_endpoint(), m_recv_endpoint1(), m_seqNum(0), m_nextSeqNum(0)
    {
        this->Initialize_socket(socket_datas);
    }
    
    // receive data from udp feed, and process it with processor
    // the processor will access buffer directly
    void UDPReceiver::Start_receive(std::function<void(char *, const size_t)> processor)
    {
        //LOG_DEBUG("(udp receiver start:", m_socket.local_endpoint().address().to_string(), ":", m_socket.local_endpoint().port(), ")");
        LOG_DEBUG("===== UDPReceiver::Start_receive =====");
        //std::vector< boost::asio::ip::udp::socket >::iterator iter = m_sockets.begin();
		std::vector<socket_ptr>::iterator iter = m_sockets.begin();
        for(; iter!=m_sockets.end(); ++iter)
        {
            LOG_DEBUG("===================================");
            this->Async_receive_from(processor, *iter);
        }
        m_io_service.run();
    }

    // stop this receiver
    void UDPReceiver::Stop()
    {
        LOG_DEBUG("===== UDPReceiver::Stop =====");
        m_is_stopped = true;
        auto iter = m_sockets.begin();
        for(; iter!=m_sockets.end(); ++iter)
        {
            LOG_DEBUG("(udp receiver stopped:", (*iter)->local_endpoint().address().to_string(), ":", (*iter)->local_endpoint().port(), ")");
            (*iter)->cancel();
            (*iter)->close();
        }
        
        m_io_service.stop();
        //LOG_DEBUG("(udp receiver stopped:", m_socket.local_endpoint().address().to_string(), ":", m_socket.local_endpoint().port(), ")");
    }

    void UDPReceiver::Initialize_socket(
        const boost::asio::ip::address &listen_address,
        const std::uint16_t listen_port
    )
    {
        boost::asio::ip::udp::endpoint listen_endpoint(listen_address, listen_port);
    }

    void UDPReceiver::Initialize_socket(
        const std::vector<std::pair<const boost::asio::ip::address, const std::uint16_t>> &socket_datas
    )
    {
        LOG_DEBUG("===== UDPReceiver::Initialize_socket =====");
		 
        for(const auto &item : socket_datas)
        {
            boost::asio::ip::udp::endpoint listen_endpoint(item.first, item.second);
            LOG_DEBUG("(udp receiver Initialize_socket:", listen_endpoint.address().to_string(), ":", listen_endpoint.port(), ")");
            socket_ptr sock(new boost::asio::ip::udp::socket(m_io_service));
            sock->open(listen_endpoint.protocol());
            sock->set_option(boost::asio::ip::udp::socket::reuse_address(true));
            sock->set_option(boost::asio::ip::multicast::join_group(item.first));
            boost::asio::socket_base::receive_buffer_size recv_option(16*65534);  
            sock->set_option(recv_option);  
            sock->bind(listen_endpoint);
            m_sockets.push_back(sock);
        }   		

        auto itersock = m_sockets.cbegin();
        if(itersock!=m_sockets.cend())
        {
            m_recv_endpoint1.address((*itersock)->local_endpoint().address());
            m_recv_endpoint1.port((*itersock)->local_endpoint().port());
        }
		
        LOG_DEBUG("===== m_sockets.size = [", m_sockets.size(), "] =====");
    }
    
    void UDPReceiver::Async_receive_from(std::function<void(char *, const size_t)> processor, socket_ptr socket)
    {        
        LOG_DEBUG("=====  UDPReceiver::Async_receive_from =====");
        boost::asio::ip::address org_address(socket->local_endpoint().address());
        unsigned short org_port_num = socket->local_endpoint().port();
        boost::asio::ip::address recv_address1(m_recv_endpoint1.address());
        unsigned short recv_port_num1 = m_recv_endpoint1.port();

        LOG_DEBUG("\n==============================\n org_address = ", org_address.to_string(), ",\n recv_address1 = ", recv_address1, ",\n org_port_num = ", org_port_num, ",\n recv_port_num1 = ", recv_port_num1, "\n==============================");

        if( (org_port_num == recv_port_num1)
            && (org_address.to_string() == recv_address1.to_string()) )
        {
            LOG_DEBUG("[master] (udp receiver async_receive_from:", org_address.to_string(), ":", org_port_num, ")");
            socket->async_receive_from(
                        boost::asio::buffer(m_buffer1), 
                        m_sender_endpoint,
                        boost::bind(
                            &UDPReceiver::Handle_receive_from, 
                            this,
                            processor,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred, 
                            socket
                        )
                    ); 
        }
        else
        {
            LOG_DEBUG("[slaver](udp receiver async_receive_from:", org_address.to_string(), ":", org_port_num, ")");
            socket->async_receive_from(
                        boost::asio::buffer(m_buffer2), 
                        m_sender_endpoint,
                        boost::bind(
                            &UDPReceiver::Handle_receive_from, 
                            this,
                            processor,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred, 
                            socket
                        )
                    ); 
        }
    }

    void UDPReceiver::Handle_receive_from(
        std::function<void(char *, const size_t)> processor,
        const boost::system::error_code &error_code,
        const size_t bytes_recieved,
        socket_ptr socket
    )
    {
        //LOG_DEBUG("=================== [begin] UDPReceiver::Handle_receive_from ===================");
        LOG_DEBUG("=================== [begin] UDPReceiver::Handle_receive_from ===================");
        
        if (error_code && error_code != boost::asio::error::message_size)
        {
            LOG_DEBUG("===== error_code=(", error_code, ") error_code != boost::asio::error::message_size, return! =====");
            LOG_ERROR("=================== [end] UDPReceiver::Handle_receive_from , error end, return !!!!===================");

            return;
        }
        
        boost::asio::ip::address org_address(socket->local_endpoint().address());
        unsigned short org_port_num = socket->local_endpoint().port();
        boost::asio::ip::address recv_address1(m_recv_endpoint1.address());
        unsigned short recv_port_num1 = m_recv_endpoint1.port();
        
        LOG_DEBUG("\n==============================\n org_address = ", org_address.to_string(), ",\n recv_address1 = ", recv_address1, ",\n org_port_num = ", org_port_num, ",\n recv_port_num1 = ", recv_port_num1, "\n==============================");

        if( (org_port_num == recv_port_num1)
            && (org_address.to_string() == recv_address1.to_string()) )
        {
            std::uint32_t packet_seq_num = *((std::uint32_t *)m_buffer1);

            LOG_DEBUG("---------------------------------- [master] -------------------------------------------------");
            LOG_TRACE("[master][Handle_receive_from] udp received from [seq=",packet_seq_num, ", ", 
                org_address.to_string(), ":", org_port_num, "](", bytes_recieved, ")=", 
                fh::core::assist::utility::Hex_str(m_buffer1, bytes_recieved)); 

            #ifdef _MUT_CME
            if(packet_seq_num!= m_seqNum+1)
            {
                LOG_ERROR("***************************************************"); 
                LOG_ERROR("error: seq=",packet_seq_num, ", lastseq = ",m_seqNum, "!!!"); 
                LOG_ERROR("***************************************************");
                exit(0);
            }
            else
            {
                LOG_DEBUG("===== [", m_seqNum, "->", packet_seq_num, "] ====="); 
                m_seqNum = packet_seq_num;            
            }
            #endif
            processor(m_buffer1, bytes_recieved);

            LOG_DEBUG("---------------------------------- [master] -------------------------------------------------");
        }
        else
        {
            std::uint32_t packet_seq_num = *((std::uint32_t *)m_buffer2);

            LOG_DEBUG("^^^^^^^^^^^^^^^^^^^^^^^^^^ [slaver] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

            LOG_TRACE("[slaver][Handle_receive_from] udp received from [seq=",packet_seq_num, ", ",
                org_address.to_string(), ":", org_port_num, "](", bytes_recieved, ")=", 
                fh::core::assist::utility::Hex_str(m_buffer2, bytes_recieved)); 

            #ifdef _MUT_CME
            if(packet_seq_num!= m_nextSeqNum+1)
            {
                LOG_ERROR("***************************************************"); 
                LOG_ERROR("error: seq=",packet_seq_num, ", lastseq = ",m_nextSeqNum, "!!!"); 
                LOG_ERROR("***************************************************"); 
                exit(0);
            }
            else
            {
                LOG_DEBUG("===== [", m_nextSeqNum, "->", packet_seq_num, "] ====="); 
                m_nextSeqNum = packet_seq_num;    
            }
            #endif
            processor(m_buffer2, bytes_recieved);

            LOG_DEBUG("^^^^^^^^^^^^^^^^^^^^^^^^^^ [slaver] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        }
                
        this->Async_receive_from(processor, socket);

        LOG_DEBUG("=================== [end] UDPReceiver::Handle_receive_from ===================");
    }

    UDPReceiver::~UDPReceiver()
    {
        // noop
    }

} // namespace udp
} // namespace core
} // namespace fh
