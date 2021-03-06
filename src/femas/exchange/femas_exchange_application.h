#ifndef __FH_FEMA_EXCHANGE_FEMAS_EXCHANGE_APPLICATION_H__
#define __FH_FEMA_EXCHANGE_FEMAS_EXCHANGE_APPLICATION_H__

#include <string>
#include "core/global.h"
#include "femas/exchange/communicator.h"
#include "core/strategy/strategy_communicator.h"

namespace fh
{
namespace femas
{
namespace exchange
{

    class CFemasExchangeApp
    {

        public:
                    explicit CFemasExchangeApp(const std::string &app_setting_file = FEMAS_CONFIG_INI);
                    virtual ~CFemasExchangeApp();

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
                        int ret = m_pCFemasGlobexCommunicator->m_pUstpFtdcTraderManger->MaxOrderLocalID;
			  m_pCFemasGlobexCommunicator->m_pUstpFtdcTraderManger->MaxOrderLocalID++;
			  return ret;
		      }
		      //code test end	  

	  public:
	  	      CFemasCommunicator* m_pCFemasGlobexCommunicator;
		      fh::core::strategy::StrategyCommunicator *m_strategy;
		      fh::core::assist::Settings *pFileConfig;	  

	  private:
            DISALLOW_COPY_AND_ASSIGN(CFemasExchangeApp);		  
	 	

    };




}
}
}









#endif