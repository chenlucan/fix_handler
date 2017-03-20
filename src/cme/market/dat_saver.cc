
#include <boost/iterator/counting_iterator.hpp>
#include "cme/market/dat_saver.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{
    DatSaver::DatSaver(const std::string &org_save_url, fh::cme::market::BookSender *book_sender)
    : m_last_seq(0), m_unreceived_seqs(), m_mutex(), m_datas(),
      m_org_sender(org_save_url),
      m_definition_datas(nullptr), m_recovery_datas(nullptr), m_book_manager(book_sender),
      m_recovery_first_seq(0)
    {
        // noop
    }

    void DatSaver::Set_later_join(bool is_lj)
    {
        if(is_lj)
        {
            // use max value as initial value
            m_last_seq = std::numeric_limits<std::uint32_t>::max();
        }
        else
        {
            m_last_seq = 0;
        }
    }

    // 把接受到的行情数据保存到内存，如果有 GAP 那么就记录下丢失的序号
    void DatSaver::Insert_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

        if(m_last_seq == std::numeric_limits<std::uint32_t>::max())
        {
            // 说明这次来的是 later joiner 的第一条数据，更新已保存最大序列号
            m_last_seq = packet_seq_num;
        }
        else if(packet_seq_num < m_last_seq)
        {
            // 说明这次来的是以前丢失的数据，从丢失列表中去掉它（如果有的话）
            m_unreceived_seqs.erase(packet_seq_num);
        }
        else
        {
            // 说明这次来的是后续数据，如果有 GAP 就记录下丢失的序号，同时更新已保存最大序列号
            m_unreceived_seqs.insert(
                boost::counting_iterator<std::uint32_t>(m_last_seq + 1), 
                boost::counting_iterator<std::uint32_t>(packet_seq_num));
            m_last_seq = packet_seq_num;
        }
    }

    // set received definition messages
    void DatSaver::Set_definition_data(std::vector<fh::cme::market::message::MdpMessage> *definition_datas)
    {
        m_definition_datas = definition_datas;
        m_book_manager.Set_definition_data(definition_datas);
    }

    // set received recovery messages
    void DatSaver::Set_recovery_data(std::vector<fh::cme::market::message::MdpMessage> *recovery_datas)
    {
        m_recovery_datas = recovery_datas;
        m_book_manager.Set_recovery_data(recovery_datas);

        if(!recovery_datas->empty())
        {
            m_recovery_first_seq = recovery_datas->front().last_msg_seq_num_processed();
        }
    }

    // process the messages in memory and save to zeroqueue
    void DatSaver::Start_save()
    {
        // first, send definition messages if exist
        this->Send_definition_messages();
        // first, send recovery messages if exist
        this->Send_recovery_messages();

        while(true)
        {
           auto message = Pick_next_message();
           if(message != m_datas.end())
           {
               bool should_send_now = this->Convert_message(message);
               if(should_send_now)
               {
                   this->Send_message(message->Serialize());
               }

               Remove_past_message(message);
           }
           else
           {
               std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
           }
       }
    }

    // pick first message to serialize and remove it from memory
    std::multiset<fh::cme::market::message::MdpMessage>::iterator DatSaver::Pick_next_message()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(m_datas.empty())
        {
            // 暂无数据
            return m_datas.end();
        }

        auto message = m_datas.begin();
        std::uint32_t first_unreceived = m_unreceived_seqs.empty() ? 0 : *m_unreceived_seqs.begin();

        if(first_unreceived != 0 && message->packet_seq_num() >= first_unreceived)
        {
            // 下一个 seq 的数据缺失，需要等待
            LOG_DEBUG("wait for seq=", first_unreceived);
            return m_datas.end();
        }

        return message;
    }

    void DatSaver::Send_definition_messages()
    {
        if(m_definition_datas == nullptr)
        {
            return;
        }

        std::for_each(m_definition_datas->cbegin(), m_definition_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m.Serialize()); });

        LOG_INFO("all definition message sent: ", m_definition_datas->size());
    }

    void DatSaver::Send_recovery_messages()
    {
        if(m_recovery_datas == nullptr)
        {
            return;
        }

        std::for_each(m_recovery_datas->cbegin(), m_recovery_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m.Serialize()); });

        LOG_INFO("all recovery message sent: ", m_recovery_datas->size());
    }

    bool DatSaver::Convert_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message)
    {
        if(message->packet_seq_num() <= m_recovery_first_seq)
        {
            // drop the message before first recovery's message's 369-LastMsgSeqNumProcessed
            LOG_INFO("drop message: seq=", message->packet_seq_num());
            return false;
        }

        // convert the message to books and send to zeromq for book
        m_book_manager.Parse_to_send(*message);

        // now send received messages to zeromq for save to db
        return true;
    }

    void DatSaver::Send_message(const std::string &serialized_message)
    {
        static fh::core::assist::TimeMeasurer t;

        // send to zeroqueue
        m_org_sender.Send(serialized_message);

        //        8 bytes : m_received_time
        //        2 bytes : m_packet_length
        //        4 bytes : m_packet_seq_num
        //        8 bytes : m_packet_sending_time
        //        2 bytes : m_message_length
        std::uint32_t packet_seq_num = *(std::uint32_t *)(serialized_message.data() + 8 + 2);
        std::uint16_t message_length = *(std::uint16_t *)(serialized_message.data() + 8 + 2 + 4 + 8);
        LOG_INFO("sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, send size=", serialized_message.size(), " seq=", packet_seq_num, " message size=", message_length);
    }

    void DatSaver::Remove_past_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_datas.erase(message);
    }

    DatSaver::~DatSaver()
    {
        // noop
    }
} // namespace market
} // namespace cme
} // namespace fh
