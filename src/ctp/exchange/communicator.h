#ifndef __FH_CTP_EXCHANGE_COMMUNICATOR_H__
#define __FH_CTP_EXCHANGE_COMMUNICATOR_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include "ThostFtdcTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"
#include <atomic>
#include "ctp_trader_spi.h"
#include <boost/make_shared.hpp>
#include "AccountID.h"

namespace fh
{
namespace ctp
{
namespace exchange
{    
	class CCtpCommunicator : public core::exchange::ExchangeI
	{
		public:
			CCtpCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file);
			virtual ~CCtpCommunicator();
		public:
			// implement of ExchangeI
			bool Start(const std::vector<::pb::ems::Order> &init_orders) override;
			// implement of ExchangeI
			void Stop() override;

			public:
			// implement of ExchangeI
			void Initialize(std::vector<::pb::dms::Contract> contracts) override;
			// implement of ExchangeI
			void Add(const ::pb::ems::Order& order) override;
			// implement of ExchangeI
			void Change(const ::pb::ems::Order& order) override;
			// implement of ExchangeI
			void Delete(const ::pb::ems::Order& order) override;
			// implement of ExchangeI
			void Query(const ::pb::ems::Order& order) override;
			// implement of ExchangeI
			void Query_mass(const char *data, size_t size) override;
			// implement of ExchangeI
			void Delete_mass(const char *data, size_t size) override;

			void SendReqQryTrade(const std::vector<::pb::ems::Order> &init_orders);
			void SendReqQryInvestorPosition(const std::vector<::pb::ems::Order> &init_orders);
			public:	
			fh::core::assist::Settings *m_pFileConfig;	

		private:
			std::shared_ptr<fh::ctp::exchange::CCtpTraderSpi> m_trader;
			core::exchange::ExchangeListenerI *m_strategy;
			std::vector<::pb::ems::Order> m_init_orders;	

		public:		   
			int m_itimeout;

		private:
			DISALLOW_COPY_AND_ASSIGN(CCtpCommunicator);			 


	};



}
}
}









#endif