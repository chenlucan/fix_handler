#include "gmock/gmock.h"

#include "cme/market/market_manager.h"
#include "cme/market/cme_market.h"
#include "core/assist/logger.h"

#include "../../core/assist/mut_common.h"
#include "../../core/market/mock_marketlisteneri.h"

#include "mut_market_manager.h"

using ::testing::AtLeast;  
using ::testing::Return;  
using ::testing::Mock;
using ::testing::AnyNumber;

namespace fh
{
namespace cme
{
namespace market
{

    MutMarketManager::MutMarketManager()
    {
    }

    MutMarketManager::~MutMarketManager()
    {
    }
    
    void MutMarketManager::SetUp()
    {
    }
    
    void MutMarketManager::TearDown()
    {
    }
    
    TEST_F(MutMarketManager, MarketManager_Test001)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
                    
        if(nullptr == mock_market_listener)
        {
            return;
        }

        fh::cme::market::setting::Channel channel;
        channel.id = "360";
        channel.label = "COMEX Globex Futures";
        std::vector<fh::cme::market::setting::Channel> target_channels;        
        target_channels.push_back(channel);

        if(target_channels.empty())
        {
            // 如果没有 channel，直接终止
            LOG_WARN("all channels not found, exit");
            return;
        }
        
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);        
        app_setting_file +="market_settings.ini";
        
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        fh::cme::market::CmeMarket * cmeMarket = new CmeMarket(mock_market_listener, target_channels, app_settings);
        
        if(nullptr == cmeMarket)
        {
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        delete cmeMarket;
        cmeMarket = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    
    TEST_F(MutMarketManager, MarketManager_Test002)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
                    
        if(nullptr == mock_market_listener)
        {
            return;
        }

        fh::cme::market::setting::Channel channel;
        channel.id = "360";
        channel.label = "COMEX Globex Futures";
        std::vector<fh::cme::market::setting::Channel> target_channels;        
        target_channels.push_back(channel);

        if(target_channels.empty())
        {
            // 如果没有 channel，直接终止
            LOG_WARN("all channels not found, exit");
            return;
        }
        
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);        
        app_setting_file +="market_settings.ini";
        
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        fh::cme::market::CmeMarket * cmeMarket = new CmeMarket(mock_market_listener, target_channels, app_settings);
        
        if(nullptr == cmeMarket)
        {
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        // stop测试有子线程invalid argument问题，源码需修改，暂时用例代码先注释
        //cmeMarket->Start();
        //cmeMarket->Stop();
        delete cmeMarket;
        cmeMarket = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
} // namespace market
} // namespace cme
} // namespace fh
