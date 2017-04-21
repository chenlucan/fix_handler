
#include "femas_market_application.h"


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
    printf("CFemasMarketApp::SetFileConfigData file = %s \n",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig);
    if(NULL == m_pFileConfig)
    {
          printf("Error m_pFileConfig is NULL \n");
	   return;	  
    }	
		
    m_pFemasMarket->SetFileConfigData(FileConfig);	
}

void CFemasMarketApp::Initialize(std::vector<std::string> insts)
{
    printf("CFemasMarketApp::Initialize() \n");
    m_pFemasMarket->Initialize(insts);
}

bool CFemasMarketApp::Start()
{
    printf("CFemasMarketApp::Start() \n");        
    	
    return m_pFemasMarket->Start();	
}

void CFemasMarketApp::Subscribe(std::vector<std::string> instruments)
{
    printf("CFemasMarketApp::Subscribe() \n");
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
    printf("CFemasMarketApp::Stop() \n");
    m_pFemasMarket->Stop();	
}



}
}
}
