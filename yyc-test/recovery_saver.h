
#ifndef __RECOVERY_SAVER_H__
#define __RECOVERY_SAVER_H__

#include "global.h"
#include "mdp_message.h"

namespace rczg
{
	// save recovery datas(market recovery, instrument recovery) when later joiner startup
	class RecoverySaver
	{
		public:
			explicit RecoverySaver(bool is_definition);
			virtual ~RecoverySaver();

		public:
			void Process_recovery_packet(std::function<void()> callback, char *buffer, const size_t data_length);
			std::vector<rczg::MdpMessage> *Get_data();

		private:
			bool Save_packet(char *buffer, const size_t data_length, const std::uint32_t packet_seq_num);

		private:
			bool m_is_definition;
			std::uint32_t m_current_sequence;
			std::uint32_t m_total_number;
			std::vector<rczg::MdpMessage> m_recovery_datas;
			std::mutex m_mutex;

		private:
			DISALLOW_COPY_AND_ASSIGN(RecoverySaver);
	};
}

#endif // __RECOVERY_SAVER_H__
