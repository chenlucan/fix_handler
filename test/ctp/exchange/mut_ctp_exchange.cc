#include "core/assist/logger.h"
#include "gmock/gmock.h"
#include "mut_ctp_exchange.h"

namespace fh
{
namespace ctp
{
namespace exchange
{

CMutExchangeCtp::CMutExchangeCtp()
{
        // noop
        
}      


CMutExchangeCtp::~CMutExchangeCtp()
{
        // noop
}

TEST_F(CMutExchangeCtp, CMutExchangeCtp_Test001)
{
    std::string app_setting_file="Ctp_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    bool b_ret=false;	
    if(NULL != pFileConfig)
    {
        b_ret = true;
    }
    ASSERT_TRUE(b_ret);	
    delete pFileConfig;	
}

TEST_F(CMutExchangeCtp, CMutExchangeCtp_Test002)
{
    std::string app_setting_file="Ctp_config.ini";	
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7202","tcp://localhost:7201");
    fh::ctp::exchange::CCtpCommunicator* m_pCRemGlobexCommunicator = new CCtpCommunicator(m_strategy,app_setting_file);
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
} // namespace Ctp
} // namespace fh