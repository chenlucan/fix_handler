#ifndef __FH_FEMA_EXCHANGE_APP_H__
#define __FH_FEMA_EXCHANGE_APP_H__

#include <string>
#include "core/global.h"
#include "femas/exchange/femas_globex_communicator.h"

namespace fh
{
namespace femas
{
namespace exchange
{

    class CFemasExchangeApp
    {

        public:
                    explicit CFemasExchangeApp(const std::string &app_setting_file = "femas_config.ini");
                    virtual ~CFemasExchangeApp();

	 public:

		      bool Start(const std::vector<::pb::ems::Order> &init_orders);
		      void Stop();

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

	  public:
	  	      CFemasGlobexCommunicator* m_pCFemasGlobexCommunicator;

	  private:
            DISALLOW_COPY_AND_ASSIGN(CFemasExchangeApp);		  
	 	

    };




}
}
}









#endif