#include "ctp_market_application.h"
#include "core/assist/logger.h"

namespace fh
{
namespace ctp
{
namespace market
{


CCtpMarketApp::CCtpMarketApp(std::string &FileConfig)
{	
    fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfig);
	if(NULL == pFileConfig)
    {
       LOG_ERROR("Error pFileConfig is NULL ");
	   return;	  
    }	
	
	auto MDAccountID = std::make_shared<fh::ctp::market::MDAccountID>();
	MDAccountID->setPassword(pFileConfig->Get("ctp-user.Password"));
	MDAccountID->setBrokerID(pFileConfig->Get("ctp-user.BrokerID"));
	MDAccountID->setMarketFrontAddress(pFileConfig->Get("ctp-market.url"));
	MDAccountID->setUserID(pFileConfig->Get("ctp-user.UserID"));
	MDAccountID->settimeout(pFileConfig->Get("ctp-timeout.timeout"));	
	m_instrumentFileDir = pFileConfig->Get("ctp-market.InstrumentFileDir");	
    std::string save_url_f = pFileConfig->Get("zeromq.org_url");
    std::string save_url_s = pFileConfig->Get("zeromq.book_url");	
    m_book_sender = std::make_shared<fh::core::book::BookSender>(save_url_f, save_url_s);
    m_pCtpManager = std::make_shared<fh::ctp::market::CustomManager>(m_book_sender, MDAccountID);
	
    delete pFileConfig;	
}

void CCtpMarketApp::Initialize(std::vector<std::string> insts)
{
    LOG_INFO("CCtpMarketApp::Initialize()");
	std::string  ignore;
	std::string insts_for_file;
	std::ifstream out(m_instrumentFileDir);
	getline(out, ignore);
	while(!out.eof())
	{
		getline(out,insts_for_file);
		m_insts.push_back(insts_for_file);
	}
	out.close();
	m_insts.pop_back();

    m_pCtpManager->Initialize(m_insts);
}

bool CCtpMarketApp::Start()
{
    LOG_INFO("CCtpMarketApp::Start()");            	
    return m_pCtpManager->Start();	
}

void CCtpMarketApp::Subscribe(std::vector<std::string> instruments)
{
    LOG_INFO("CCtpMarketApp::Subscribe()");
    m_pCtpManager->Subscribe(instruments);	
}

void CCtpMarketApp::UnSubscribe(std::vector<std::string> instruments)
{
    m_pCtpManager->UnSubscribe(instruments);
}

void CCtpMarketApp::ReqDefinitions(std::vector<std::string> instruments)
{
    m_pCtpManager->ReqDefinitions(instruments);
}

void CCtpMarketApp::Stop()
{
    LOG_INFO("CCtpMarketApp::Stop()");
    m_pCtpManager->Stop();	
}



}
}
}
