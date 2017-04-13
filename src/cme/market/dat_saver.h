
#ifndef __FH_CME_MARKET_DAT_SAVER_H__
#define __FH_CME_MARKET_DAT_SAVER_H__

#include <set>
#include <mutex>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/book_manager.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DatSaver
    {
        public:
            explicit DatSaver(fh::core::market::MarketListenerI *book_sender);
            virtual ~DatSaver();

        public:
            // save mdp messages to memory
            virtual void Insert_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages);
            // process the messages in memory and save to zeroqueue
            virtual void Start_save();
            // set received definition，recovery messages
            void Set_recovery_data(
                    std::vector<fh::cme::market::message::MdpMessage> *definition_datas,
                    std::vector<fh::cme::market::message::MdpMessage> *recovery_datas);
            // stop saver
            void Stop();

        private:
            // send definition messages to db
            void Send_definition_messages();
            // send recovery messages to db
            void Send_recovery_messages();
            // pick first message to serialize and remove it from memory
            std::pair<std::multiset<fh::cme::market::message::MdpMessage>::iterator, bool> Pick_next_message();
            // 如果有必要的话对指定 message 进行变换（结合 recovery message）；同时提取 book 信息，整理成 book state 发送出去
            bool Convert_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message);
            // send increment messages to db
            void Send_message(const fh::cme::market::message::MdpMessage &message);
            // remove increment message after sent
            void Remove_past_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message);
            // 获取到第一条数据的 sequence number（如果没有数据，则一直等待）
            std::uint32_t Get_first_data_seq();
            // 处理恢复数据（如果没有恢复数据，则一直等待）
            void Process_recovery_data();

        private:
            struct Message_Compare
            {
                bool operator() (const fh::cme::market::message::MdpMessage &a, const fh::cme::market::message::MdpMessage &b)
                {
                    return a.packet_seq_num() < b.packet_seq_num();
                }
            };

        private:
            std::uint32_t m_last_seq;
            std::set<std::uint32_t> m_unreceived_seqs;
            std::mutex m_mutex;
            std::mutex m_recovery_mutex;
            std::multiset<fh::cme::market::message::MdpMessage, Message_Compare> m_datas;
            std::vector<fh::cme::market::message::MdpMessage> *m_definition_datas;
            std::vector<fh::cme::market::message::MdpMessage> *m_recovery_datas;
            fh::core::market::MarketListenerI *m_book_sender;
            fh::cme::market::BookManager m_book_manager;
            std::uint32_t m_recovery_first_seq;
            bool m_is_stopped;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatSaver);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_SAVER_H__
