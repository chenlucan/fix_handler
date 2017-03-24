
#ifndef __FH_CME_MARKET_RECOVERY_SAVER_H__
#define __FH_CME_MARKET_RECOVERY_SAVER_H__

#include <mutex>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"

namespace fh
{
namespace cme
{
namespace market
{
    // save recovery datas(market recovery, instrument recovery) when later joiner startup
    class RecoverySaver
    {
        public:
            explicit RecoverySaver(bool is_definition);
            virtual ~RecoverySaver();

        public:
            void Process_recovery_packet(std::function<void()> callback, char *buffer, const size_t data_length);
            std::vector<fh::cme::market::message::MdpMessage> *Get_data();

        private:
            bool Save_packet(char *buffer, const size_t data_length, const std::uint32_t packet_seq_num);

        private:
            bool m_is_definition;
            std::uint32_t m_current_sequence;
            std::uint32_t m_total_number;       // 总共需要接收的 message 数
            std::vector<fh::cme::market::message::MdpMessage> m_recovery_datas;
            std::mutex m_mutex;

        private:
            DISALLOW_COPY_AND_ASSIGN(RecoverySaver);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_RECOVERY_SAVER_H__
