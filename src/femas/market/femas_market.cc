#include <unistd.h>
#include <time.h>
#include "femas_market.h"



namespace fh
{
namespace femas
{
namespace market
{


CFemasMarket::CFemasMarket(fh::core::market::MarketListenerI *listener)
: fh::core::market::MarketI(listener)
{
    m_pUstpFtdcMduserApi = CUstpFtdcMduserApi::CreateFtdcMduserApi();
    m_FemasMarkrtManager = new fh::femas::market::CFemasMarkrtManager(m_pUstpFtdcMduserApi);
    if(NULL == m_pUstpFtdcMduserApi || NULL == m_FemasMarkrtManager)
    {
          printf("Error m_pUstpFtdcMduserApi or m_FemasMarkrtManager is NULL \n");
	   return;	  
    }	
    m_FemasMarkrtManager->CreateFemasBookManager(listener);	
    m_itimeout = 10;	
}

CFemasMarket::~CFemasMarket()
{
    delete m_FemasMarkrtManager;	
    delete m_pFileConfig;	
}

// implement of MarketI
bool CFemasMarket::Start()
{
     printf("CFemasMarket::Start() \n");
     if(NULL == m_pUstpFtdcMduserApi)
     {
          printf("Error m_pUstpFtdcMduserApi is NULL \n");
	   return false;	  
     }
     if(m_FemasMarkrtManager->mIConnet != 0)
    {
          return false;
    }
     time_t tmtimeout = time(NULL);	 	 
     while(0 != m_FemasMarkrtManager->mISubSuss)
     {
         if(time(NULL)-tmtimeout>m_itimeout)
	  {
              printf("CFemasMarket::mISubSuss tiomeout \n");
		return false;	  
	  }
         sleep(0.1);  
     }	 
     printf("CFemasMarket::mISubSuss is ok \n");		 
     return true;	 
}
 // implement of MarketI
void CFemasMarket::Initialize(std::vector<std::string> insts)
{
     printf("CFemasMarket::Initialize() \n");
	 
     if(NULL == m_pUstpFtdcMduserApi)
    {
          printf("Error m_pUstpFtdcMduserApi is NULL \n");
	   return ;	  
    }
      

    m_pUstpFtdcMduserApi->RegisterSpi(m_FemasMarkrtManager);


    if(insts.size() <= 0)
    {
          m_pUstpFtdcMduserApi->SubscribeMarketDataTopic (21001, USTP_TERT_RESUME);	  
    }
    else
    {
          for(int i=0;i<insts.size();i++)
          {
               printf("num = %d ,SubscribeMarketDataTopic ID = %d.\n",i+1,std::atoi(insts[i].c_str()));
               m_pUstpFtdcMduserApi->SubscribeMarketDataTopic (std::atoi(insts[i].c_str()), USTP_TERT_RESUME);
          }	
    }
    

	 
    std::string tmpurl = m_pFileConfig->Get("femas-market.url");
    printf("femas market url = %s \n",tmpurl.c_str());

    m_itimeout = std::atoi((m_pFileConfig->Get("femas-timeout.timeout")).c_str());
	
	 
    m_pUstpFtdcMduserApi->RegisterFront((char*)(tmpurl.c_str()));

     m_pUstpFtdcMduserApi->SetHeartbeatTimeout(m_itimeout); 

     m_pUstpFtdcMduserApi->Init();

     time_t tmtimeout = time(NULL);
     while(0 != m_FemasMarkrtManager->mIConnet)
     {
         if(time(NULL)-tmtimeout>m_itimeout)
	  {
              printf("CFemasMarket::mIConnet tiomeout \n");
	       return;		  
	  }
         sleep(0.1);    
     }	 
     printf("CFemasMarket::mIConnet is ok \n");	 

     return;	 
}
// implement of MarketI
void CFemasMarket::Stop()
{
     printf("CFemasMarket::Release \n");

     if(NULL == m_pUstpFtdcMduserApi)
     {
          printf("Error m_pUstpFtdcMduserApi is NULL \n");
	   return;	  
     }

     CUstpFtdcReqUserLogoutField reqUserLogout;

     std::string BrokerIDstr = m_pFileConfig->Get("femas-user.BrokerID");
     std::string UserIDstr = m_pFileConfig->Get("femas-user.UserID");
     strcpy(reqUserLogout.BrokerID, BrokerIDstr.c_str());
     printf("femas-user.BrokerID = %s.\n",reqUserLogout.BrokerID);
     strcpy(reqUserLogout.UserID, UserIDstr.c_str());
     printf("femas-user.UserID = %s.\n",reqUserLogout.UserID);
	 
	
     m_pUstpFtdcMduserApi->ReqUserLogout(&reqUserLogout,0);	 
	 
     m_pUstpFtdcMduserApi->Release();
      
}
// implement of MarketI
void CFemasMarket::Subscribe(std::vector<std::string> instruments)
{
     if(NULL == m_pUstpFtdcMduserApi)
      {
          return;
      }
      if(instruments.size() <= 0)
      {
          m_FemasMarkrtManager->mISubSuss = 1;
          char *contracts[1];
	   contracts[0] = new char[100];
          memset(contracts[0],0,100);
          strcpy(contracts[0], "*"); 	  
          m_pUstpFtdcMduserApi->SubMarketData (contracts,1);
	   delete []contracts[0];	  
          return;		  
      }
      else
      {
          m_FemasMarkrtManager->mISubSuss = instruments.size();
          char **contracts = new char*[instruments.size()];
          for(int i=0;i<instruments.size();i++)
	   {
               contracts[i] = new char[instruments[i].length()+1];
	        memset(contracts[i],0,instruments[i].length()+1);
	        strcpy(contracts[i],instruments[i].c_str());	
		 printf("num = %d,sub contracts = %s.\n",i+1,contracts[i]);	
	   }	   	  
	   m_pUstpFtdcMduserApi->SubMarketData (contracts,instruments.size());
	   for(int i=0;i<instruments.size();i++)
	   {
              delete [] contracts[i];   
	   }
	   delete [] contracts;
      }	  	
}
// implement of MarketI
void CFemasMarket::UnSubscribe(std::vector<std::string> instruments)
{
      return ;
}
 // implement of MarketI
void CFemasMarket::ReqDefinitions(std::vector<std::string> instruments)
{
      return ;
}


 void CFemasMarket::SetFileConfigData(std::string &FileConfig)
 {
     printf("CFemasMarket::SetFileConfigData file = %s \n",FileConfig.c_str());
     m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
     

     if(NULL == m_FemasMarkrtManager)
     {
          printf("Error m_FemasMarkrtManager is NULL \n");
	   return;	  
     }

     m_FemasMarkrtManager->SetFileData(FileConfig);	 
      	 
 }



}
}
}