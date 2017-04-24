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
     std::vector<::pb::ems::Order> insertOrders;
     insertOrders.clear();	 
     //pFemasExchangeApp->Add(insertOrders);		 
     main_loop();
     pFemasExchangeApp->Stop();	 
     delete pFemasExchangeApp;
     printf("exchange main stop.\n");	  
     return 0;	 

}	 