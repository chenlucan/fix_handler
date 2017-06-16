
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "cme/market/message/message_utility.h"

class TCPSession: public std::enable_shared_from_this<TCPSession>
{
    public:
        TCPSession(boost::asio::ip::tcp::socket socket) : m_socket(std::move(socket))
        {
            // send message immediately
            boost::asio::ip::tcp::no_delay option(true);
            m_socket.set_option(option);
        }

        void start()
        {
            do_read();
        }

    private:
        void do_read()
        {
            auto self(shared_from_this());
            m_socket.async_read_some(
                    boost::asio::buffer(m_data),
                    [this, self](boost::system::error_code ec, std::size_t length)
                    {
                        if (ec)
                        {
                            LOG_ERROR("tcp receive error: ", ec);
                        }
                        else
                        {
                            on_received(m_data, length);
                        }
                    }
            );
        }

        void do_write(std::size_t length, char message_type, std::uint32_t current_seq, std::uint32_t last_seq)
        {
            auto self(shared_from_this());
            boost::asio::async_write(
                    m_socket,
                    boost::asio::buffer(m_data, length),
                    [this, self, message_type, current_seq, last_seq](boost::system::error_code ec, std::size_t length)
                    {
                        if (ec)
                        {
                            LOG_ERROR("tcp send error: ", ec);
                        }
                        else
                        {
                            on_sent(m_data, length, message_type, current_seq, last_seq);
                        }
                    }
            );
        }

        void on_received(char *data, std::size_t length)
        {
            std::string message(data, length);
            LOG_INFO("tcp received: length=", length, " data=", message);

            auto type = this->received_message_type(message);
            if(type == 'A')
            {
                // received logon request, so send logon response
                std::size_t packet_size = this->make_logon_packet();
                this->do_write(packet_size, type, 0, 0);
            }
            else // 'V'
            {
                // replay request
                auto range = this->replay_request_range(message);
                std::size_t packet_size = this->make_recovery_packet(range.first);
                this->do_write(packet_size, type, range.first, range.second);
            }
        }

        // A: logon message, V: replay message 5: logout message
        void on_sent(char *data, std::size_t length, char message_type, std::uint32_t current_seq, std::uint32_t last_seq)
        {
            std::string message(data, length);
            LOG_INFO("tcp sent: length=", length, " type=", message_type, " seq=", current_seq, " data=", fh::core::assist::utility::Hex_str(data, length));

            if(message_type == 'A')
            {
                // sent logon message, now wait replay request
                this->do_read();
            }
            else if(message_type == 'V')
            {
                if(current_seq < last_seq)    // continue send recovery message
                {
                    std::size_t packet_size = this->make_recovery_packet(current_seq + 1);
                    this->do_write(packet_size, message_type, current_seq + 1, last_seq);
                }
                else // last recovery message sent
                {
                    std::size_t packet_size = this->make_logout_packet();
                    this->do_write(packet_size, '5', 0, 0);
                }
            }
            else // 5
            {
                // after sent logout message, close socket
                m_socket.close();
            }
        }

        // return A: logon request, V: replay request
        char received_message_type(const std::string &message) const
        {
            // received message is fix format
            // logon request: 35=A
            // replay request: 35=V
            std::string::size_type pos = message.find("35=");
            return message.at(pos + 3);
        }

        std::pair<std::uint32_t, std::uint32_t> replay_request_range(const std::string &message) const
        {
            // 1182=ApplBeginSeqNo 1183=ApplEndSeqNo
            std::string::size_type start1182 = message.find("1182=");
            std::string::size_type end1182 = message.find("\001", start1182 + 5);
            std::string::size_type start1183 = message.find("1183=");
            std::string::size_type end1183 = message.find("\001", start1183 + 5);

            auto start = boost::lexical_cast<std::uint32_t>(message.substr(start1182 + 5, end1182 - start1182 - 5));
            auto end = boost::lexical_cast<std::uint32_t>(message.substr(start1183 + 5, end1183 - start1183 - 5));

            return std::make_pair(start, end);
        }

        std::size_t make_logon_packet()
        {
            uint32_t ids[] = {15};
            std::size_t length = fh::cme::market::message::utility::Make_packet(m_data + 2, 0, ids, 1);
            memcpy(m_data, (char *)&length, 2);    // first 2 bytes is packet length
            return length + 2;
        }

        std::size_t make_logout_packet()
        {
            uint32_t ids[] = {16};
            std::size_t length = fh::cme::market::message::utility::Make_packet(m_data + 2, 0, ids, 1);
            memcpy(m_data, (char *)&length, 2);    // first 2 bytes is packet length
            return length + 2;
        }

        std::size_t make_recovery_packet(std::uint32_t seq)
        {
            uint32_t ids[] = {30, 37};
            std::size_t length = fh::cme::market::message::utility::Make_packet(m_data + 2, seq, ids, 2);
            memcpy(m_data, (char *)&length, 2);    // first 2 bytes is packet length
            return length + 2;
        }

    private:
        boost::asio::ip::tcp::socket m_socket;
        char m_data[BUFFER_MAX_LENGTH];
};

class TCPServer
{
    public:
        TCPServer(boost::asio::io_service& io_service, std::uint16_t port) :
            m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
            m_socket(io_service)
        {
            do_accept();
        }

    private:
        void do_accept()
        {
            m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::make_shared<TCPSession>(std::move(m_socket))->start();
                }

                do_accept();
            });
        }

    private:
        boost::asio::ip::tcp::acceptor m_acceptor;
        boost::asio::ip::tcp::socket m_socket;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            LOG_ERROR("Usage: tcp_sender_test <send_port>");
            LOG_ERROR("Ex:       tcp_sender_test 30007");

            return 1;
        }

        LOG_INFO("tcp server listening on port ", argv[1]);

        boost::asio::io_service io_service;
        TCPServer s(io_service, std::atoi(argv[1]));
        io_service.run();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./tcp_sender_test 30007
