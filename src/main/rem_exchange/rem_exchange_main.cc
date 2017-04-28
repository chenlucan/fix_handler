#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>


#include "core/assist/logger.h"
#include "rem/exchange/rem_exchange_application.h"

bool stop_all=false;
fh::rem::exchange::CRemExchangeApp *pRemExchangeApp=NULL;
fh::core::assist::Settings *pFileConfig=NULL;

void onsignal(int sig)
{
    switch (sig)
    {
        case SIGINT:
            stop_all = true;
            break;

        case SIGTRAP:
            //printf("\r\n SIGTRAP\n");
            break;

        case SIGPIPE:
            //printf("\r\n SIGPIPE\n");
            break;
    }

    return;
}

int set_SignalProc()
{
    signal(SIGINT,  &onsignal);
    signal(SIGTRAP, &onsignal);
    signal(SIGPIPE, &onsignal);
    return 0;
}

int main_loop()
{
        while(!stop_all)
       {
           LOG_INFO("exchange order runing");
	    sleep(10);	 
       }
	return 0;
}

int main(int argc, char* argv[])
{

     set_SignalProc();
     LOG_INFO("exchange main() start");

     //∂¡»°≈‰÷√Œƒº˛
     std::string FileConfigstr= "rem_config.ini";
  
     LOG_INFO("FileConfigstr : ",FileConfigstr.c_str());
     pRemExchangeApp = new fh::rem::exchange::CRemExchangeApp(FileConfigstr);
     std::vector<::pb::ems::Order> init_orders;	
     init_orders.clear();     	 
     std::vector<::pb::dms::Contract> contracts;	 
     contracts.clear();	 
     pRemExchangeApp->Initialize(contracts);
     if(!pRemExchangeApp->Start(init_orders))
      {
          LOG_ERROR("FemasExchangeApp start  Error!\n");
	   pRemExchangeApp->Stop();	 
	   delete pRemExchangeApp;
	   return 0;	  
      }
      
     //::pb::ems::Order morder;
     pFileConfig = new fh::core::assist::Settings(FileConfigstr);
	 
     main_loop();
     pRemExchangeApp->Stop();	 
     delete pRemExchangeApp;
     delete pFileConfig;
     LOG_INFO("exchange main stop");	  
     return 0;	 

}

