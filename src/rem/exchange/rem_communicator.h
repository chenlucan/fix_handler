#ifndef __FH_REM_EXCHANGE_REM_COMMNUICATOR_H__
#define __FH_REM_EXCHANGE_REM_COMMNUICATOR_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include "EesTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"
#include "RemEESTraderApiManger.h"

namespace fh
{
namespace rem
{
namespace exchange
{


    class CRemCommunicator : public core::exchange::ExchangeI
    {
          public:
		   CRemCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file);
                 virtual ~CRemCommunicator();
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

		 bool SendQuerySymbolList();		

				 
	  public:			 
                 CEESTraderApiManger* m_pEESTraderApiManger;
                 fh::core::assist::Settings *m_pFileConfig;
		   EESTraderApi *m_pUserApi;
		   ////secs
		   int m_itimeout;
		   int m_ReqId;
		   
  

	  private:
                 core::exchange::ExchangeListenerI *m_strategy;
				 
		   std::vector<::pb::ems::Order> m_init_orders;		 

	  private:
	  	
                 DISALLOW_COPY_AND_ASSIGN(CRemCommunicator);			 

			
    };











}
}
}









#endif