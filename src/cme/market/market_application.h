
#ifndef __FH_CME_MARKET_APPLICATION_H__
#define __FH_CME_MARKET_APPLICATION_H__

#include "core/global.h"
#include "cme/market/cme_market.h"
#include "core/book/book_sender.h"

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
                    const std::string &channel_setting_file = CME_MARKET_CONFIG_XML,
                    const std::string &app_setting_file = CME_MARKET_SETTINGS_INI);
            virtual ~MarketApplication();

        public:
            void Start();
            void Stop();

        private:
            void Initial_application(const std::string &channel_id, const std::string &channel_setting_file, const std::string &app_setting_file);
            std::vector<fh::cme::market::setting::Channel> Get_target_channels(const std::string &channel_ids, const std::string &channel_setting_file);

        private:
            fh::core::book::BookSender *m_book_sender;
            CmeMarket *m_market;

        private:
            DISALLOW_COPY_AND_ASSIGN(MarketApplication);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_APPLICATION_H__
