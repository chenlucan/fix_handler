
#include "core/assist/logger.h"
#include "gmock/gmock.h"
#include "mut_rem_exchange.h"

namespace fh
{
namespace rem
{
namespace exchange
{

CMutRemExchange::CMutRemExchange()
{
        // noop
        
}      


CMutRemExchange::~CMutRemExchange()
{
        // noop
}

TEST_F(CMutRemExchange, CMutRemExchange_Test001)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:7002","tcp://localhost:7001");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
	
    EXPECT_EQ(10,m_pCRemGlobexCommunicator->m_itimeout);	
	
    delete m_pCRemGlobexCommunicator;	
}

TEST_F(CMutRemExchange, CMutRemExchange_Test002)
{
    std::string app_setting_file="rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(app_setting_file);
    fh::core::strategy::StrategyCommunicator* m_strategy = new fh::core::strategy::StrategyCommunicator("tcp://*:6002","tcp://localhost:6001");
    fh::rem::exchange::CRemCommunicator* m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
	
    EXPECT_EQ(0,m_pCRemGlobexCommunicator->m_ReqId);	
	
    delete m_pCRemGlobexCommunicator;	
}


} // namespace exchange
} // namespace rem
} // namespace fh