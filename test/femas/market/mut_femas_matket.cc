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
    CFemasMarket *ptestFemasMarket = new CFemasMarket(NULL);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);
  
    std::vector<std::string> insts;
    insts.clear();	
    insts.push_back("cu1711P35500");	
    insts.push_back("HO1706-P-2450");	
    insts.push_back("SR801P7400");		
    ptestFemasMarket->Subscribe(insts);	
    EXPECT_EQ(3,ptestFemasMarket->m_FemasMarketManager->mISubSuss);
    delete ptestFemasMarket;		
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test005)
{
    CFemasMarket *ptestFemasMarket = new CFemasMarket(NULL);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);
  
    std::vector<std::string> insts;
    insts.clear();			
    ptestFemasMarket->Subscribe(insts);	
    EXPECT_EQ(1,ptestFemasMarket->m_FemasMarketManager->mISubSuss);
    delete ptestFemasMarket;		
}


} // namespace market
} // namespace femas
} // namespace fh