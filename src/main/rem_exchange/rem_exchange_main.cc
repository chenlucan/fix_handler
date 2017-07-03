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

     //读取配置文件
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
     pFileConfig = new fh::core::assist::Settings(FileConfigstr);	
     LOG_INFO("exchange order begin");
     LOG_INFO("InstrumentID:");
     char userInstrumentID[100]={0};	
     scanf("%s", userInstrumentID);	

     LOG_INFO("LimitPrice:");
     float userLimitPrice=0;
     scanf("%f", &userLimitPrice);	   
		   
     LOG_INFO("Volume:");
     int userVolume=0;
     scanf("%d", &userVolume);	 

     std::string UserId = pFileConfig->Get("rem-user.LogID");	  
     ::pb::ems::Order morder;
     morder.set_client_order_id(std::to_string(pRemExchangeApp->GetMaxOrderLocalID()));
     morder.set_account(UserId);
     morder.set_contract(userInstrumentID);	 
     morder.set_buy_sell(pb::ems::BuySell::BS_Buy);
     morder.set_price(std::to_string(userLimitPrice));
     morder.set_quantity(userVolume);	 
     morder.set_tif(pb::ems::TimeInForce::TIF_GFD);
     morder.set_order_type(pb::ems::OrderType::OT_Limit);	 
     
     pRemExchangeApp->Add(morder);
  
	 
     main_loop();
     pRemExchangeApp->Stop();	 
     delete pRemExchangeApp;
     delete pFileConfig;
     LOG_INFO("exchange main stop");	  
     return 0;	 

}

