#include "core/assist/logger.h"
#include "gmock/gmock.h"

#include "mut_custom_manager.h"



namespace fh
{
namespace ctp
{
namespace market
{

CMutCustomManager::CMutCustomManager()
{
        // noop
        
}      


CMutCustomManager::~CMutCustomManager()
{
        // noop
}   

TEST_F(CMutCustomManager, CMutCustomManager_Test001)
{
	std::string FileConfig= "ctp_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfig);
	auto MDAccountID = std::make_shared<fh::ctp::market::MDAccountID>();
	MDAccountID->setPassword(pFileConfig->Get("ctp-user.Password"));
	MDAccountID->setBrokerID(pFileConfig->Get("ctp-user.BrokerID"));
	MDAccountID->setMarketFrontAddress(pFileConfig->Get("ctp-market.url"));
	MDAccountID->setUserID(pFileConfig->Get("ctp-user.UserID"));
	MDAccountID->settimeout(pFileConfig->Get("ctp-timeout.timeout"));	
    delete pFileConfig;
    EXPECT_EQ(0,0);	 
}
} // namespace market
} // namespace femas
} // namespace fh
