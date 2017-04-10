
#ifndef __FH_CORE_PERSIST_MONGO_H__
#define __FH_CORE_PERSIST_MONGO_H__

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace persist
{
    class Mongo
    {
        public:
            Mongo(const std::string &db_url, const std::string &db_name, const std::string &collection_name);
            virtual ~Mongo();

        public:
            void Insert(const std::string &json);

        private:
            mongocxx::instance m_instance;
            mongocxx::client m_client;
            mongocxx::collection m_collection;

        private:
			DISALLOW_COPY_AND_ASSIGN(Mongo);
    };
} // namespace persist
} // namespace core
} // namespace fh

#endif // __FH_CORE_PERSIST_MONGO_H__
