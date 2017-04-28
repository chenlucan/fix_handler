#include <unistd.h>
#include <time.h>
#include "rem_market.h"
#include "core/assist/logger.h"


namespace fh
{
namespace rem
{
namespace market
{



CRemMarket::CRemMarket(fh::core::market::MarketListenerI *listener)
: fh::core::market::MarketI(listener)
{
    m_pEESQuoteApi = CreateEESQuoteApi();
    m_RemMarkrtManager = new fh::rem::market::CRemMarkrtManager(m_pEESQuoteApi);
    if(NULL == m_pEESQuoteApi || NULL == m_RemMarkrtManager)
    {
          LOG_ERROR("Error m_pEESQuoteApi or m_RemMarkrtManager is NULL ");
	   return;	  
    }
    m_RemMarkrtManager->CreateRemBookManager(listener);
    m_itimeout = 10;	
}

CRemMarket::~CRemMarket()
{
    if(NULL != m_pEESQuoteApi)
    {
        DestroyEESQuoteApi(m_pEESQuoteApi);
    }
    
    delete m_RemMarkrtManager;	
    delete m_pFileConfig;	
}

// implement of MarketI
bool CRemMarket::Start()
{
     LOG_INFO("CRemMarket::Start() ");
     if(NULL == m_pEESQuoteApi)
     {
          LOG_ERROR("Error m_pEESQuoteApi is NULL ");
	   return false;	  
     }
     if(m_RemMarkrtManager->mIConnet != 0)
    {
          return false;
    }
     time_t tmtimeout = time(NULL);	 	 
     while(0 != m_RemMarkrtManager->mISubSuss)
     {
         if(time(NULL)-tmtimeout>m_itimeout)
	  {
              LOG_ERROR("CFemasMarket::mISubSuss tiomeout ");
		return false;	  
	  }
         sleep(0.1);  
     }	 
     LOG_INFO("CFemasMarket::mISubSuss is ok ");	 
    		 
     return true;	 
}
 // implement of MarketI
void CRemMarket::Initialize(std::vector<std::string> insts)
{
     LOG_INFO("CRemMarket::Initialize() ");
     if(NULL == m_pEESQuoteApi)
     {
          LOG_ERROR("Error m_pEESQuoteApi is NULL ");
	   return ;	  
     }


     m_itimeout = std::atoi((m_pFileConfig->Get("rem-timeout.timeout")).c_str());
	 
     EqsTcpInfo			info;
     std::vector<EqsTcpInfo>	vec_info; 
     vec_info.clear();	 
     memset(&info, 0, sizeof(EqsTcpInfo));
     strcpy(info.m_eqsId,m_pFileConfig->Get("rem-user.UserID").c_str());
     LOG_INFO("UserID=", info.m_eqsId);
     strcpy(info.m_eqsIp,m_pFileConfig->Get("rem-market.IP").c_str());
     LOG_INFO("market IP=", info.m_eqsIp);
     info.m_eqsPort = std::stoi(m_pFileConfig->Get("rem-market.Port"));	
     LOG_INFO("market Port=", info.m_eqsPort);
     vec_info.push_back(info);	 

     m_pEESQuoteApi->ConnServer(vec_info, m_RemMarkrtManager);	 

     time_t tmtimeout = time(NULL);
     while(0 != m_RemMarkrtManager->mIConnet)
     {
         if(time(NULL)-tmtimeout>m_itimeout)
	  {
              LOG_ERROR("CFemasMarket::mIConnet tiomeout ");
	       return;		  
	  }
         sleep(0.1);    
     }	 
     LOG_INFO("CFemasMarket::mIConnet is ok ");	 

     return;	 
	 
}
// implement of MarketI
void CRemMarket::Stop()
{
     LOG_INFO("CRemMarket::Release ");
     m_pEESQuoteApi->DisConnServer();	 
      
}
// implement of MarketI
void CRemMarket::Subscribe(std::vector<std::string> instruments)
{
     LOG_INFO("CRemMarket::Subscribe ");
     if(NULL == m_pEESQuoteApi)
     {
          return;
     }	
     if(instruments.size() <= 0)
     {
          m_RemMarkrtManager->mISubSuss = 1;	  
          m_pEESQuoteApi->QuerySymbolList();	  	  
     }	 
     else
     {
         //test 
         m_RemMarkrtManager->m_SubSymbol = instruments;
         m_RemMarkrtManager->mISubSuss = instruments.size();
         m_pEESQuoteApi->QuerySymbolList(); 
     }	
     return;	  
          	  	
}
// implement of MarketI
void CRemMarket::UnSubscribe(std::vector<std::string> instruments)
{
      LOG_INFO("CRemMarket::UnSubscribe ");
      return ;
}
 // implement of MarketI
void CRemMarket::ReqDefinitions(std::vector<std::string> instruments)
{
      LOG_INFO("CRemMarket::ReqDefinitions ");
      return ;
}


 void CRemMarket::SetFileConfigData(std::string &FileConfig)
 {
     LOG_INFO("CRemMarket::SetFileConfigData file = ",FileConfig.c_str());
     m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
     

     if(NULL == m_RemMarkrtManager)
     {
          LOG_ERROR("Error m_RemMarkrtManager is NULL ");
	   return;	  
     }

     m_RemMarkrtManager->SetFileData(FileConfig);	 
      	 
 }


}
}
}