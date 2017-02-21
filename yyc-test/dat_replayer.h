
#ifndef __DAT_REPLAYER_H__
#define __DAT_REPLAYER_H__

#include "global.h"
#include "tcp_receiver.h"

namespace rczg
{
    class DatReplayer
    {
        public:
            DatReplayer(
            		const boost::asio::ip::address &server_address,
					const unsigned short server_port,
        			const std::string &username,
        			const std::string &password,
        			const std::string &channel_id);
            virtual ~DatReplayer();
            
        public:
            // request data([begin, end)) from tcp receiver, and process it with processor
            void Start_receive(std::function<void(char *, const size_t)> processor, std::uint32_t begin, std::uint32_t end);
            
        private:
            void Send_Logon(
            		rczg::TCPReceiver &tcp_receiver,
        			std::function<void(char *, const size_t)> processor,
        			std::uint32_t begin, std::uint32_t end);
            void On_Logon(
            		rczg::TCPReceiver &tcp_receiver,
        			std::function<void(char *, const size_t)> processor,
        			std::uint32_t begin, std::uint32_t end,
        			char *buffer, const size_t length);
            void Send_recovery_request(
            		rczg::TCPReceiver &tcp_receiver,
        			std::function<void(char *, const size_t)> processor,
        			std::uint32_t begin, std::uint32_t end);
            void On_recovery_data(
            		rczg::TCPReceiver &tcp_receiver,
        			std::function<void(char *, const size_t)> processor,
        			char *buffer, const size_t length);

        private:
            boost::asio::ip::tcp::endpoint m_endpoint;
            std::string m_username;
            std::string m_password;
            std::string m_channel_id;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatReplayer);
    };
}
    
#endif // __DAT_REPLAYER_H__
