#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>

#include "rem/efhmarket/rem_efhmarket.h"
#include "core/assist/logger.h"

bool stop_all=false;

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
           LOG_INFO("RemEfhMarket  runing  main_loop");
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

     main_loop();
	 
}	 