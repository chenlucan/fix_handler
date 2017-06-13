
#ifndef __FH_CME_MARKET_CME_DATA_H__
#define __FH_CME_MARKET_CME_DATA_H__

#include <set>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include "cme/market/message/mdp_message.h"
#include "cme/market/recovery_saver.h"

namespace fh
{
namespace cme
{
namespace market
{
	class CmeData
	{
	private:
		struct Message_Compare
		{
			bool operator() (const fh::cme::market::message::MdpMessage &a, const fh::cme::market::message::MdpMessage &b)
			{
				return a.packet_seq_num() < b.packet_seq_num();
			}
		};	
		
	public:
		explicit CmeData();
		virtual ~CmeData();
		void Insert_increment_data(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages);
		std::pair<std::multiset<fh::cme::market::message::MdpMessage>::iterator, bool> Pick_next_message();
		std::multiset<fh::cme::market::message::MdpMessage, fh::cme::market::CmeData::Message_Compare> *Get_increment_data();
		std::vector<fh::cme::market::message::MdpMessage> *Get_definition_data();
		std::vector<fh::cme::market::message::MdpMessage> *Get_recovery_data();

		void Remove_increment_data(std::multiset<fh::cme::market::message::MdpMessage>::iterator message);
		std::uint32_t Get_increment_first_data_seq();
		std::uint32_t Get_increment_data_count();

		fh::cme::market::RecoverySaver * Get_definition_saver();
		fh::cme::market::RecoverySaver * Get_recovery_saver();

		void Stop();	

		
	private:
		std::uint32_t m_last_seq;
		std::set<std::uint32_t> m_unreceived_seqs;
		std::mutex m_increment_mutex;	
		std::multiset<fh::cme::market::message::MdpMessage, Message_Compare> m_increment_datas;
		fh::cme::market::RecoverySaver *m_definition_saver;
		fh::cme::market::RecoverySaver *m_recovery_saver;

		std::atomic_bool m_is_stopped;
		std::condition_variable m_condition_variable;

	private:
            DISALLOW_COPY_AND_ASSIGN(CmeData);
	};
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_CME_DATA_H__
