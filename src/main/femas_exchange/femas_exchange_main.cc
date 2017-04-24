#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>



#include "femas/exchange/femas_exchange_application.h"

bool stop_all=false;

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
           printf("========================exchange runing=========================\n");
	    sleep(10);	 
       }
	return 0;
}


int main(int argc, char* argv[])
{

     set_SignalProc();
     printf("exchange main() start\n");

     //∂¡»°≈‰÷√Œƒº˛
     std::string FileConfigstr= "femas_config.ini";
  
     printf("FileConfigstr : %s \n",FileConfigstr.c_str());
     fh::femas::exchange::CFemasExchangeApp *pFemasExchangeApp = new fh::femas::exchange::CFemasExchangeApp(FileConfigstr);
     std::vector<::pb::ems::Order> init_orders;	
     init_orders.clear();     	 
     std::vector<::pb::dms::Contract> contracts;	 
     contracts.clear();	 
     pFemasExchangeApp->Initialize(contracts);
     if(!pFemasExchangeApp->Start(init_orders))
      {
          printf("FemasExchangeApp start  Error!\n");
	   pFemasExchangeApp->Stop();	 
	   delete pFemasExchangeApp;
	   return 0;	  
      }	 
     ::pb::ems::Order morder;
     fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
     std::string UserId = pFileConfig->Get("femas-user.UserID");	
	 
     morder.set_client_order_id(std::to_string(pFemasExchangeApp->GetMaxOrderLocalID()));
     morder.set_account(UserId);	
     morder.set_contract("IF1306");	 
     morder.set_buy_sell(pb::ems::BuySell::BS_Buy);
     morder.set_price(std::to_string(398));
     morder.set_quantity(1000);	 
     morder.set_tif(pb::ems::TimeInForce::TIF_GFD);
     morder.set_order_type(pb::ems::OrderType::OT_Limit);

	 
     pFemasExchangeApp->Add(morder);		 
     main_loop();
     pFemasExchangeApp->Stop();	 
     delete pFemasExchangeApp;
     printf("exchange main stop.\n");	  
     return 0;	 

}	 