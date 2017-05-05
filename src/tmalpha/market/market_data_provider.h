
#ifndef __FH_TMALPHA_MARKET_MARKET_DATA_PROVIDER_H__
#define __FH_TMALPHA_MARKET_MARKET_DATA_PROVIDER_H__

#include <string>
#include <vector>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include "core/global.h"
#include "core/assist/logger.h"
#include "core/persist/mongo.h"
#include "core/assist/settings.h"
#include "tmalpha/market/data_provider.h"

#define  GET_STR_FROM_JSON(view, key) view[key].get_utf8().value.to_string()
#define  GET_INT_FROM_JSON(view, key) std::stol(GET_STR_FROM_JSON(view, key))

namespace fh
{
namespace tmalpha
{
namespace market
{
    class MarketDataProvider : public DataProvider
    {
        public:
            MarketDataProvider(const std::string &market, const fh::core::assist::Settings &db_settings)
            : m_market(market), m_range(), m_db(nullptr)
            {
                std::string db_url = db_settings.Get("persist.db_url");
                std::string db_name = db_settings.Get("persist.db_name");
                std::uint32_t page_size = std::stoi(db_settings.Get("persist.page_size"));
                m_db = new fh::core::persist::Mongo(db_url, db_name, page_size);
            }

            virtual ~MarketDataProvider()
            {
                delete m_db;
            }

        public:
            // "yyyy-MM-dd HH:mm:ss" 格式日期范围
            void Range(const std::string &start_date_include, const std::string &end_date_exclude) override
            {
                m_range = {start_date_include, end_date_exclude};
            }

            std::uint64_t Total_count() override
            {
                std::uint64_t count =  m_db->Count(m_market, m_range.first, m_range.second);
                LOG_INFO("Total count for [", m_market, "] range [", m_range.first, ", ", m_range.second, ") = ", count);
                return count;
            }

            std::uint64_t Query(std::vector<std::string> &result, std::uint64_t prev_last_record_insert_time) override
            {
                std::uint64_t count =  m_db->Query(result, m_market, m_range.first, m_range.second, prev_last_record_insert_time);
                LOG_INFO("fetch count from [",  prev_last_record_insert_time, "] = ", count);
                return count;
            }

            std::uint64_t Message_identify(const std::string &message) override
            {
                auto doc = bsoncxx::from_json(message);
                auto view = doc.view();
                return GET_INT_FROM_JSON(view, "insertTime");
            }

            std::uint64_t Message_send_time(const std::string &message) override
            {
                auto doc = bsoncxx::from_json(message);
                auto view = doc.view();
                return GET_INT_FROM_JSON(view, "sendingTime");
            }

        private:
            std::string m_market;
            std::pair<std::string, std::string> m_range;
            fh::core::persist::Mongo *m_db;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketDataProvider);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MARKET_DATA_PROVIDER_H__
