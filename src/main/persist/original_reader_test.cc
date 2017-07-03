
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include "core/persist/mongo.h"
#include "core/assist/logger.h"


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            LOG_ERROR("Usage: original_reader_test market start_date_include end_date_exclude");
            LOG_ERROR("Ex:       original_reader_test TEST \"2017-06-06 10:00:00\" \"2017-06-06 11:00:00\"");

            return 1;
        }

        std::string market = argv[1];
        std::string start_date_include = argv[2];     // "yyyy-MM-dd HH:mm:ss"
        std::string end_date_exclude = argv[3];   // "yyyy-MM-dd HH:mm:ss"

        fh::core::persist::Mongo mongo("mongodb://localhost:27017/", "markets_data", 10);

        std::uint64_t count =  mongo.Count(market, start_date_include, end_date_exclude);
        LOG_INFO("Total count for [", market, "] range [", start_date_include, ", ", end_date_exclude, ") = ", count);

        std::uint64_t prev = 0;
        std::uint64_t total = 0;
        while(true)
        {
            std::vector<std::string> result;
            count = mongo.Query(result, market, start_date_include, end_date_exclude, prev);
            LOG_INFO("fetch count start from [", prev, "] = ", count, ", total = ", (total += count));
            if(count == 0) break;
            for(auto &r : result) LOG_INFO(r);
            auto doc = bsoncxx::from_json(result.back());
            auto view = doc.view();
            prev = std::stol(view["insertTime"].get_utf8().value.to_string());
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./original_reader_test CME "2017-04-24 01:00:00" "2017-04-24 05:13:50"
