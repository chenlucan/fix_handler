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

bool CFemasExchangeApp::Start()
{
     if(NULL == m_pCFemasGlobexCommunicator)
     {
         return false;
     }
     //m_pCFemasGlobexCommunicator->Start();	 
     return true;
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


}
}
}