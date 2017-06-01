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
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:2557", "tcp://*:2558"); 
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
    EXPECT_EQ(0,(int)(ptestFemasMarket->Start()));	
    delete ptestFemasMarket;	
    delete m_book_sender;  
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test002)
{
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:3557", "tcp://*:3558"); 
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 	
    bool bRet = ptestFemasMarket->Start();
    ASSERT_TRUE(!bRet);	
    delete ptestFemasMarket;
    delete m_book_sender;  	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test003)
{
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:4557", "tcp://*:4558"); 
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);	
    //bool bRet = ptestFemasMarket->Start();
    std::vector<std::string> insts;
    insts.clear();	
    insts.push_back("cu1711P35500");	
    insts.push_back("HO1706-P-2450");	
    insts.push_back("SR801P7400");		
    ptestFemasMarket->Subscribe(insts);
    int i=0;
    CUstpFtdcSpecificInstrumentField tmpSpecificInstrument1;
    memset(&tmpSpecificInstrument1,0,sizeof(CUstpFtdcSpecificInstrumentField));
    strcpy(tmpSpecificInstrument1.InstrumentID,"cu1711P35500");	

    CUstpFtdcSpecificInstrumentField tmpSpecificInstrument2;
    memset(&tmpSpecificInstrument2,0,sizeof(CUstpFtdcSpecificInstrumentField));
    strcpy(tmpSpecificInstrument2.InstrumentID,"HO1706-P-2450");	

    CUstpFtdcSpecificInstrumentField tmpSpecificInstrument3;
    memset(&tmpSpecificInstrument3,0,sizeof(CUstpFtdcSpecificInstrumentField));
    strcpy(tmpSpecificInstrument3.InstrumentID,"SR801P7400");		
	
    ptestFemasMarket->m_FemasMarketManager->OnRspSubMarketData(&tmpSpecificInstrument1, NULL, i++, false);
    ptestFemasMarket->m_FemasMarketManager->OnRspSubMarketData(&tmpSpecificInstrument2, NULL, i++, false);
    ptestFemasMarket->m_FemasMarketManager->OnRspSubMarketData(&tmpSpecificInstrument3, NULL, i++, false);	
	
    EXPECT_EQ(0,ptestFemasMarket->m_FemasMarketManager->mISubSuss);	
    delete ptestFemasMarket;
    delete m_book_sender;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test004)
{
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:6557", "tcp://*:6558"); 
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
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
    delete m_book_sender;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test005)
{
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:7557", "tcp://*:7558"); 
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);
  
    std::vector<std::string> insts;
    insts.clear();			
    ptestFemasMarket->Subscribe(insts);	
    EXPECT_EQ(1,ptestFemasMarket->m_FemasMarketManager->mISubSuss);
    delete ptestFemasMarket;	
    delete m_book_sender;	
}

TEST_F(CMutFemasMarket, CMutFemasMarket_Test006)
{
    fh::core::book::BookSender *m_book_sender = new fh::core::book::BookSender("tcp://*:8557", "tcp://*:8558");  
    CFemasMarket *ptestFemasMarket = new CFemasMarket(m_book_sender);  
    std::string FileConfigstr= "femas_config.ini";	
    ptestFemasMarket->m_pFileConfig = new fh::core::assist::Settings(FileConfigstr); 
    ptestFemasMarket->m_FemasMarketManager->SetFileData(FileConfigstr);

    CUstpFtdcDepthMarketDataField tmpMarketData;
    memset(&tmpMarketData,0,sizeof(CUstpFtdcDepthMarketDataField));
    strcpy(tmpMarketData.InstrumentID,"rb1804");	
    strcpy(tmpMarketData.TradingDay,"20170531");
    strcpy(tmpMarketData.SettlementGroupID,"00000001");	
    tmpMarketData.SettlementID=1;
    tmpMarketData.PreSettlementPrice=1720.0;	
    tmpMarketData.PreClosePrice=1720.0;
    tmpMarketData.PreOpenInterest=64.0;
    tmpMarketData.PreDelta=0.0;
    tmpMarketData.OpenPrice=0.0;	
    tmpMarketData.HighestPrice=0.0;	
    tmpMarketData.LowestPrice=0.0;
    tmpMarketData.ClosePrice=0.0;
    tmpMarketData.UpperLimitPrice=1788.0;
    tmpMarketData.LowerLimitPrice=1651.0;
    tmpMarketData.SettlementPrice=0.0;
    tmpMarketData.CurrDelta=0.0;
    tmpMarketData.LastPrice=1720.0;
    tmpMarketData.Volume=0;
    tmpMarketData.Turnover=0.0;
    tmpMarketData.OpenInterest=64.0;
    tmpMarketData.BidPrice1=1651.0;
    tmpMarketData.BidVolume1=1;
    tmpMarketData.AskPrice1=0.0;
    tmpMarketData.AskVolume1=0.0;	

    tmpMarketData.BidPrice2=0.0;
    tmpMarketData.BidVolume2=0.0;
    tmpMarketData.AskPrice2=0.0;
    tmpMarketData.AskVolume2=0.0;		
    tmpMarketData.BidPrice3=0.0;
    tmpMarketData.BidVolume3=0.0;
    tmpMarketData.AskPrice3=0.0;
    tmpMarketData.AskVolume3=0.0;		
    tmpMarketData.BidPrice4=0.0;
    tmpMarketData.BidVolume4=0.0;
    tmpMarketData.AskPrice4=0.0;
    tmpMarketData.AskVolume4=0.0;		
    tmpMarketData.BidPrice5=0.0;
    tmpMarketData.BidVolume5=0.0;
    tmpMarketData.AskPrice5=0.0;
    tmpMarketData.AskVolume5=0.0;
    strcpy(tmpMarketData.UpdateTime,"10:13:18");	
    tmpMarketData.UpdateMillisec=500;	
    strcpy(tmpMarketData.ActionDay,"20170531");
    tmpMarketData.HisHighestPrice=0.0;
    tmpMarketData.HisLowestPrice=0.0;
    tmpMarketData.LatestVolume=0;
    tmpMarketData.InitVolume=0;
    tmpMarketData.ChangeVolume=0;
    tmpMarketData.BidImplyVolume=0;
    tmpMarketData.AskImplyVolume=0;
    tmpMarketData.AvgPrice=0.0;
    tmpMarketData.ArbiType='0';
    tmpMarketData.TotalBidVolume=0;
    tmpMarketData.TotalAskVolume=0;	
    	
	
	
	
    ptestFemasMarket->m_FemasMarketManager->OnRtnDepthMarketData(&tmpMarketData);
    bool ret=true;
    if(strcmp(tmpMarketData.InstrumentID,"rb1804") != 0)
    {
        ret = false;
    }
    ASSERT_TRUE(ret);	
    delete ptestFemasMarket;		
    delete m_book_sender;	
}


} // namespace market
} // namespace femas
} // namespace fh