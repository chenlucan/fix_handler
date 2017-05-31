#include "rem_exchange_application.h"
#include "core/assist/logger.h"
#include <thread>



namespace fh
{
namespace rem
{
namespace exchange
{

CRemExchangeApp::CRemExchangeApp(const std::string &app_setting_file)
{
    LOG_INFO("CRemExchangeApp::CFemasExchangeApp");
    pFileConfig = new fh::core::assist::Settings(app_setting_file);
    std::string receive_url = pFileConfig->Get("strategy.receive_url");
    std::string send_url = pFileConfig->Get("strategy.send_url");
    LOG_INFO("CRemExchangeApp::receive_url:",receive_url);	
    LOG_INFO("CRemExchangeApp::send_url:",send_url);	
    m_strategy = new fh::core::strategy::StrategyCommunicator(send_url,receive_url );
    m_pCRemGlobexCommunicator = new CRemCommunicator(m_strategy,app_setting_file);
    m_strategy->Set_exchange(m_pCRemGlobexCommunicator);
    return;
}


CRemExchangeApp::~CRemExchangeApp()
{
    delete m_pCRemGlobexCommunicator;
    delete pFileConfig;
    delete m_strategy;	
    return;
}

bool CRemExchangeApp::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     LOG_INFO("CRemExchangeApp::Start");
     if(NULL == m_pCRemGlobexCommunicator)
     {
         LOG_ERROR("CRemExchangeApp::Start");
         return false;
     }
     	 
     LOG_INFO("start rem strategy thread");
     std::thread strategy_listener([this]{
            m_strategy->Start_receive();
        });
     strategy_listener.detach();
     		
     return m_pCRemGlobexCommunicator->Start(init_orders);	 
}

void CRemExchangeApp::Stop()
{
     LOG_INFO("CRemExchangeApp::Stop");
     if(NULL == m_pCRemGlobexCommunicator)
     {
         LOG_ERROR("CRemExchangeApp::Stop");
         return ;
     }
     m_pCRemGlobexCommunicator->Stop();
     return;
}


//code test begin
void CRemExchangeApp::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
        LOG_INFO("CRemExchangeApp::Initialize");
        m_pCRemGlobexCommunicator->Initialize(contracts);
        return;
}

void CRemExchangeApp::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemExchangeApp::Add");
        m_pCRemGlobexCommunicator->Add(order);
        return;
}

void CRemExchangeApp::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemExchangeApp::Change");
        return;
}

void CRemExchangeApp::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemExchangeApp::Delete");
        return;
}

void CRemExchangeApp::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemExchangeApp::Query");
        return;
}

void CRemExchangeApp::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CRemExchangeApp::Query_mass");
        return;
}

void CRemExchangeApp::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CRemExchangeApp::Delete_mass");
        return;
}

//code test end


}
}
}