#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>

#include "rem/market/rem_market_manager.h"
#include "rem/market/rem_market_application.h"
#include "core/assist/logger.h"


bool stop_all=false;
fh::rem::market::CRemMarketApp *pSRemMarketApp = NULL;

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
        while(!stop_all && !(pSRemMarketApp->GetReatart()))
       {
           LOG_INFO("RemMarket  runing  main_loop");
	    sleep(10);	 
       }
	return 0;
}

int main(int argc, char* argv[])
{

     set_SignalProc();
     LOG_INFO("main() start");
     //read my cfg
     std::string FileConfigstr= "rem_config.ini";
  
     LOG_INFO("FileConfigstr : ",FileConfigstr.c_str());
     std::vector<std::string> Depthstruments;
     std::vector<std::string> Subminstruments;	 
	     
      fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
      std::string save_url_f = pFileConfig->Get("zeromq.org_url");
      std::string save_url_s = pFileConfig->Get("zeromq.book_url");

      while(!stop_all )
      {
          fh::rem::market::CRemMarketApp *pRemMarletApp = new fh::rem::market::CRemMarketApp(save_url_f,save_url_s);	
          pRemMarletApp->SetFileConfigData(FileConfigstr);	  
      
          Depthstruments.clear();
          Subminstruments.clear();
	   Subminstruments.push_back("*");	  

          pRemMarletApp->Initialize(Subminstruments);
          
	   //pRemMarletApp->Subscribe(Subminstruments);	   
	  
          if(!pRemMarletApp->Start())
          {
              LOG_ERROR("FemasMarletApp start  Error!");
	       pRemMarletApp->Stop();	 
	       delete pRemMarletApp;
	       return 0;	  
          }	  

          pSRemMarketApp = pRemMarletApp;

          main_loop();
	  
          pRemMarletApp->Stop();	 
	   delete pRemMarletApp;	  
      }
      
      //Subminstruments.push_back("IF1706");	  
      //Subminstruments.push_back("IF1705");		   
      
      delete pFileConfig;
      

      LOG_INFO("rem-market pross stop!");
      return 0;
}
