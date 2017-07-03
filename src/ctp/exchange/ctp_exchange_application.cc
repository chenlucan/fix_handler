#include "ctp_exchange_application.h"
#include "core/assist/logger.h"
#include <thread>



namespace fh
{
namespace ctp
{
namespace exchange
{

CCtpExchangeApp::CCtpExchangeApp(const std::string &app_setting_file)
{
    LOG_INFO("CCtpExchangeApp::CCtpExchangeApp");
    pFileConfig = new fh::core::assist::Settings(app_setting_file);
    std::string receive_url = pFileConfig->Get("strategy.receive_url");
    std::string send_url = pFileConfig->Get("strategy.send_url");
    LOG_INFO("CCtpExchangeApp::receive_url:",receive_url);	
    LOG_INFO("CCtpExchangeApp::send_url:",send_url);	
    m_strategy = new fh::core::strategy::StrategyCommunicator(send_url,receive_url );
    m_pCtpGlobexCommunicator = new CCtpCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCtpGlobexCommunicator);
    return;
}


CCtpExchangeApp::~CCtpExchangeApp()
{
	delete pFileConfig;
    delete m_pCtpGlobexCommunicator;
    delete m_strategy;	
    return;
}

bool CCtpExchangeApp::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     LOG_INFO("CCtpExchangeApp::Start");
     if(NULL == m_pCtpGlobexCommunicator)
     {
         LOG_ERROR("CCtpExchangeApp::Start");
         return false;
     }
     	 
     LOG_INFO("start ctp strategy thread");
     std::thread strategy_listener([this]{
            m_strategy->Start_receive();
        });
     strategy_listener.detach();
     		
     return m_pCtpGlobexCommunicator->Start(init_orders);	 
}

void CCtpExchangeApp::Stop()
{
     LOG_INFO("CCtpExchangeApp::Stop");
     if(NULL == m_pCtpGlobexCommunicator)
     {
         LOG_ERROR("CCtpExchangeApp::Stop");
         return ;
     }
     m_pCtpGlobexCommunicator->Stop();
     return;
}


//code test begin
void CCtpExchangeApp::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
        LOG_INFO("CCtpExchangeApp::Initialize");
        m_pCtpGlobexCommunicator->Initialize(contracts);
        return;
}

void CCtpExchangeApp::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CCtpExchangeApp::Add");
        m_pCtpGlobexCommunicator->Add(order);
        return;
}

void CCtpExchangeApp::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CCtpExchangeApp::Change");
        return;
}

void CCtpExchangeApp::Delete(const ::pb::ems::Order& order)
{
	m_pCtpGlobexCommunicator->Delete(order);
        LOG_INFO("CCtpExchangeApp::Delete");
        return;
}

void CCtpExchangeApp::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CCtpExchangeApp::Query");
        return;
}

void CCtpExchangeApp::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CCtpExchangeApp::Query_mass");
        return;
}

void CCtpExchangeApp::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CCtpExchangeApp::Delete_mass");
        return;
}

//code test end

}
}
}
