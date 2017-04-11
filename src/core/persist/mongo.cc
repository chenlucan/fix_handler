#include <bsoncxx/document/value.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <bsoncxx/json.hpp>
#include "core/assist/logger.h"
#include "core/persist/mongo.h"

namespace fh
{
namespace core
{
namespace persist
{

    Mongo::Mongo(const std::string &db_url, const std::string &db_name, const std::string &collection_name)
    : m_instance(),
      m_client(mongocxx::uri(db_url)),
      m_collection(m_client[db_name][collection_name])
    {
        LOG_INFO("initialize mongo db connection: ", db_url, "(", db_name, ", ", collection_name, ")");
    }

    Mongo::~Mongo()
    {
        // noop
    }

    void Mongo::Insert(const std::string &json)
    {
        LOG_DEBUG("insert: ", json);

        auto doc = bsoncxx::from_json(json);
        auto result = m_collection.insert_one(doc.view());

        if(result)
        {
            LOG_DEBUG("insert success.");
        }
        else
        {
            LOG_WARN("insert error.");
        }
    }

} // namespace persist
} // namespace core
} // namespace fh
