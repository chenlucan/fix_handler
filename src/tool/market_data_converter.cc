
#include "core/assist/settings.h"
//#include "femas/market/convert/Femas_book_convert.h"
#include "ctp/market/book_convert.h"
#include "tool/market_data_converter.h"

namespace fh
{
namespace tool
{

    MarketDataConverter::MarketDataConverter(const std::string &market, const std::string &db_setting_file)
    : m_data_provider(nullptr), m_converter(nullptr), m_source_collection(market), m_target_collection("")
    {
        this->Init(db_setting_file);
    }

    MarketDataConverter::~MarketDataConverter()
    {
        delete m_converter;
        delete m_data_provider;
    }

    std::pair<std::uint64_t, std::uint64_t> MarketDataConverter::Convert(const std::string &start_date_include, const std::string &end_date_exclude)
    {
        m_data_provider->Range(start_date_include, end_date_exclude);

        std::uint64_t total = m_data_provider->Total_count();
        LOG_INFO("Start[", m_source_collection, "]. There are ", total, " messages waiting for convert.");

        std::uint64_t total_success = 0;
        std::uint64_t total_failure = 0;
        std::uint64_t last_message_identify = 0;
        while(true)
        {
            std::vector<std::string> messages;
            std::uint64_t count = m_data_provider->Query(messages, last_message_identify);

            LOG_INFO("pick messages count: ", count);
            if(count == 0)
            {
                LOG_INFO("all messages fetched.");
                break;
            }

            // 保存最后一条消息的识别 ID，下次的检索就从这条数据以下开始
            last_message_identify = m_data_provider->Message_identify(messages.back());

            for(auto &m : messages)
            {
                auto counts = this->Convert_one(m);
                LOG_DEBUG("converted original message: success=", counts.first, ", failure=", counts.second);
                total_success += counts.first;
                total_failure += counts.second;
            }
        }

        LOG_INFO("total: success=", total_success, ", failure=", total_failure);
        return {total_success, total_failure};
    }

    void MarketDataConverter::Init(const std::string &db_setting_file)
    {
        fh::core::assist::Settings settings(db_setting_file);
        m_target_collection = settings.Get("persist.market_data_collection");
        m_data_provider = new fh::core::persist::MarketDataProvider(m_source_collection, settings);

       // if(m_source_collection == "FEMAS") m_converter = new fh::femas::market::convert::FemasBookConvert();
	   if(m_source_collection == "CTP") m_converter = new fh::ctp::market::convert::CtpBookConvert();
        else  throw std::invalid_argument("market not exist: " + m_source_collection);
    }

    // 转换一条原始行情，然后将得到的转换后数据插入到数据库，返回成功件数和失败件数
    std::pair<std::uint64_t, std::uint64_t> MarketDataConverter::Convert_one(const std::string &original_message)
    {
        std::uint64_t total_success = 0;
        std::uint64_t total_failure = 0;
        std::map<std::string, std::string> result = m_converter->Convert(original_message);
        std::for_each(result.begin(), result.end(), [this, &total_success, &total_failure](const std::pair<std::string, std::string> &r) {
            bool is_success = m_data_provider->Insert(m_target_collection, r.second);
            if(is_success) total_success++;
            else total_failure++;
        });

        return {total_success, total_failure};
    }

} // namespace tool
} // namespace fh
