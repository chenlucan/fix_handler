#include "core/assist/logger.h"
#include "gmock/gmock.h"
#include "mut_femas_exchange.h"

namespace fh
{
namespace femas
{
namespace exchange
{

CMutExchangeFemas::CMutExchangeFemas()
{
        // noop
        
}      


CMutExchangeFemas::~CMutExchangeFemas()
{
        // noop
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test001)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7002","tcp://localhost:7001");
    fh::femas::exchange::CFemasGlobexCommunicator* m_pCRemGlobexCommunicator = new CFemasGlobexCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(10 == m_pCRemGlobexCommunicator->m_itimeout)
    {
        b_ret = true;
    }
    ASSERT_TRUE(!b_ret);	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test002)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7002","tcp://localhost:7001");
    fh::femas::exchange::CFemasGlobexCommunicator* m_pCRemGlobexCommunicator = new CFemasGlobexCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(10 == m_pCRemGlobexCommunicator->m_itimeout)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}


} // namespace market
} // namespace femas
} // namespace fh