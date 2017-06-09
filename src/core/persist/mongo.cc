#include <map>
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

    // 将接受到的数据保存到数据库
    // 保存的目标 collection 由数据中 market 属性决定；如果没有这个属性，那么插入到 default 这个 collection 里面
    void Mongo::Insert(const std::string &json)
    {
        try
        {
            auto doc = bsoncxx::from_json(json);
            auto view = doc.view();
            std::string collection_name = Mongo::Get_value(view, "market", "default");

            LOG_DEBUG("insert to: ", collection_name, ", value:", json);

            auto collection = m_db[collection_name];
            auto result = collection.insert_one(view);

            if(result)  LOG_DEBUG("insert success.");
            else  LOG_WARN("insert error.");
        }
        catch(std::exception &e)
        {
            LOG_ERROR("insert exception:", e.what());
        }
    }

    // 查询指定 market 的行情数据，将结果存入 result
    // 查询范围： sendingTimeStr 在 [start_date_include, end_date_exclude) 之间，并且 insertTime > prev_last_record_insert_time
    // 返回查询结果的数据件数
    std::uint64_t Mongo::Query(std::vector<std::string> &result, const std::string &market,
                                                         const std::string &start_date_include, const std::string &end_date_exclude,
                                                         std::uint64_t prev_last_record_insert_time)
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

        // order by insertTime asc limit $m_page_size
        bsoncxx::builder::stream::document sort{};
        sort << "insertTime" << 1 ;
        mongocxx::options::find options;
        options.sort(sort.view());
        options.limit(m_page_size);

        LOG_INFO("query ", market, " in range [", start_date_include, ", ", end_date_exclude, ") and start after ",
                prev_last_record_insert_time, " limit ", m_page_size);

        auto collection = m_db[market];
        auto cursor  = collection.find(condition.view(), options);

        // 如果该交易所没有另外保存了合约信息的话，直接将结果返回
        std::string contract_collection = market + "_contract";
        if(!m_db.has_collection(contract_collection))
        {
            for(auto v : cursor) result.push_back(bsoncxx::to_json(v));
            return result.size();
        }

        // 否则说明该交易所还另外保存了合约信息，要检索出所有关联到的合约信息
        LOG_DEBUG("query contract info from ", contract_collection);

        // 先把行情数据提取出来，然后将结果中的所有数据的 InstrumentID 提取出来
        std::vector<bsoncxx::document::view> result_views;
        std::set<std::string> contract_ids;
        for(auto doc : cursor)
        {
            result_views.push_back(doc);
            contract_ids.insert(Mongo::Get_value(doc, "InstrumentID"));
        }

        // 检索出这些 InstrumentID 对应的合约属性信息
        std::map<std::string, std::string> contract_infos = this->Query_contracts(contract_collection, contract_ids);
        // 将合约的 VolumeMultiple 属性添加到行情数据中返回出去
        for(auto v : result_views)
        {
            bsoncxx::builder::stream::document d{};
            d << bsoncxx::builder::stream::concatenate(v);
            std::string id = Mongo::Get_value(v, "InstrumentID");
            if(contract_infos.find(id) != contract_infos.end()) d << "VolumeMultiple" << contract_infos[id];
            else d << "VolumeMultiple" << "0";   // 找不到的话设置成一个特殊值 0
            result.push_back(bsoncxx::to_json(d.view()));
        }

        return result.size();
    }

    // 检索指定 InstrumentID 的一系列合约的参数信息
    std::map<std::string, std::string> Mongo::Query_contracts(const std::string &contract_collection, const std::set<std::string> &contract_ids)
    {
        // InstrumentId in (contract_ids)
        bsoncxx::builder::stream::document condition{};
        auto builder = condition << "InstrumentID"
                                                      << bsoncxx::builder::stream::open_document
                                                      << "$in"
                                                       << bsoncxx::builder::stream::open_array;
        for(auto &id : contract_ids) builder << id;
        builder << bsoncxx::builder::stream::close_array
                      << bsoncxx::builder::stream::close_document;

        // order by insertTime asc：为了让同一个合约的多条记录中，最后插入的排在最后面
        bsoncxx::builder::stream::document sort{};
        sort << "insertTime" << 1 ;
        mongocxx::options::find options;
        options.sort(sort.view());

        std::map<std::string, std::string> result;
        auto cursor = m_db[contract_collection].find(condition.view(), options);
        for(auto doc : cursor)
        {
            std::string id = Mongo::Get_value(doc, "InstrumentID");
            std::string value = Mongo::Get_value(doc, "VolumeMultiple", "0");
            result[id] = value;
        }

        return result;
    }

    std::uint64_t Mongo::Count(const std::string &market, const std::string &start_date_include, const std::string &end_date_exclude)
    {
        // condition is: sendingTimeStr >= start_date_include AND sendingTimeStr < end_date_exclude
        bsoncxx::builder::stream::document condition{};
        condition << "sendingTimeStr"
                                        << bsoncxx::builder::stream::open_document
                                                << "$gte" << start_date_include
                                                << "$lt" << end_date_exclude
                                         << bsoncxx::builder::stream::close_document;

        auto collection = m_db[market];
        return collection.count(condition.view());
    }

    // 获取 json 中某个 key 对应的 value 的字符串值，key 不存在的时候返回 default_value
    std::string Mongo::Get_value(const bsoncxx::document::view &doc, const std::string &key, const std::string &default_value)
    {
        auto e = doc[key];
        return e ? e.get_utf8().value.to_string() : default_value;
    }

} // namespace persist
} // namespace core
} // namespace fh
