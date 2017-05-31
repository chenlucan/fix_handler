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
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7102","tcp://localhost:7101");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
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
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7202","tcp://localhost:7201");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(10 == m_pCRemGlobexCommunicator->m_itimeout)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutExchangeFemas, CMutExchangeFemas_Test003)
{
    std::string app_setting_file="femas_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7302","tcp://localhost:7301");
    fh::femas::exchange::CFemasCommunicator* m_pCRemGlobexCommunicator = new CFemasCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    bool b_ret=false;	
    if(0 == m_pCRemGlobexCommunicator->m_ReqId)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete m_pCRemGlobexCommunicator;	
}


} // namespace market
} // namespace femas
} // namespace fh