
#ifndef __FH_CME_MARKET_DAT_SAVER_H__
#define __FH_CME_MARKET_DAT_SAVER_H__

#include <set>
#include <mutex>
#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/book_manager.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DatSaver
    {
        public:
            DatSaver(const std::string &org_save_url, const std::string &book_save_url);
            virtual ~DatSaver();

        public:
            // save mdp messages to memory
            virtual void Insert_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages);
            // process the messages in memory and save to zeroqueue
            virtual void Start_save();
            // set whether start join in middle week
            void Set_later_join(bool is_lj);
            // set received definition messages
            void Set_definition_data(std::vector<fh::cme::market::message::MdpMessage> *definition_datas);
            // set received recovery messages
            void Set_recovery_data(std::vector<fh::cme::market::message::MdpMessage> *recovery_datas);

        private:
            // send definition messages
            void Send_definition_messages();
            // send recovery messages
            void Send_recovery_messages();
            // pick first message to serialize and remove it from memory
            std::multiset<fh::cme::market::message::MdpMessage>::iterator Pick_next_message();
            // merge recovery message to increment message
            bool Convert_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message);
            // send increment messages
            void Send_message(const std::string &serialized_message);
            // remove increment message after sent
            void Remove_past_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message);

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
            std::multiset<fh::cme::market::message::MdpMessage, Message_Compare> m_datas;
            fh::core::zmq::ZmqSender m_org_sender;
            fh::core::zmq::ZmqSender m_book_sender;
            std::vector<fh::cme::market::message::MdpMessage> *m_definition_datas;
            std::vector<fh::cme::market::message::MdpMessage> *m_recovery_datas;
            fh::cme::market::BookManager m_book_manager;
            std::uint32_t m_recovery_first_seq;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatSaver);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_SAVER_H__
