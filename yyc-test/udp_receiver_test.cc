
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "application.h"
#include "logger.h"

void handler(int sig) 
{
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler); 
    
    try
    {
        if (argc != 2 || (strcmp(argv[1], "-s") != 0 && strcmp(argv[1], "-j") != 0))
        {
            rczg::Logger::Error("Usage: udp_receiver_test -s|-j");
            return 1;
        }

        rczg::Logger::Set_level(rczg::Logger::Level::DEBUG);
        rczg::Application a("360");
        
        if(strcmp(argv[1], "-s") == 0)
        {
            a.Start();
        }
        else
        {
            a.Join();
        }
        
        std::cin.get();
    }
    catch (std::exception& e)
    {
        rczg::Logger::Error("Exception", e.what());
    }

    return 0;
}

// ./udp_receiver_test -s