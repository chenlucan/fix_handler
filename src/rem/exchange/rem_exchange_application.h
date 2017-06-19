#ifndef __FH_REM_EXCHANGE_REM_EXCHANGE_APPLICATION_H__
#define __FH_REM_EXCHANGE_REM_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"
#include "rem/exchange/rem_communicator.h"
#include "core/strategy/strategy_communicator.h"

namespace fh
{
namespace rem
{
namespace exchange
{


    class CRemExchangeApp
    {

        public:
                    explicit CRemExchangeApp(const std::string &app_setting_file = "rem_config.ini");
                    virtual ~CRemExchangeApp();

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
                        int ret = m_pCRemGlobexCommunicator->m_pEESTraderApiManger->MaxOrderLocalID;
			  m_pCRemGlobexCommunicator->m_pEESTraderApiManger->MaxOrderLocalID++;
			  return ret;
		      }
		      //code test end	  

	  public:
	  	      CRemCommunicator* m_pCRemGlobexCommunicator;
		      fh::core::strategy::StrategyCommunicator *m_strategy;
		      fh::core::assist::Settings *pFileConfig;	  

	  private:
            DISALLOW_COPY_AND_ASSIGN(CRemExchangeApp);		  
	 	

    };













}
}
}









#endif
