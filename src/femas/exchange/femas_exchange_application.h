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

		      bool Start();
		      void Stop();


	  public:
	  	      CFemasGlobexCommunicator* m_pCFemasGlobexCommunicator;

	  private:
            DISALLOW_COPY_AND_ASSIGN(CFemasExchangeApp);		  
	 	

    };




}
}
}









#endif