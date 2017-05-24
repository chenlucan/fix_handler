#include "core/assist/logger.h"
#include "gmock/gmock.h"

#include "mut_rem_matket.h"

namespace fh
{
namespace rem
{
namespace market
{

CMutRemMarket::CMutRemMarket()
{
        // noop
        
}      


CMutRemMarket::~CMutRemMarket()
{
        // noop
}

TEST_F(CMutRemMarket, CMutRemMarket_Test001)
{
    CRemMarket *ptestRemMarket = new CRemMarket(NULL);  
    EXPECT_EQ(0,(int)(ptestRemMarket->Start()));	
    delete ptestRemMarket;	
}

TEST_F(CMutRemMarket, CMutRemMarket_Test002)
{
    std::string FileConfigstr= "rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:2557", "tcp://*:2558");	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	
	
    std::vector<std::string> insts;
    insts.clear();	
    insts.push_back("cu1711P35500");	
    insts.push_back("HO1706-P-2450");	
    insts.push_back("SR801P7400");		
    ptestRemMarket->Subscribe(insts);	
    EXPECT_EQ(3,ptestRemMarket->m_RemMarkrtManager->mISubSuss);
    ptestRemMarket->Stop();		
    delete ptestRemMarket;		
}

TEST_F(CMutRemMarket, CMutRemMarket_Test003)
{
    std::string FileConfigstr= "rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:3557", "tcp://*:3558");	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	
	
    std::vector<std::string> insts;
    insts.clear();			
    ptestRemMarket->Subscribe(insts);	
    EXPECT_EQ(1,ptestRemMarket->m_RemMarkrtManager->mISubSuss);
    ptestRemMarket->Stop();		
    delete ptestRemMarket;		
}

TEST_F(CMutRemMarket, CMutRemMarket_Test004)
{
    
    std::string FileConfigstr= "rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender(save_url_f, save_url_s);	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	
    bool bRet = ptestRemMarket->Start();
    ASSERT_TRUE(bRet);	
    ptestRemMarket->Stop();	
    delete ptestRemMarket;	
}

} // namespace market
} // namespace rem
} // namespace fh