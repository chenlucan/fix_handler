
#ifndef __FH_CORE_PERSIST_MONGO_H__
#define __FH_CORE_PERSIST_MONGO_H__

#include <vector>
#include <set>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/document/value.hpp>
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
            Mongo(const std::string &db_url, const std::string &db_name, std::uint32_t page_size);
            virtual ~Mongo();

        public:
            // 将指定内容插入到数据库（目标 collection 由 json 中的 market 属性的值确定）
            void Insert(const std::string &json);
            // 将指定内容插入到数据库
            void Insert(const std::string &collection_name, const std::string &json);
            // 从指定 collection_name 对应的 collection 中检索指定日期时间范围内从指定位置开始的数据（检索的最大条数由配置文件决定）
            // start_date_include, end_date_exclude: "yyyy-MM-dd HH:mm:ss" 格式
            // prev_last_record_insert_time: 分页用。0 表示从上述范围内第一条数据开始检索；
            std::uint64_t Query(std::vector<std::string> &result, const std::string &collection_name,
                                  const std::string &start_date_include, const std::string &end_date_exclude,
                                  std::uint64_t prev_last_record_insert_time = 0);
            // 从指定 collection_name 对应的 collection 中检索指定日期时间范围内从指定位置开始的数据（检索的最大条数由配置文件决定）
            // market: 只检索 market 属性值是指定 market 的数据
            // start_date_include, end_date_exclude: "yyyy-MM-dd HH:mm:ss" 格式
            // prev_last_record_insert_time: 分页用。0 表示从上述范围内第一条数据开始检索
            std::uint64_t Query(std::vector<std::string> &result, const std::string &collection_name, const std::string &market,
                                  const std::string &start_date_include, const std::string &end_date_exclude,
                                  std::uint64_t prev_last_record_insert_time = 0);
            // 检索指定日期时间范围内指定集合的行情数据件数
            std::uint64_t Count(const std::string &collection_name, const std::string &start_date_include, const std::string &end_date_exclude);
            // 检索指定日期时间范围内指定集合的行情数据件数（只检索 market 属性值是指定 market 的数据）
            std::uint64_t Count(const std::string &collection_name, const std::string &market,
                    const std::string &start_date_include, const std::string &end_date_exclude);

        private:
            // 获取 json 中某个 key 对应的 value 的字符串值，key 不存在的时候返回 default_value
            static std::string Get_value(const bsoncxx::document::view &doc, const std::string &key, const std::string &default_value = "");
            // 检索附加合约信息
            std::map<std::string, std::string> Query_contracts(const std::string &contract_collection, const std::set<std::string> &contract_ids);

        private:
            mongocxx::instance m_instance;
            mongocxx::client m_client;
            mongocxx::database m_db;
            std::uint32_t m_page_size;

        private:
			DISALLOW_COPY_AND_ASSIGN(Mongo);
    };
} // namespace persist
} // namespace core
} // namespace fh

#endif // __FH_CORE_PERSIST_MONGO_H__
