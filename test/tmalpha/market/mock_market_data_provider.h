
#ifndef __FH_TMALPHA_MARKET_MOCK_MARKET_DATA_PROVIDER_H__
#define __FH_TMALPHA_MARKET_MOCK_MARKET_DATA_PROVIDER_H__

#include <string>
#include <vector>
#include "core/global.h"
#include "core/assist/logger.h"
#include "core/persist/data_provider.h"

namespace fh
{
namespace tmalpha
{
namespace market
{
    class MockMarketDataProvider : public fh::core::persist::DataProvider
    {
        public:
            MockMarketDataProvider(const std::vector<std::string> &messages, std::uint32_t page_size)
            : m_messages(messages), m_page_size(page_size)
            {
            }

            virtual ~MockMarketDataProvider()
            {
            }

        public:
            void Range(const std::string &start_date_include, const std::string &end_date_exclude) override
            {
            }

            std::uint64_t Total_count() override
            {
                return m_messages.size();
            }

            std::uint64_t Query(std::vector<std::string> &result, std::uint64_t prev_last_record) override
            {
                for(const std::string &m : m_messages)
                {
                    if(this->Message_identify(m) > prev_last_record)
                    {
                        result.push_back(m);
                        if(result.size() >= m_page_size) break;
                    }
                }

                return result.size();
            }

            std::uint64_t Message_identify(const std::string &message) override
            {
                auto pos = message.find("\"insertTime\"") + 16;
                return std::stol(message.substr(pos, 19));
            }

            std::uint64_t Message_send_time(const std::string &message) override
            {
                auto pos = message.find("\"sendingTime\"") + 17;
                return std::stol(message.substr(pos, 19));
            }

        private:
            const std::vector<std::string> &m_messages;
            std::uint32_t m_page_size;

        private:
            DISALLOW_COPY_AND_ASSIGN(MockMarketDataProvider);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_MOCK_MARKET_DATA_PROVIDER_H__
