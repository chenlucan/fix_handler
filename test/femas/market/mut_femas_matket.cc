#include "core/assist/logger.h"
#include "gmock/gmock.h"

#include "mut_femas_matket.h"



namespace fh
{
namespace femas
{
namespace market
{

CMutFemasMarket::CMutFemasMarket()
{
        // noop
        
}      


CMutFemasMarket::~CMutFemasMarket()
{
        // noop
}   

TEST_F(CMutFemasMarket, CMutFemasMarket_Test001)
{
    CFemasMarket *ptestFemasMarket = new CFemasMarket(NULL);  
    EXPECT_EQ(0,(int)(ptestFemasMarket->Start()));	
    delete ptestFemasMarket;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test002)
{
    CFemasMarket *ptestFemasMarket = new CFemasMarket(NULL);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 	
    bool bRet = ptestFemasMarket->Start();
    ASSERT_TRUE(!bRet);	
    delete ptestFemasMarket;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test003)
{
    CFemasMarket *ptestFemasMarket = new CFemasMarket(NULL);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);	
    bool bRet = ptestFemasMarket->Start();
    ASSERT_TRUE(bRet);	
    delete ptestFemasMarket;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test004)
{
	
}


} // namespace market
} // namespace femas
} // namespace fh