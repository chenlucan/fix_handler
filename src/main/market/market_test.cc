
#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "cme/market/market_application.h"
#include "core/assist/logger.h"

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

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);

    try
    {
        if (argc != 1 && argc != 2)
        {
            LOG_ERROR("Usage: market_test [channel_id1[,channel_id2,...]]");
            return 1;
        }

        fh::core::assist::Logger::Set_level(fh::core::assist::Logger::Level::TRACE);
        fh::cme::market::MarketApplication a(argc == 2 ? argv[1] : "");

        a.Start();
        std::cin.get();
        a.Stop();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./market_test 360,361
