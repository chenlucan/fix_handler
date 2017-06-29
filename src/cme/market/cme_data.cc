#include <boost/iterator/counting_iterator.hpp>

#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"

#include "cme/market/cme_data.h"

namespace fh
{
namespace cme
{
namespace market
{
    CmeData::CmeData()
    : m_last_seq(std::numeric_limits<std::uint32_t>::max()), m_unreceived_seqs(),
      m_increment_mutex(), m_increment_datas(), m_definition_saver(nullptr), m_recovery_saver(nullptr), 
      m_is_stopped(false), m_condition_variable()
    {
        m_definition_saver = new  fh::cme::market::RecoverySaver(true);
        m_recovery_saver = new  fh::cme::market::RecoverySaver(false);		
    }
	
    CmeData::~CmeData()
    {
        delete m_recovery_saver;
        delete m_definition_saver;
    }	

    // 把接受到的行情数据保存到内存，如果有 GAP 那么就记录下丢失的序号
    void CmeData::Insert_increment_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages)
    {
        std::unique_lock<std::mutex> lock(m_increment_mutex);

        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](fh::cme::market::message::MdpMessage &m){ m_increment_datas.insert(std::move(m)); });

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

        lock.unlock();
        m_condition_variable.notify_all();
    }
	
    // pick first message to serialize and remove it from memory
    std::pair<std::multiset<fh::cme::market::message::MdpMessage>::iterator, bool> CmeData::Pick_next_message()
    {
        while(!m_is_stopped)
        {
            std::unique_lock<std::mutex> lock(m_increment_mutex);
            if(m_increment_datas.empty())
            {
                // 暂无数据，需要等待
                fh::core::assist::TimeMeasurer t;
                LOG_DEBUG("===== [begin-1] Pick_next_message : wait =====");
                if(m_is_stopped)   // stop
                {
                    LOG_DEBUG("===== [end-1] Pick_next_message : exit =====");
                    break;
                }
                m_condition_variable.wait(lock);
                LOG_DEBUG("===== [end-1] Pick_next_message : wait [", t.Elapsed_nanoseconds(), "ns] =====");
            }
            else
            {
                auto message = m_increment_datas.begin();
                std::uint32_t first_unreceived = m_unreceived_seqs.empty() ? 0 : *m_unreceived_seqs.begin();

                if(first_unreceived != 0 && message->packet_seq_num() >= first_unreceived)
                {
                    // 下一个 seq 的数据缺失，需要等待
                    LOG_DEBUG("wait for seq=", first_unreceived);
                    fh::core::assist::TimeMeasurer t;
                    LOG_DEBUG("===== [begin-2] Pick_next_message : wait =====");
                    if(m_is_stopped)   // stop
                    {
                        LOG_DEBUG("===== [end-2] Pick_next_message : exit =====");
                        break;
                    }
                    m_condition_variable.wait(lock);
                    LOG_DEBUG("===== [end-2] Pick_next_message : wait [", t.Elapsed_nanoseconds(), "ns] =====");
                }
                else
                {
                    return {message, true};
                }                
            }
        }

        return {m_increment_datas.end(), false};//m_is_stopped=true
    }	
	
    std::vector<fh::cme::market::message::MdpMessage> *CmeData::Get_definition_data()
    {
        return m_definition_saver->Get_data();
    }	
	
    std::vector<fh::cme::market::message::MdpMessage> *CmeData::Get_recovery_data()
    {
        return m_recovery_saver->Get_data();
    }

    std::multiset<fh::cme::market::message::MdpMessage, fh::cme::market::CmeData::Message_Compare> *CmeData::Get_increment_data()	
    {
	std::unique_lock<std::mutex> lock(m_increment_mutex);
	return &m_increment_datas;
    }

    void CmeData::Remove_increment_data(std::multiset<fh::cme::market::message::MdpMessage>::iterator message)	
    {
	std::unique_lock<std::mutex> lock(m_increment_mutex);
	m_increment_datas.erase(message);
    }

    std::uint32_t CmeData::Get_increment_first_data_seq()
    {
        while(!m_is_stopped)
        {
            std::unique_lock<std::mutex> lock(m_increment_mutex);
            if(!m_increment_datas.empty())
            {
                return m_increment_datas.cbegin()->packet_seq_num();
            }
            else
            { 
                // no data, need to wait
                fh::core::assist::TimeMeasurer t;
                LOG_DEBUG("===== [begin-2] Get_increment_first_data_seq : wait =====");
                if(m_is_stopped)   // stop
                {
                    LOG_DEBUG("===== [end-2] Get_increment_first_data_seq : exit =====");
                    break;
                }
                m_condition_variable.wait(lock);
               // std::cv_status::timeout
                //m_condition_variable.wait_for(lock, std::chrono::seconds(1));
                LOG_DEBUG("===== [end-2] Get_increment_first_data_seq : wait [", t.Elapsed_nanoseconds(), "ns] =====");
            }
        }        

        LOG_DEBUG("===== [return] Get_increment_first_data_seq =====");
        return 0;
    }

    std::uint32_t CmeData::Get_increment_data_count()
    {
        std::unique_lock<std::mutex> lock(m_increment_mutex);
        return m_increment_datas.size();
    }

    fh::cme::market::RecoverySaver * CmeData::Get_definition_saver()
    {
        return m_definition_saver;
    }

    fh::cme::market::RecoverySaver * CmeData::Get_recovery_saver()
    {
        return m_recovery_saver;
    }

    void CmeData::Stop()
    {
        LOG_DEBUG("===== CmeData::Stop() =====");
        m_is_stopped = true;
        m_condition_variable.notify_all();
    }
} // namespace market
} // namespace cme
} // namespace fh
