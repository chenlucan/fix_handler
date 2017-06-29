#include <unistd.h>
#include <time.h>
#include <boost/container/flat_map.hpp>
#include "communicator.h"
#include "core/assist/logger.h"
 
namespace fh
{
namespace ctp
{
namespace exchange
{

CCtpCommunicator::CCtpCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy),m_itimeout(10)
{
     m_pFileConfig = new fh::core::assist::Settings(config_file); 
}

CCtpCommunicator::~CCtpCommunicator()
{
     delete m_pFileConfig;	 
}

bool CCtpCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
    LOG_INFO("CCtpCommunicator::Start");
    if(NULL == m_pFileConfig)
    {
        LOG_ERROR("Error m_pThostFtdcMdApi is NULL ");
	    return false;	  
    }
	
	auto accountID = std::make_shared<fh::ctp::exchange::AccountID>();
	accountID->setInvestorID(m_pFileConfig->Get("ctp-exchange.InvestorID"));
	accountID->setPassword(m_pFileConfig->Get("ctp-user.Password"));
	accountID->setBrokerID(m_pFileConfig->Get("ctp-user.BrokerID"));
	accountID->setExchangeFrontAddress(m_pFileConfig->Get("ctp-exchange.url"));
	accountID->setUserID(m_pFileConfig->Get("ctp-user.UserID"));
	accountID->setCombOffsetFlag(m_pFileConfig->Get("ctp-exchange.OffsetFlag"));
	accountID->setCombHedgeFlag(m_pFileConfig->Get("ctp-exchange.HedgeFlag"));
	accountID->setTimeCondition(m_pFileConfig->Get("ctp-exchange.TimeCondition"));
	accountID->setIsAutoSuspend(m_pFileConfig->Get("ctp-exchange.IsAutoSuspend"));
	accountID->setExchangeID(m_pFileConfig->Get("ctp-exchange.ExchangeID"));
	accountID->setVolumeCondition(m_pFileConfig->Get("ctp-exchange.HedgeFlag"));
	accountID->setForceCloseReason( m_pFileConfig->Get("ctp-exchange.HedgeFlag"));
	accountID->settimeout(m_pFileConfig->Get("ctp-timeout.timeout"));
	m_trader = std::make_shared<fh::ctp::exchange::CCtpTraderSpi>(m_strategy, accountID);
	time_t tmtimeout = time(NULL);
	while(true != m_trader->isTradable())
	{
		if(time(NULL)-tmtimeout>m_itimeout)
		{
			LOG_ERROR("tiomeout ");
		    break;		  
		}
		sleep(0.1);    
	}

	// 做完投资者结算结果确认操作，整个服务器连接与用户登录过程就完成了，可以正常下单交易了。
	if(m_trader->isTradable() != true)
	{ 
        LOG_ERROR("m_trader->isTradable() != true ");
		return false;
	}
		
	m_trader->m_InitQueryNum = init_orders.size();
	
	for(auto& it : init_orders)
	{
	    sleep(1); 
	    Query(it);  //报单查询		
	}
	
	tmtimeout = time(NULL);	
	int tmpQueryNum = m_trader->m_InitQueryNum;
	while(0 != m_trader->m_InitQueryNum)
	{
		if(time(NULL)-tmtimeout>m_itimeout)
		{
			LOG_ERROR("CCtpCommunicator::InitQuery tiomeout ");
		    return false;
		}
		
		if(tmpQueryNum != m_trader->m_InitQueryNum)
		{
			tmpQueryNum = m_trader->m_InitQueryNum;
			tmtimeout = time(NULL);		
		}
		sleep(0.1);  
	} 
	//check suss order
	SendReqQryTrade(init_orders);

	//check suss position
	SendReqQryInvestorPosition(init_orders);

	m_strategy->OnExchangeReady(boost::container::flat_map<std::string, std::string>());
	
	LOG_INFO("CCtpCommunicator::InitQuery is over ");	
	return true;
}

void CCtpCommunicator::Stop()
{
    LOG_INFO("CCtpCommunicator::Stop ");	
    // send message	    
    m_trader->reqUserLogout();
    return;
}


void CCtpCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
	// make Initialize	
	LOG_INFO("CCtpCommunicator::Initialize ");
	return;	
}

void CCtpCommunicator::Add(const ::pb::ems::Order& order)
{
	LOG_INFO("CCtpCommunicator::Add ");  
	if(NULL == m_trader)
	{
		return ;
	}	

	m_trader->reqOrderInsert(order);
	return;
}

void CCtpCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CCtpCommunicator::Change ");		
        return;
}

void CCtpCommunicator::Delete(const ::pb::ems::Order& order)
{
	LOG_INFO("CCtpCommunicator::Delete ");
    m_trader->reqOrderAction(order, THOST_FTDC_AF_Delete);
	return;
}

void CCtpCommunicator::Query(const ::pb::ems::Order& order)
{
	LOG_INFO("CCtpCommunicator::Query");

	// send message
	m_trader->reqQryOrder(order);	 	
	return;
}

void CCtpCommunicator::Query_mass(const char *data, size_t size)
{
    LOG_INFO("CCtpCommunicator::Query_mass ");
    return;
}

void CCtpCommunicator::Delete_mass(const char *data, size_t size)
{
    LOG_INFO("CCtpCommunicator::Delete_mass ");
    return;
}

void CCtpCommunicator::SendReqQryTrade(const std::vector<::pb::ems::Order> &init_orders)
{
    m_trader->reqQryTrade(init_orders);	
}

void CCtpCommunicator::SendReqQryInvestorPosition(const std::vector<::pb::ems::Order> &init_orders)
{
    LOG_INFO("CCtpCommunicator::SendReqQryInvestorPosition ");
    m_trader->queryPosition(init_orders);
}

}
}
}