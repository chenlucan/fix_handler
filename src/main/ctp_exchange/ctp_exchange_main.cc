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
#include "ctp/exchange/ctp_exchange_application.h"

bool stop_all=false;
fh::ctp::exchange::CCtpExchangeApp *pCtpExchangeApp=NULL;
::pb::ems::Order morder;
fh::core::assist::Settings *pFileConfig=NULL;

void handler(int sig) 
{
#ifdef __unix__
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
#endif
}

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
  	   
           LOG_INFO("exchange runing main_loop");
	    	
	    sleep(10);	 
       }
	return 0;
}


int main(int argc, char* argv[])
{

     set_SignalProc();
     LOG_INFO("exchange main() start");

     //∂¡»°≈‰÷√Œƒº˛
     std::string FileConfigstr= "ctp_config.ini";
  
     LOG_INFO("FileConfigstr : ",FileConfigstr.c_str());
     pCtpExchangeApp = new fh::ctp::exchange::CCtpExchangeApp(FileConfigstr);
     std::vector<::pb::ems::Order> init_orders;	
     init_orders.clear();     	 
     std::vector<::pb::dms::Contract> contracts;	 
     contracts.clear();	 
	 pb::dms::Contract contract;
	 contract.set_name("cs1801");
	 contracts.push_back(contract);
     pCtpExchangeApp->Initialize(contracts);
	 
     if(!pCtpExchangeApp->Start(init_orders))
      {
          LOG_ERROR("CtpExchangeApp start  Error!\n");
	   pCtpExchangeApp->Stop();	 
	   delete pCtpExchangeApp;
	   return 0;	  
      }	 
     ::pb::ems::Order morder;
     pFileConfig = new fh::core::assist::Settings(FileConfigstr);

		LOG_INFO("exchange order begin");
	    LOG_INFO("InstrumentID:");
	    char userInstrumentID[100]="cs1801";	
		//"cs1801";
	 //   scanf("%s", userInstrumentID);	

           LOG_INFO("LimitPrice:");
		   float userLimitPrice=38850;
          // scanf("%f", &userLimitPrice);	   
		   
	    LOG_INFO("Volume:");
	    int userVolume= 1;
       //    scanf("%d", &userVolume);

           std::string UserId = pFileConfig->Get("ctp-user.UserID");	
	 
           morder.set_client_order_id(std::to_string(pCtpExchangeApp->GetMaxOrderLocalID()));
           morder.set_account(UserId);	
           morder.set_contract(userInstrumentID);	 
		 morder.set_contract("cs1801");
           morder.set_buy_sell(pb::ems::BuySell::BS_Buy);
           morder.set_price(std::to_string(userLimitPrice));
           morder.set_quantity(userVolume);	 
           morder.set_tif(pb::ems::TimeInForce::TIF_GFD);
           morder.set_order_type(pb::ems::OrderType::OT_Limit);
		   

	    pCtpExchangeApp->Add(morder);


		 
     main_loop();
     pCtpExchangeApp->Stop();	 
     delete pCtpExchangeApp;
     delete pFileConfig;
     LOG_INFO("exchange main stop");	  
     return 0;	 

}	 