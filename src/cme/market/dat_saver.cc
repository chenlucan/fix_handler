
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
    DatSaver::DatSaver(fh::core::market::MarketListenerI *book_sender)
    : m_last_seq(std::numeric_limits<std::uint32_t>::max()), m_unreceived_seqs(),
      m_mutex(), m_recovery_mutex(), m_datas(),
      m_definition_datas(nullptr), m_recovery_datas(nullptr),
      m_book_sender(book_sender), m_book_manager(book_sender),
      m_recovery_first_seq(0), m_is_stopped(false)
    {
        // noop
    }

    // 把接受到的行情数据保存到内存，如果有 GAP 那么就记录下丢失的序号
    void DatSaver::Insert_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

        if(m_last_seq == std::numeric_limits<std::uint32_t>::max())
        {
            // 说明这次来的是第一条数据，更新已保存最大序列号
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

    // set received definition,recovery messages
    void DatSaver::Set_recovery_data(
            std::vector<fh::cme::market::message::MdpMessage> *definition_datas,
            std::vector<fh::cme::market::message::MdpMessage> *recovery_datas)
    {
        std::lock_guard<std::mutex> lock(m_recovery_mutex);
        m_definition_datas = definition_datas;
        m_recovery_datas = recovery_datas;
    }

    // process the messages in memory and save to zeroqueue
    void DatSaver::Start_save()
    {
        std::uint32_t first_seq = Get_first_data_seq();
        LOG_INFO("first message received, seq = ", first_seq);

        if(first_seq == 1)
        {
            // 说明是从周末就开始启动了，这样才能接受到第 1 条行情数据，
            // 此时无需处理恢复数据，直接从第一条行情数据开始处理即可
        }
        else
        {
            // 说明是周中才启动的，此时需要从恢复数据开始处理
            // 这里会等待接受到所有的恢复数据，然后处理之（解析，发送）
            Process_recovery_data();
        }

        while(!m_is_stopped)
        {
           auto message = Pick_next_message();  // 返回：{数据内容, 有无数据}
           if(message.second)
           {
               bool should_send_now = this->Convert_message(message.first);
               if(should_send_now)
               {
                   this->Send_message(*message.first);
               }

               Remove_past_message(message.first);
           }
           else
           {
               std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
           }
       }
    }

    // 获取到第一条数据的 sequence number（如果没有数据，则一直等待）
    std::uint32_t DatSaver::Get_first_data_seq()
    {
        while(true)
        {
            { // for release mutex
                std::lock_guard<std::mutex> lock(m_mutex);
                if(!m_datas.empty())
                {
                    return m_datas.cbegin()->packet_seq_num();
                }
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        }
    }

    // 处理恢复数据（如果没有恢复数据，则一直等待）
    void DatSaver::Process_recovery_data()
    {
        while(true)
        {
            { // for release mutex
                std::lock_guard<std::mutex> lock(m_recovery_mutex);
                if(m_recovery_datas != nullptr)
                {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        }

        LOG_INFO("recovery messages received.");

        m_book_manager.Set_definition_data(m_definition_datas);
        m_book_manager.Set_recovery_data(m_recovery_datas);

        m_recovery_first_seq = m_recovery_datas->front().last_msg_seq_num_processed();

        // 发送接受到的恢复数据
        this->Send_definition_messages();
        this->Send_recovery_messages();
    }

    // pick first message to serialize and remove it from memory
    std::pair<std::multiset<fh::cme::market::message::MdpMessage>::iterator, bool> DatSaver::Pick_next_message()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(m_datas.empty())
        {
            // 暂无数据
            return {m_datas.end(), false};
        }

        auto message = m_datas.begin();
        std::uint32_t first_unreceived = m_unreceived_seqs.empty() ? 0 : *m_unreceived_seqs.begin();

        if(first_unreceived != 0 && message->packet_seq_num() >= first_unreceived)
        {
            // 下一个 seq 的数据缺失，需要等待
            LOG_DEBUG("wait for seq=", first_unreceived);
            return {m_datas.end(), false};
        }

        return {message, true};
    }

    void DatSaver::Send_definition_messages()
    {
        if(m_definition_datas == nullptr)
        {
            LOG_INFO("no definition message.");
            return;
        }

        std::for_each(m_definition_datas->cbegin(), m_definition_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m); });

        LOG_INFO("{DB}all definition message sent: ", m_definition_datas->size());
    }

    void DatSaver::Send_recovery_messages()
    {
        if(m_recovery_datas == nullptr)
        {
            LOG_INFO("no recovery message.");
            return;
        }

        std::for_each(m_recovery_datas->cbegin(), m_recovery_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m); });

        LOG_INFO("{DB}all recovery message sent: ", m_recovery_datas->size());
    }

    bool DatSaver::Convert_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message)
    {
        LOG_INFO("{BS}process message: seq=", message->packet_seq_num(), ", type=", message->message_type());
        if(message->packet_seq_num() <= m_recovery_first_seq)
        {
            // drop the message before first recovery's message's 369-LastMsgSeqNumProcessed
            LOG_INFO("this message is before first recovery's message(", m_recovery_first_seq, "), discard it");
            return false;
        }

        // convert the message to books and send to zeromq for book
        m_book_manager.Parse_to_send(*message);

        // now send received messages to zeromq for save to db
        LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
        return true;
    }

    void DatSaver::Send_message(const fh::cme::market::message::MdpMessage &message)
    {
        static fh::core::assist::TimeMeasurer t;

        // send to db
        std::string json = message.Serialize();
        if(json == "")  // 返回是空说明是不需要保存的消息
        {
            LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", message.packet_seq_num(), " type=", message.message_type());
        }
        else
        {
            m_book_sender->OnOrginalMessage(json);
            LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", message.packet_seq_num(), " type=", message.message_type());
        }
    }

    void DatSaver::Remove_past_message(std::multiset<fh::cme::market::message::MdpMessage>::iterator message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_datas.erase(message);
    }

    void DatSaver::Stop()
    {
        m_is_stopped = true;
    }

    DatSaver::~DatSaver()
    {
        // noop
    }
} // namespace market
} // namespace cme
} // namespace fh
