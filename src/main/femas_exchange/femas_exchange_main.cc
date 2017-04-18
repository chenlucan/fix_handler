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


int main_loop()
{
     while (1)
    {
        printf("==========================exchange ruing================================\n");
	 sleep(10);	
    }
    return 0;	 
}

int main(int argc, char* argv[])
{

     signal(SIGSEGV, handler);

     printf("exchange main() start\n");

     main_loop();

     return 0;	 

}	 