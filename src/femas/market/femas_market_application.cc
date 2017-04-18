
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


bool CFemasMarketApp::Start()
{
    printf("CFemasMarketApp::Start() \n");

    m_pFemasMarket->Start();	

    return true;	
}

void CFemasMarketApp::Stop()
{
    printf("CFemasMarketApp::Stop() \n");
    m_pFemasMarket->Stop();	
}



}
}
}
