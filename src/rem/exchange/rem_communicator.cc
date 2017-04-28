#include <unistd.h>
#include <time.h>
#include "rem_communicator.h"
#include "core/assist/logger.h"


namespace fh
{
namespace rem
{
namespace exchange
{


void CEESTraderApiManger::OnConnection(ERR_NO errNo, const char* pErrStr )
{
    LOG_INFO("CEESTraderApiManger::OnConnection");
}
void CEESTraderApiManger::OnDisConnection(ERR_NO errNo, const char* pErrStr )
{
    LOG_INFO("CEESTraderApiManger::OnDisConnection");
}
void CEESTraderApiManger::OnUserLogon(EES_LogonResponse* pLogon)
{
    LOG_INFO("CEESTraderApiManger::OnUserLogon");
}
void CEESTraderApiManger::OnRspChangePassword(EES_ChangePasswordResult nResult)
{
    LOG_INFO("CEESTraderApiManger::OnRspChangePassword");
}
void CEESTraderApiManger::OnQueryUserAccount(EES_AccountInfo * pAccoutnInfo, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryUserAccount");
}
void CEESTraderApiManger::OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountPosition");
}
void CEESTraderApiManger::OnQueryAccountBP(const char* pAccount, EES_AccountBP* pAccoutnPosition, int nReqId )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountBP");
}
void CEESTraderApiManger::OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQuerySymbol");
}
void CEESTraderApiManger::OnQueryAccountTradeMargin(const char* pAccount, EES_AccountMargin* pSymbolMargin, bool bFinish )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountTradeMargin");
}
void CEESTraderApiManger::OnQueryAccountTradeFee(const char* pAccount, EES_AccountFee* pSymbolFee, bool bFinish )
{
    LOG_INFO("CEESTraderApiManger::OnQueryAccountTradeFee");
}
void CEESTraderApiManger::OnOrderAccept(EES_OrderAcceptField* pAccept )
{
    LOG_INFO("CEESTraderApiManger::OnOrderAccept");
}
void CEESTraderApiManger::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketAccept");
}
void CEESTraderApiManger::OnOrderReject(EES_OrderRejectField* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnOrderReject");
}
void CEESTraderApiManger::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
    LOG_INFO("CEESTraderApiManger::OnOrderMarketReject");
}
void CEESTraderApiManger::OnOrderExecution(EES_OrderExecutionField* pExec )
{
    LOG_INFO("CEESTraderApiManger::OnOrderExecution");
}
void CEESTraderApiManger::OnOrderCxled(EES_OrderCxled* pCxled )
{
    LOG_INFO("CEESTraderApiManger::OnOrderCxled");
}
void CEESTraderApiManger::OnCxlOrderReject(EES_CxlOrderRej* pReject )
{
    LOG_INFO("CEESTraderApiManger::OnCxlOrderReject");
}
void CEESTraderApiManger::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrder");
}
void CEESTraderApiManger::OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish  )
{
    LOG_INFO("CEESTraderApiManger::OnQueryTradeOrderExec");
}
void CEESTraderApiManger::OnPostOrder(EES_PostOrder* pPostOrder )
{
    LOG_INFO("CEESTraderApiManger::OnPostOrder");
}
void CEESTraderApiManger::OnPostOrderExecution(EES_PostOrderExecution* pPostOrderExecution )
{
    LOG_INFO("CEESTraderApiManger::OnPostOrderExecution");
}
void CEESTraderApiManger::OnQueryMarketSession(EES_ExchangeMarketSession* pMarketSession, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQueryMarketSession");
}
void CEESTraderApiManger::OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood)
{
    LOG_INFO("CEESTraderApiManger::OnMarketSessionStatReport");
}
void CEESTraderApiManger::OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus)
{
    LOG_INFO("CEESTraderApiManger::OnSymbolStatusReport");
}
void CEESTraderApiManger::OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish)
{
    LOG_INFO("CEESTraderApiManger::OnQuerySymbolStatus");
}

		   
void CEESTraderApiManger::SetFileConfigData(const std::string &FileConfig)
{
    LOG_INFO("CUstpFtdcTraderManger::SetFileConfigData file =  ",FileConfig.c_str());
    m_pFileConfig = new fh::core::assist::Settings(FileConfig); 
}










CRemGlobexCommunicator::CRemGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file)
	                                                                                                  :core::exchange::ExchangeI(strategy), m_strategy(strategy)
{

     m_pFileConfig = new fh::core::assist::Settings(config_file);
     m_pUserApi = CreateEESTraderApi();	 
     m_pEESTraderApiManger = new CEESTraderApiManger(m_pUserApi);

     m_pEESTraderApiManger->SetFileConfigData(config_file);
     m_itimeout = 10;
      
}

CRemGlobexCommunicator::~CRemGlobexCommunicator()
{
     
     delete m_pFileConfig;	 
     LOG_INFO("m_pUserApi::Release ");
     DestroyEESTraderApi(m_pUserApi);	 
}

bool CRemGlobexCommunicator::Start(const std::vector<::pb::ems::Order> &init_orders)
{
     if(m_pEESTraderApiManger->mIConnet != 0)
    {
        //return false;
    }
    return true;
}

void CRemGlobexCommunicator::Stop()
{
    LOG_INFO("CRemGlobexCommunicator::Stop ");
	
    return;
}


void CRemGlobexCommunicator::Initialize(std::vector<::pb::dms::Contract> contracts)
{
        // noop
         LOG_INFO("CRemGlobexCommunicator::Initialize ");   

         //m_pUserApi->ConnServer(const char * svrAddr, int nPort, m_pEESTraderApiManger);
		
         return;	
}

void CRemGlobexCommunicator::Add(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Add ");
        
        return;
}

void CRemGlobexCommunicator::Change(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Change ");
        return;
}

void CRemGlobexCommunicator::Delete(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Delete ");
        return;
}

void CRemGlobexCommunicator::Query(const ::pb::ems::Order& order)
{
        LOG_INFO("CRemGlobexCommunicator::Query ");
        return;
}

void CRemGlobexCommunicator::Query_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Query_mass ");
        return;
}

void CRemGlobexCommunicator::Delete_mass(const char *data, size_t size)
{
        LOG_INFO("CRemGlobexCommunicator::Delete_mass ");
        return;
}












}
}
}