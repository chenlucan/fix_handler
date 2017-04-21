#include "femas_exchange_application.h"




namespace fh
{
namespace femas
{
namespace exchange
{

CFemasExchangeApp::CFemasExchangeApp(const std::string &app_setting_file)
{
    m_pCFemasGlobexCommunicator = new CFemasGlobexCommunicator(NULL,app_setting_file);
    return;
}


CFemasExchangeApp::~CFemasExchangeApp()
{
    delete m_pCFemasGlobexCommunicator;
    return;
}

bool CFemasExchangeApp::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     if(NULL == m_pCFemasGlobexCommunicator)
     {
         return false;
     }
     return m_pCFemasGlobexCommunicator->Start(init_orders);	 
}

void CFemasExchangeApp::Stop()
{
     if(NULL == m_pCFemasGlobexCommunicator)
     {
         return ;
     }
     m_pCFemasGlobexCommunicator->Stop();
     return;
}

void CFemasExchangeApp::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
        m_pCFemasGlobexCommunicator->Initialize(contracts);
        return;
}

void CFemasExchangeApp::Add(const ::pb::ems::Order& order)
{
        return;
}

void CFemasExchangeApp::Change(const ::pb::ems::Order& order)
{
        return;
}

void CFemasExchangeApp::Delete(const ::pb::ems::Order& order)
{
        return;
}

void CFemasExchangeApp::Query(const ::pb::ems::Order& order)
{
        return;
}

void CFemasExchangeApp::Query_mass(const char *data, size_t size)
{
        return;
}

void CFemasExchangeApp::Delete_mass(const char *data, size_t size)
{
        return;
}

}
}
}