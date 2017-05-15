#include <thread>
#include "cme/exchange/exchange_application.h"
#include "cme/exchange/exchange_settings.h"
#include "core/assist/logger.h"

#include "../../core/assist/mut_common.h"
#include "mut_exchange_application.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    MutExchangeApplication::MutExchangeApplication()
    {
    }
    
    MutExchangeApplication::~MutExchangeApplication()
    {
        // noop
    }
    
    void MutExchangeApplication::SetUp()
    {
    }
    
    void MutExchangeApplication::TearDown()
    {
    }
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test001)
    {
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        fh::cme::exchange::ExchangeApplication exchangeApp(fix_setting_file, app_setting_file);
        exchangeApp.Start();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        exchangeApp.Stop();
    }
    
} // namespace exchange
} // namespace cme
} // namespace fh
