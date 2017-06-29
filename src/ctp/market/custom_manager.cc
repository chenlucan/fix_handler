#include <unistd.h>
#include <time.h>
#include "custom_manager.h"
#include "core/assist/logger.h"
#include <fstream>

namespace fh
{
namespace ctp
{
namespace market
{


CustomManager::CustomManager(std::shared_ptr<fh::core::market::MarketListenerI> listener, std::shared_ptr<MDAccountID> id):fh::core::market::MarketI(listener.get()), subscribeOK(false)
{
	this->id = id;
    m_ctpmaket = std::make_shared<MDWrapper>(listener, id);	
	m_itimeout = std::atoi(id->getTimeout().c_str());
    m_insts.clear();	
}

CustomManager::~CustomManager()
{
	printf("CustomManager::~CustomManager()\n");
}

// implement of MarketI
bool CustomManager::Start()
{
	LOG_INFO("CustomManager::Start() ");
	
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, id->getBrokerID().c_str());
	strcpy(loginReq.UserID, id->getUserID().c_str());
	strcpy(loginReq.Password, id->getPassword().c_str());
	LOG_INFO("ctp-user.BrokerID = ",loginReq.BrokerID);
	LOG_INFO("ctp-user.UserID = ",loginReq.UserID);
	LOG_INFO("ctp-user.Passwor = ",loginReq.Password);
	int requestID = 0; // 请求编号
	int rt = m_ctpmaket->login(&loginReq, requestID);
	std::cout << rt << std::endl;
	if(rt != 0)
	{
		std::cout << "log fail!"<< std::endl;
	}	
	
	time_t tmtimeout = time(NULL);
	while(true != m_ctpmaket->isMdable())
	{
		if(time(NULL)-tmtimeout>m_itimeout)
		{
			LOG_ERROR("CustomManager::mIConnet tiomeout ");
			return false;		  
		}
	sleep(0.1);    
	}	 
	LOG_INFO("CustomManager::mIConnet is ok ");

	Subscribe(m_insts);
	m_insts.clear();	 
	if (subscribeOK)
	{
		std::cout << ">>>>>>发送订阅行情请求成功" << std::endl;
		return true;
	}
	else
	{
		std::cerr << "--->>>发送订阅行情请求失败" << std::endl;
		return false;
	}		 
}
 // implement of MarketI
void CustomManager::Initialize(std::vector<std::string> insts)
{
     LOG_INFO("CustomManager::Initialize() ");
     m_insts = insts;	      
     return;	 
}
// implement of MarketI
void CustomManager::Stop()
{
	LOG_INFO("CustomManager::Release ");

	CThostFtdcUserLogoutField reqUserLogout;
	strcpy(reqUserLogout.BrokerID, id->getBrokerID().c_str());
	LOG_INFO("ctp-user.BrokerID = ",reqUserLogout.BrokerID);
	strcpy(reqUserLogout.UserID, id->getUserID().c_str());
	LOG_INFO("ctp-user.UserID = ",reqUserLogout.UserID);
	int requestID = 100;
	m_ctpmaket->logout(&reqUserLogout, requestID);	 

}
// implement of MarketI
void CustomManager::Subscribe(std::vector<std::string> instruments)
{	
	if(instruments.size() <= 0)
	{
		return;		  
	}
	else
	{
		char **contracts = new char*[instruments.size()];
		for(unsigned int i=0;i<instruments.size();i++)
		{
			contracts[i] = new char[instruments[i].length()+1];
			memset(contracts[i],0,instruments[i].length()+1);
			strcpy(contracts[i],instruments[i].c_str());	
		//	LOG_INFO("num = ",i+1,",sub contracts = ",contracts[i]);	
		}	   	  
		int rt = m_ctpmaket->ReqSubscribeMarketData(contracts,instruments.size());
		for(unsigned int i=0;i<instruments.size();i++)
		{
			delete [] contracts[i];   
		}
		delete [] contracts;
		
		if (!rt)
		{
			subscribeOK = true;
		}
		else
		{
			subscribeOK = false;
		}		
	}	  	
}
// implement of MarketI
void CustomManager::UnSubscribe(std::vector<std::string> instruments)
{
	return ;
}
 // implement of MarketI
void CustomManager::ReqDefinitions(std::vector<std::string> instruments)
{
	return ;
}

}
}
}