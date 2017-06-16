
#include "core/assist/logger.h"
#include "tool/market_data_converter.h"


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            LOG_ERROR("Usage: data_converter_test <target_market> <start_date_include> <end_date_exclude>");
            LOG_ERROR("Ex:       data_converter_test FEMAS \"2017-06-06 10:00:00\" \"2017-06-06 11:00:00\"");

            return 1;
        }

        std::string market = argv[1];
        std::string start_date_include = argv[2];     // "yyyy-MM-dd HH:mm:ss"
        std::string end_date_exclude = argv[3];   // "yyyy-MM-dd HH:mm:ss"

        fh::tool::MarketDataConverter c(market);
        c.Convert(start_date_include, end_date_exclude);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./data_converter_test FEMAS "2017-04-24 01:00:00" "2017-04-24 05:13:50"
