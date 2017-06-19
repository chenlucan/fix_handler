#include "rem_market_application.h"
#include "core/assist/logger.h"

namespace fh
{
namespace rem
{
namespace market
{







CRemMarketApp::CRemMarketApp(std::string &save_url_f,std::string &save_url_s)
{	
    m_book_sender = new fh::core::book::BookSender(save_url_f, save_url_s);
	
    m_pRemMarket = new CRemMarket(m_book_sender);

}

CRemMarketApp::CRemMarketApp(CRemMarket *pRemMarket)
{
    if(NULL == pRemMarket)
    {
        return;
    }
    m_pRemMarket = pRemMarket;	
}


CRemMarketApp::~CRemMarketApp()
{
      delete m_book_sender;
      delete m_pRemMarket;  
      delete m_pFileConfig;	  
}



void CRemMarketApp::SetFileConfigData(std::string &FileConfig)
{
    LOG_INFO("CRemMarketApp::SetFileConfigData file = ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig);
    if(NULL == m_pFileConfig)
    {
          LOG_ERROR("Error m_pFileConfig is NULL ");
	   return;	  
    }	
		
    m_pRemMarket->SetFileConfigData(FileConfig);	
}

void CRemMarketApp::Initialize(std::vector<std::string> insts)
{
    LOG_INFO("CRemMarketApp::Initialize()");
    m_pRemMarket->Initialize(insts);
}

bool CRemMarketApp::Start()
{
    LOG_INFO("CRemMarketApp::Start()");        
    	
    return m_pRemMarket->Start();	
}

void CRemMarketApp::Subscribe(std::vector<std::string> instruments)
{
    LOG_INFO("CRemMarketApp::Subscribe()");
    m_pRemMarket->Subscribe(instruments);	
}

void CRemMarketApp::UnSubscribe(std::vector<std::string> instruments)
{
    m_pRemMarket->UnSubscribe(instruments);
}

void CRemMarketApp::ReqDefinitions(std::vector<std::string> instruments)
{
    m_pRemMarket->ReqDefinitions(instruments);
}

void CRemMarketApp::Stop()
{
    LOG_INFO("CRemMarketApp::Stop()");
    m_pRemMarket->Stop();	
}







}
}
}

