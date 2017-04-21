#include <bsoncxx/document/value.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/json.hpp>
#include "core/assist/logger.h"
#include "core/persist/mongo.h"

namespace fh
{
namespace core
{
namespace persist
{

    Mongo::Mongo(const std::string &db_url, const std::string &db_name, std::uint32_t page_size)
    : m_instance(), m_client(mongocxx::uri(db_url)), m_db(m_client[db_name]), m_page_size(page_size)
    {
        LOG_INFO("initialize mongo db connection: ", db_url, db_name);
    }

    Mongo::~Mongo()
    {
        // noop
    }

    void Mongo::Insert(const std::string &json)
    {
        try
        {
            // 根据来的数据中 market 属性决定数据插入到哪个 collection
            auto doc = bsoncxx::from_json(json);
            auto view = doc.view();
            auto colname = view["market"].get_utf8().value.to_string();

            LOG_DEBUG("insert to: ", colname, ", value:", json);

            auto collection = m_db[colname];
            auto result = collection.insert_one(view);

            if(result)
            {
                LOG_DEBUG("insert success.");
            }
            else
            {
                LOG_WARN("insert error.");
            }
        }
        catch(std::exception &e)
        {
            LOG_ERROR("insert exception:", e.what());
        }
    }

    bool Mongo::Query(std::vector<std::string> &result, const std::string &market,
                                                                             const std::string &start_date_include, const std::string &end_date_exclude,
                                                                             std::uint64_t prev_last_record_insert_time)
    {
        try
        {
            // condition is: (sendingTimeStr >= start_date_include AND sendingTimeStr < end_date_exclude) AND (insertTime > prev_last_record_insert_time)
            bsoncxx::builder::stream::document condition{};
            condition << "$and" << bsoncxx::builder::stream::open_array
                                                        << bsoncxx::builder::stream::open_document
                                                            << "sendingTimeStr"
                                                                    << bsoncxx::builder::stream::open_document
                                                                            << "$gte" << start_date_include
                                                                            << "$lt" << end_date_exclude
                                                                     << bsoncxx::builder::stream::close_document
                                                         << bsoncxx::builder::stream::close_document
                                                         << bsoncxx::builder::stream::open_document
                                                             << "insertTime"
                                                                     << bsoncxx::builder::stream::open_document
                                                                             << "$gt" << std::to_string(prev_last_record_insert_time)
                                                                     << bsoncxx::builder::stream::close_document
                                                          << bsoncxx::builder::stream::close_document
                                                 << bsoncxx::builder::stream::close_array;

            // order by insertTime limit $m_page_size
            bsoncxx::builder::stream::document sort{};
            sort << "insertTime" << 1 ;
            mongocxx::options::find options;
            options.sort(sort.view());
            options.limit(m_page_size);

            LOG_INFO("query ", market, " in range [", start_date_include, ", ", end_date_exclude, ") and start after ",
                    prev_last_record_insert_time, " limit ", m_page_size);

            auto collection = m_db[market];
            auto cursor  = collection.find(condition.view(), options);
            for(auto doc : cursor)
            {
                result.push_back(bsoncxx::to_json(doc));
            }

            return true;
        }
        catch(std::exception &e)
        {
            LOG_ERROR("query exception:", e.what());
            return false;
        }
    }

} // namespace persist
} // namespace core
} // namespace fh
