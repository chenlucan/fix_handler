
#ifndef __FH_CME_MARKET_APPLICATION_H__
#define __FH_CME_MARKET_APPLICATION_H__

#include "core/global.h"
#include "cme/market/cme_market.h"
#include "cme/market/book_sender.h"

namespace fh
{
namespace cme
{
namespace market
{

    class MarketApplication
    {
        public:
            explicit MarketApplication(
                    const std::string &channel_id,
                    const std::string &channel_setting_file = "market_config.xml",
                    const std::string &app_setting_file = "market_settings.ini");
            virtual ~MarketApplication();

        public:
            void Start();
            void Join();
            void Stop();

        private:
            void Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file);
            std::vector<fh::cme::market::setting::Channel> Get_target_channels(const std::string &channel_ids, const std::string &channel_setting_file);

        private:
            BookSender *m_book_sender;
            CmeMarket *m_market;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketApplication);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_APPLICATION_H__
