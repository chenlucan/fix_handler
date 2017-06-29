#include "rem_efhmarket.h"
#include <unistd.h>
#include <time.h>
#include "core/assist/logger.h"

namespace fh
{
namespace rem
{
namespace efhmarket
{


CRemEfhMarket::CRemEfhMarket(fh::core::market::MarketListenerI *listener)
: fh::core::market::MarketI(listener)	
{
    LOG_INFO("CRemEfhMarket::CRemEfhMarket");
    pRemEfhMarkrtManager = NULL;
    pRemEfhMarkrtManager = new CRemEfhMarkrtManager(listener);	
    m_pFileConfig = NULL;
	
}

CRemEfhMarket::~CRemEfhMarket()
{

}

bool CRemEfhMarket::Start()
{
    if(NULL == m_pFileConfig || NULL == pRemEfhMarkrtManager)
    {
        LOG_ERROR("CRemEfhMarket::Start has NULL");
	 return false;	
    }
    pRemEfhMarkrtManager->run();	
    return true;
}


void CRemEfhMarket::Initialize(std::vector<std::string> insts)
{
    return;
}

void CRemEfhMarket::Stop()
{

}

void CRemEfhMarket::Subscribe(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::UnSubscribe(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::ReqDefinitions(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::SetFileConfigData(std::string &FileConfig)
{
    LOG_INFO("CRemEfhMarket::SetFileConfigData file = ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
     

     if(NULL == pRemEfhMarkrtManager)
     {
          LOG_ERROR("Error m_FemasMarketManager is NULL ");
	   return;	  
     }

     pRemEfhMarkrtManager->SetFileData(FileConfig);	 		
}





}
}
}