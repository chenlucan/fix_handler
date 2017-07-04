#ifndef __FH_CTP_EXCHANGE_CTP_EXCHANGE_APPLICATION_H__
#define __FH_CTP_EXCHANGE_CTP_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"
#include "ctp/exchange/ctpcommunicator.h"
#include "core/strategy/strategy_communicator.h"

namespace fh
{
namespace ctp
{
namespace exchange
{

	class CCtpExchangeApp
	{

		public:
			explicit CCtpExchangeApp(const std::string &app_setting_file = "ctp_config.ini");
			virtual ~CCtpExchangeApp();

		public:

			bool Start(const std::vector<::pb::ems::Order> &init_orders);
			void Stop();

			//code test begin
			void Initialize(std::vector<::pb::dms::Contract> contracts);

			void Add(const ::pb::ems::Order& order);

			void Change(const ::pb::ems::Order& order);

			void Delete(const ::pb::ems::Order& order);

			void Query(const ::pb::ems::Order& order);

			void Query_mass(const char *data, size_t size);

			void Delete_mass(const char *data, size_t size);

			int GetMaxOrderLocalID()
			{
			//	int ret = m_pCtpGlobexCommunicator->m_pUserSpi->m_order_ref;
			//	m_pCtpGlobexCommunicator->m_pUserSpi->m_order_ref++;
			//	return ret;
			return 0;
			}
			//code test end	  

		public:
			CCtpCommunicator* m_pCtpGlobexCommunicator;
			fh::core::strategy::StrategyCommunicator *m_strategy;
			fh::core::assist::Settings *pFileConfig;	  

		private:
			DISALLOW_COPY_AND_ASSIGN(CCtpExchangeApp);		  


	};




}
}
}









#endif