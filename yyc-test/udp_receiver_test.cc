
#include "application.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

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
        if (argc != 1)
        {
            std::cerr << "Usage: udp_receiver_test \n";
            std::cerr << "Ex:    udp_receiver_test \n";
            
            return 1;
        }

        rczg::Application a;
        a.Start();
        
        std::cin.get();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

// ./udp_receiver_test 