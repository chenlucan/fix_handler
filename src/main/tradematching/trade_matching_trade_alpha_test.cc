
#include "core/assist/logger.h"
#include "tmalpha/trade/tmalpha_trade_application.h"

int main(int argc, char** argv)
{
    try
    {
        if (argc != 1)
        {
            LOG_ERROR("Usage: trade_matching_trade_alpha_test");
            LOG_ERROR("Ex:       trade_matching_trade_alpha_test");

            return 1;
        }

        fh::tmalpha::trade::TmalphaTradeApplication a;
        if(a.Start())
        {
            std::cin.get();
            a.Stop();
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./trade_matching_trade_alpha_test
