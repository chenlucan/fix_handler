

#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>

#include "femas/market/femas_market_manager.h"
#include "femas/market/femas_market_application.h"


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
           printf("========================runing=========================\n");
	    sleep(10);	 
       }
	return 0;
}

int main(int argc, char* argv[])
{

     //signal(SIGSEGV, handler);
     set_SignalProc();
     printf("main() start\n");
     //¶ÁÈ¡ÅäÖÃÎÄ¼þ
     std::string FileConfigstr= "femas_config.ini";
  
     printf("FileConfigstr : %s \n",FileConfigstr.c_str());
	     
      fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
      std::string save_url_f = pFileConfig->Get("zeromq.org_url");
      std::string save_url_s = pFileConfig->Get("zeromq.book_url");
      fh::femas::market::CFemasMarketApp *pFemasMarletApp = new fh::femas::market::CFemasMarketApp(save_url_f,save_url_s);
      	  
      pFemasMarletApp->SetFileConfigData(FileConfigstr);
      std::vector<std::string> Depthstruments;
      std::vector<std::string> Subminstruments;
      Depthstruments.clear();
      Subminstruments.clear();	  
      Depthstruments.push_back(pFileConfig->Get("femas-DepthTopicID.TopicID"));
      Subminstruments.push_back("*");	  

      pFemasMarletApp->Initialize(Depthstruments);

      pFemasMarletApp->Subscribe(Subminstruments);	   
	  
      if(!pFemasMarletApp->Start())
      {
          printf("FemasMarletApp start  Error!\n");
	   pFemasMarletApp->Stop();	 
	   delete pFemasMarletApp;
	   return 0;	  
      }

      main_loop();
	  
      pFemasMarletApp->Stop();
      delete pFileConfig;
      delete pFemasMarletApp;

      printf("femas-market pross stop! \n");
      return 0;
}
