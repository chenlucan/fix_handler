

#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>

#include "ctp/market/ctp_market_application.h"
#include "core/assist/logger.h"


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
		LOG_INFO("CtpMarket  runing  main_loop");
		sleep(10);	 
	}
	return 0;
}

int main(int argc, char* argv[])
{

	//signal(SIGSEGV, handler);
	set_SignalProc();
	LOG_INFO("main() start");
	//read my cfg
	std::string FileConfigstr= "ctp_config.ini";

	LOG_INFO("FileConfigstr : ",FileConfigstr.c_str());


	fh::ctp::market::CCtpMarketApp *pCCtpMarketApp = new fh::ctp::market::CCtpMarketApp(FileConfigstr);

	std::vector<std::string> InstrumentIDs;

	InstrumentIDs.push_back("TF1706");
	InstrumentIDs.push_back("zn1705");
	InstrumentIDs.push_back("cs1801");
	InstrumentIDs.push_back("CF705");
    InstrumentIDs.push_back("");
	pCCtpMarketApp->Initialize(InstrumentIDs);


	if(!pCCtpMarketApp->Start())
	{
		LOG_ERROR("CtpMarletApp start  Error!");
		pCCtpMarketApp->Stop();	 
		delete pCCtpMarketApp;
		return 0;	  
	}

	main_loop();

	pCCtpMarketApp->Stop();
	delete pCCtpMarketApp;

	LOG_INFO("ctp-market pross stop!");
	return 0;
}
