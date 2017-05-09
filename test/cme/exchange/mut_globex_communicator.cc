
#include "gmock/gmock.h"

#include "core/assist/utility.h"
#include "core/assist/logger.h"
#include "core/strategy/invalid_order.h"
#include "cme/exchange/globex_communicator.h"
#include "cme/exchange/order.h"

#include "../../core/assist/mut_common.h"
#include "mut_globex_communicator.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    MutGlobexCommunicator::MutGlobexCommunicator()
    {
    }
    
    MutGlobexCommunicator::~MutGlobexCommunicator()
    {
        // noop
    }
    
    void MutGlobexCommunicator::SetUp()
    {
    }
    
    void MutGlobexCommunicator::TearDown()
    {
    }
    
    TEST_F(MutGlobexCommunicator, GlobexCommunicator_Test001)
    {       
        std::string fix_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);        
        fix_setting_file +="exchange_client.cfg";
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);        
        app_setting_file +="exchange_settings.ini";
        
        
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        
        fh::core::strategy::MockStrategyCommunicator *pMockStrategyCommunicator = new fh::core::strategy::MockStrategyCommunicator();

        fh::cme::exchange::GlobexCommunicator *pGlobex = new fh::cme::exchange::GlobexCommunicator(pMockStrategyCommunicator, fix_setting_file, app_settings);
        if(pGlobex!=nullptr)
        {
            std::vector<::pb::ems::Order> init_orders;
            pGlobex->Start(init_orders);  // logon
            
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
            //             
            pGlobex->Stop();             // logout
            
            delete pGlobex;
            pGlobex = nullptr;
        }
        
        if(pMockStrategyCommunicator!=nullptr)
        {
            delete pMockStrategyCommunicator;
            pMockStrategyCommunicator = nullptr;
        }
    }
    
} // namespace exchange
} // namespace cme
} // namespace fh
