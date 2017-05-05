
#include "core/assist/logger.h"
#include "tmalpha/exchange/tmalpha_exchange_application.h"

int main(int argc, char** argv)
{
    try
    {
        if (argc != 1)
        {
            LOG_ERROR("Usage: trade_matching_exchange_alpha_test");
            LOG_ERROR("Ex:       trade_matching_exchange_alpha_test");

            return 1;
        }

        fh::tmalpha::exchange::TmalphaExchangeApplication a;
        if(a.Start())
        {
            std::thread t([&a](){
                std::cin.get();
                a.Stop();
            });
            t.detach();
            a.Join();
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./trade_matching_exchange_alpha_test
