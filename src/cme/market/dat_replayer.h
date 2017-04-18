
#ifndef __FH_CME_MARKET_DAT_REPLAYER_H__
#define __FH_CME_MARKET_DAT_REPLAYER_H__

#include "core/global.h"
#include "core/tcp/tcp_receiver.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DatReplayer
    {
        public:
            DatReplayer(
                    const boost::asio::ip::address &server_address,
                    const std::uint16_t server_port,
                    const std::string &username,
                    const std::string &password,
                    const std::string &channel_id);
            virtual ~DatReplayer();

        public:
            // request data([begin, end)) from tcp receiver, and process it with processor
            void Start_receive(std::function<void(char *, const size_t)> processor, std::uint32_t begin, std::uint32_t end);
            void Stop();

        private:
            void Send_Logon(
                    fh::core::tcp::TCPReceiver &tcp_receiver,
                    std::function<void(char *, const size_t)> processor,
                    std::uint32_t begin, std::uint32_t end);
            void On_Logon(
                    fh::core::tcp::TCPReceiver &tcp_receiver,
                    std::function<void(char *, const size_t)> processor,
                    std::uint32_t begin, std::uint32_t end,
                    char *buffer, const size_t length);
            void Send_recovery_request(
                    fh::core::tcp::TCPReceiver &tcp_receiver,
                    std::function<void(char *, const size_t)> processor,
                    std::uint32_t begin, std::uint32_t end);
            void On_recovery_data(
                    fh::core::tcp::TCPReceiver &tcp_receiver,
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
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_REPLAYER_H__
