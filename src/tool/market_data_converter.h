
#ifndef __FH_TOOL_MARKET_DATA_CONVERTER_H__
#define __FH_TOOL_MARKET_DATA_CONVERTER_H__

#include <vector>
#include "core/global.h"
#include "core/persist/converter.h"
#include "core/persist/market_data_provider.h"


namespace fh
{
namespace tool
{
    // 将原始行情数据解析成 l2,bbo 等解析后行情数据，然后保存到统一的 collection 中
    class MarketDataConverter
    {
        public:
            explicit MarketDataConverter(const std::string &market, const std::string &db_setting_file = "persist_settings.ini");
            virtual ~MarketDataConverter();

        public:
            std::pair<std::uint64_t, std::uint64_t> Convert(const std::string &start_date_include, const std::string &end_date_exclude);

        private:
            void Init(const std::string &db_setting_file);
            std::pair<std::uint64_t, std::uint64_t> Convert_one(const std::string &original_message);

        private:
            fh::core::persist::MarketDataProvider *m_data_provider;
            fh::core::persist::Converter *m_converter;
            std::string m_source_collection;    // 保存原始行情数据的 collection 的名字
            std::string m_target_collection;    // 保存解析后行情数据的 collection 的名字

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketDataConverter);
    };
} // namespace tool
} // namespace fh

#endif     // __FH_TOOL_MARKET_DATA_CONVERTER_H__
