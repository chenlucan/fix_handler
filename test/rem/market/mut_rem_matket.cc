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
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:8557", "tcp://*:8558");	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender); 
	
    EXPECT_EQ(0,(int)(ptestRemMarket->Start()));	

    ptestRemMarket->Stop();		
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
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:4557", "tcp://*:4558");	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	
	
    bool bRet = true;
    if(NULL==ptestRemMarket || NULL == m_book_sender ||NULL == pFileConfig)
    {
        bRet = false;
    }
    ASSERT_TRUE(bRet);	
    ptestRemMarket->Stop();		
    delete ptestRemMarket;
    delete m_book_sender;
    delete pFileConfig;	
}

TEST_F(CMutRemMarket, CMutRemMarket_Test005)
{
    
    std::string FileConfigstr= "rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender(save_url_f, save_url_s);	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	

    EesEqsIntrumentType chInstrumentType = EQS_FUTURE;
    EESMarketDepthQuoteData tmpDepthQuoteData;
    //
    memset(&tmpDepthQuoteData,0,sizeof(EESMarketDepthQuoteData));
    strcpy(tmpDepthQuoteData.TradingDay,"20170524");	
    strcpy(tmpDepthQuoteData.InstrumentID,"rb1707");	
    strcpy(tmpDepthQuoteData.ExchangeID,"SHFE");
    strcpy(tmpDepthQuoteData.ExchangeInstID,"rb1707");	
    tmpDepthQuoteData.PreSettlementPrice=3594.000000;		
    tmpDepthQuoteData.PreClosePrice=3565.000000;
    tmpDepthQuoteData.PreOpenInterest	=954.000000;
    tmpDepthQuoteData.PreDelta=0.000000;
    tmpDepthQuoteData.OpenPrice=3538.000000;
    tmpDepthQuoteData.HighestPrice=3558.000000;
    tmpDepthQuoteData.LowestPrice=3478.000000;
    tmpDepthQuoteData.ClosePrice=3536.000000;
    tmpDepthQuoteData.UpperLimitPrice=3845.000000;
    tmpDepthQuoteData.LowerLimitPrice=3342.000000;
    tmpDepthQuoteData.SettlementPrice=3525.000000;
    tmpDepthQuoteData.CurrDelta=0.000000;
    tmpDepthQuoteData.LastPrice=3536.000000;
    tmpDepthQuoteData.Volume=322;
    tmpDepthQuoteData.Turnover=11352560.000000;
    tmpDepthQuoteData.OpenInterest=1006.000000;
    tmpDepthQuoteData.BidPrice1=3501.000000;
    tmpDepthQuoteData.BidVolume1=1;
    tmpDepthQuoteData.AskPrice1=3545.000000;
    tmpDepthQuoteData.AskVolume1=1;
    tmpDepthQuoteData.BidPrice2=0.000000;
    tmpDepthQuoteData.BidPrice3=0.000000;
    tmpDepthQuoteData.BidPrice4=0.000000;
    tmpDepthQuoteData.BidPrice5=0.000000;
    tmpDepthQuoteData.BidVolume2=0;
    tmpDepthQuoteData.BidVolume3=0;
    tmpDepthQuoteData.BidVolume4=0;
    tmpDepthQuoteData.BidVolume5=0;
    tmpDepthQuoteData.AskPrice2=0.000000;
    tmpDepthQuoteData.AskPrice3=0.000000;
    tmpDepthQuoteData.AskPrice4=0.000000;
    tmpDepthQuoteData.AskPrice5=0.000000;
    tmpDepthQuoteData.AskVolume2=0;
    tmpDepthQuoteData.AskVolume3=0;
    tmpDepthQuoteData.AskVolume4=0;
    tmpDepthQuoteData.AskVolume5=0;
    strcpy(tmpDepthQuoteData.UpdateTime,"15:28:39");
    tmpDepthQuoteData.UpdateMillisec=500;
    tmpDepthQuoteData.AveragePrice=3525.639752;	
    ptestRemMarket->m_RemMarkrtManager->OnQuoteUpdated(chInstrumentType,&tmpDepthQuoteData);	
    bool bRet = true;

    if(strcmp(tmpDepthQuoteData.InstrumentID,"rb1707")!=0)
    {
        bRet = false;
    }
	
    ASSERT_TRUE(bRet);		
    ptestRemMarket->Stop();	
    delete ptestRemMarket;
    delete m_book_sender;
    delete pFileConfig;	
}

/*TEST_F(CMutRemMarket, CMutRemMarket_Test005)
{
    
    std::string FileConfigstr= "rem_config.ini";	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender(save_url_f, save_url_s);	
    CRemMarket *ptestRemMarket = new CRemMarket(m_book_sender);  
    	
    ptestRemMarket->SetFileConfigData(FileConfigstr); 	

    ptestRemMarket->m_RemMarkrtManager->OnEqsDisconnected();	
	
    bool bRet = ptestRemMarket->Start();
    ASSERT_TRUE(bRet);	
    ptestRemMarket->Stop();	
    delete ptestRemMarket;	
}*/

} // namespace market
} // namespace rem
} // namespace fh