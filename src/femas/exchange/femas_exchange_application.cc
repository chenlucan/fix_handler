#include "femas_exchange_application.h"
#include "core/assist/logger.h"
#include <thread>



namespace fh
{
namespace femas
{
namespace exchange
{

CFemasExchangeApp::CFemasExchangeApp(const std::string &app_setting_file)
{
    LOG_INFO("CFemasExchangeApp::CFemasExchangeApp");
    pFileConfig = new fh::core::assist::Settings(app_setting_file);
    std::string receive_url = pFileConfig->Get("strategy.receive_url");
    std::string send_url = pFileConfig->Get("strategy.send_url");
    LOG_INFO("CFemasExchangeApp::receive_url:",receive_url);	
    LOG_INFO("CFemasExchangeApp::send_url:",send_url);	
    m_strategy = new fh::core::strategy::StrategyCommunicator(send_url,receive_url );
    m_pCFemasGlobexCommunicator = new CFemasGlobexCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCFemasGlobexCommunicator);
    return;
}


CFemasExchangeApp::~CFemasExchangeApp()
{
    delete m_pCFemasGlobexCommunicator;
    delete pFileConfig;
    delete m_strategy;	
    return;
}

bool CFemasExchangeApp::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     LOG_INFO("CFemasExchangeApp::Start");
     if(NULL == m_pCFemasGlobexCommunicator)
     {
         LOG_ERROR("CFemasExchangeApp::Start");
         return false;
     }
     	 
     LOG_INFO("start femas strategy thread");
     std::thread strategy_listener([this]{
            m_strategy->Start_receive();
        });
     strategy_listener.detach();
     		
     return m_pCFemasGlobexCommunicator->Start(init_orders);	 
}

void CFemasExchangeApp::Stop()
{
     LOG_INFO("CFemasExchangeApp::Stop");
     if(NULL == m_pCFemasGlobexCommunicator)
     {
         LOG_ERROR("CFemasExchangeApp::Stop");
         return ;
     }
     m_pCFemasGlobexCommunicator->Stop();
     return;
}


//code test begin
void CFemasExchangeApp::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
        LOG_INFO("CFemasExchangeApp::Initialize");
        m_pCFemasGlobexCommunicator->Initialize(contracts);
        return;
}

void CFemasExchangeApp::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasExchangeApp::Add");
        m_pCFemasGlobexCommunicator->Add(order);
        return;
}

void CFemasExchangeApp::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasExchangeApp::Change");
        return;
}

void CFemasExchangeApp::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasExchangeApp::Delete");
        return;
}

void CFemasExchangeApp::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CFemasExchangeApp::Query");
        return;
}

void CFemasExchangeApp::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasExchangeApp::Query_mass");
        return;
}

void CFemasExchangeApp::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CFemasExchangeApp::Delete_mass");
        return;
}

//code test end

}
}
}