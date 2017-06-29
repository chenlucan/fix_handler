
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
    DatSaver::DatSaver(fh::cme::market::CmeData *cme_data, fh::core::market::MarketListenerI *book_sender)
    : m_pData(cme_data),
      m_book_sender(book_sender), m_book_manager(book_sender),
      m_recovery_first_seq(0), m_is_stopped(false)
    {
        // noop
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
        else if(first_seq == 0)
        {
            return;  // m_is_stopped=true
        }
        else
        {
            // 说明是周中才启动的，此时需要从恢复数据开始处理
            // 这里会等待接受到所有的恢复数据，然后处理之（解析，发送）
            Process_recovery_data();
        }
        while(!m_is_stopped)
        {
            auto message = m_pData->Pick_next_message();  // 返回：{数据内容, 有无数据}
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
                break;
            }
       }
    }

    // 获取到第一条数据的 sequence number（如果没有数据，则一直等待）
    std::uint32_t DatSaver::Get_first_data_seq()
    {
        LOG_DEBUG("===== DatSaver::Get_first_data_seq =====");
        std::uint32_t first_seq_num = m_pData->Get_increment_first_data_seq(); 
        return first_seq_num;
    }

    // 处理恢复数据（如果没有恢复数据，则一直等待）
    void DatSaver::Process_recovery_data()
    {
        while(!m_is_stopped)
        {
            { 
                std::vector<fh::cme::market::message::MdpMessage> *recovery_datas = m_pData->Get_recovery_data();
                if( (recovery_datas!=nullptr) && (!recovery_datas->empty()) )
                {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        }

        // 说明被终止了
        if(m_is_stopped) return;

        LOG_INFO("recovery messages received.");

        m_book_manager.Set_definition_data(m_pData->Get_definition_data());
        m_book_manager.Set_recovery_data(m_pData->Get_recovery_data());

        std::vector<fh::cme::market::message::MdpMessage> *recovery_datas = m_pData->Get_recovery_data();
        if( (recovery_datas!=nullptr) && (!recovery_datas->empty()) )
        {
            m_recovery_first_seq = recovery_datas->front().last_msg_seq_num_processed();
        }
        else
        {
            LOG_ERROR("XXXXXXXXXXXXXXXX recovery_datas is nullptr or empty !XXXXXXXXXXXXXXXX");
        }

        // 发送接受到的恢复数据
        this->Send_definition_messages();
        this->Send_recovery_messages();
    }

    void DatSaver::Send_definition_messages()
    {
        std::vector<fh::cme::market::message::MdpMessage> *pDefinition_datas = m_pData->Get_definition_data();
        if( (pDefinition_datas==nullptr) || (pDefinition_datas->empty()) )
        {
            LOG_INFO("no definition message.");
            return;
        }

        std::for_each(pDefinition_datas->cbegin(), pDefinition_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m); });

        LOG_INFO("{DB}all definition message sent: ", pDefinition_datas->size());
    }

    void DatSaver::Send_recovery_messages()
    {
        std::vector<fh::cme::market::message::MdpMessage> *pRecovery_datas = m_pData->Get_recovery_data();
        if( (pRecovery_datas==nullptr) || (pRecovery_datas->empty()) )
        {
            LOG_INFO("no recovery message.");
            return;
        }

        std::for_each(pRecovery_datas->cbegin(), pRecovery_datas->cend(),
                [this](const fh::cme::market::message::MdpMessage &m){ this->Send_message(m); });

        LOG_INFO("{DB}all recovery message sent: ", pRecovery_datas->size());
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
        LOG_DEBUG("===== DatSaver::Send_message =====");

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
        m_pData->Remove_increment_data(message);
    }

    void DatSaver::Stop()
    {
        LOG_DEBUG("===== DatSaver::Stop() =====");
        m_is_stopped = true;
        m_pData->Stop();
    }

    // mut_test
    std::uint32_t DatSaver::Get_data_count()
    {
        return m_pData->Get_increment_data_count();
    }

    fh::cme::market::CmeData * DatSaver::Get_cme_data()
    {
        return m_pData;
    }

    DatSaver::~DatSaver()
    {
        // noop
    }
} // namespace market
} // namespace cme
} // namespace fh
