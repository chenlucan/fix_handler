
#include "femas_market_application.h"
#include "core/assist/logger.h"

namespace fh
{
namespace femas
{
namespace market
{


CFemasMarketApp::CFemasMarketApp(std::string &save_url_f,std::string &save_url_s)
{	
    m_book_sender = new CFemasBookSender(save_url_f, save_url_s);
	
    m_pFemasMarket = new CFemasMarket(m_book_sender);

}

CFemasMarketApp::CFemasMarketApp(CFemasMarket *pFemasMarket)
{
    if(NULL == pFemasMarket)
    {
        return;
    }
    m_pFemasMarket = pFemasMarket;	
}


CFemasMarketApp::~CFemasMarketApp()
{
      delete m_book_sender;
      delete m_pFemasMarket;  
      delete m_pFileConfig;	  
}



void CFemasMarketApp::SetFileConfigData(std::string &FileConfig)
{
    LOG_INFO("CFemasMarketApp::SetFileConfigData file = ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig);
    if(NULL == m_pFileConfig)
    {
          LOG_ERROR("Error m_pFileConfig is NULL ");
	   return;	  
    }	
		
    m_pFemasMarket->SetFileConfigData(FileConfig);	
}

void CFemasMarketApp::Initialize(std::vector<std::string> insts)
{
    LOG_INFO("CFemasMarketApp::Initialize()");
    m_pFemasMarket->Initialize(insts);
}

bool CFemasMarketApp::Start()
{
    LOG_INFO("CFemasMarketApp::Start()");        
    	
    return m_pFemasMarket->Start();	
}

void CFemasMarketApp::Subscribe(std::vector<std::string> instruments)
{
    LOG_INFO("CFemasMarketApp::Subscribe()");
    m_pFemasMarket->Subscribe(instruments);	
}

void CFemasMarketApp::UnSubscribe(std::vector<std::string> instruments)
{
    m_pFemasMarket->UnSubscribe(instruments);
}

void CFemasMarketApp::ReqDefinitions(std::vector<std::string> instruments)
{
    m_pFemasMarket->ReqDefinitions(instruments);
}

void CFemasMarketApp::Stop()
{
    LOG_INFO("CFemasMarketApp::Stop()");
    m_pFemasMarket->Stop();	
}



}
}
}
