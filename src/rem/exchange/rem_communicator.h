#ifndef __FH_REM_EXCHANGE_GLOBX_H__
#define __FH_REM_EXCHANGE_GLOBX_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "EesTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"

namespace fh
{
namespace rem
{
namespace exchange
{



    class CEESTraderApiManger : public EESTraderEvent
    {
        public:
		   CEESTraderApiManger(EESTraderApi *pUserApi) :
                                                                  m_pUserApi (pUserApi) 
		   {
                      mIConnet = -1;
			MaxOrderLocalID = 0;		  
		   }
	          ~CEESTraderApiManger ()
	          {
                      delete m_pFileConfig;
		   }
	 public:
	 	   virtual void OnConnection(ERR_NO errNo, const char* pErrStr );
		   virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr );
		   virtual void OnUserLogon(EES_LogonResponse* pLogon);
		   virtual void OnRspChangePassword(EES_ChangePasswordResult nResult);
		   virtual void OnQueryUserAccount(EES_AccountInfo * pAccoutnInfo, bool bFinish);
		   virtual void OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish);
		   virtual void OnQueryAccountBP(const char* pAccount, EES_AccountBP* pAccoutnPosition, int nReqId );
                 virtual void OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish);
		   virtual void OnQueryAccountTradeMargin(const char* pAccount, EES_AccountMargin* pSymbolMargin, bool bFinish );
		   virtual void OnQueryAccountTradeFee(const char* pAccount, EES_AccountFee* pSymbolFee, bool bFinish );
		   virtual void OnOrderAccept(EES_OrderAcceptField* pAccept );
		   virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
		   virtual void OnOrderReject(EES_OrderRejectField* pReject );
		   virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
		   virtual void OnOrderExecution(EES_OrderExecutionField* pExec );
		   virtual void OnOrderCxled(EES_OrderCxled* pCxled );
		   virtual void OnCxlOrderReject(EES_CxlOrderRej* pReject );
		   virtual void OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish  );
		   virtual void OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish  );
		   virtual void OnPostOrder(EES_PostOrder* pPostOrder );
		   virtual void OnPostOrderExecution(EES_PostOrderExecution* pPostOrderExecution );
		   virtual void OnQueryMarketSession(EES_ExchangeMarketSession* pMarketSession, bool bFinish);
		   virtual void OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood);
		   virtual void OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus);
		   virtual void OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish);


                 void SendOrderAccept(EES_OrderAcceptField* pAccept);
		   void SendOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);	
		   void SendOrderReject(EES_OrderRejectField* pReject);
		   void SendOrderMarketReject(EES_OrderMarketRejectField* pReject);
		   void SendOrderExecution(EES_OrderExecutionField* pExec);	
		   void SendOrderCxled(EES_OrderCxled* pCxled);
		   void SendCxlOrderReject(EES_CxlOrderRej* pReject);
		   void SendQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder);
				  
		   void SetStrategy(core::exchange::ExchangeListenerI *strategy)
		   {
                      m_strategy = strategy;
		   }
		   void SetFileConfigData(const std::string &FileConfig);	
		   int mIConnet;
		   int MaxOrderLocalID;
		   
        private:
		   core::exchange::ExchangeListenerI *m_strategy;	 
		   EESTraderApi *m_pUserApi;
		   fh::core::assist::Settings *m_pFileConfig;
		   
    };

    class CRemGlobexCommunicator : public core::exchange::ExchangeI
    {
          public:
		   CRemGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file);
                 virtual ~CRemGlobexCommunicator();
          public:
                 // implement of ExchangeI
                bool Start(const std::vector<::pb::ems::Order> &init_orders) override;
                // implement of ExchangeI
                void Stop() override;

         public:
                // implement of ExchangeI
                void Initialize(std::vector<::pb::dms::Contract> contracts) override;
                // implement of ExchangeI
                void Add(const ::pb::ems::Order& order) override;
                 // implement of ExchangeI
                void Change(const ::pb::ems::Order& order) override;
                // implement of ExchangeI
                void Delete(const ::pb::ems::Order& order) override;
                // implement of ExchangeI
                void Query(const ::pb::ems::Order& order) override;
                // implement of ExchangeI
                void Query_mass(const char *data, size_t size) override;
                // implement of ExchangeI
                void Delete_mass(const char *data, size_t size) override;

				 
	  public:			 
                 CEESTraderApiManger* m_pEESTraderApiManger;
                 fh::core::assist::Settings *m_pFileConfig;
		   EESTraderApi *m_pUserApi;
		   int m_itimeout;
  

	  private:
                 core::exchange::ExchangeListenerI *m_strategy;
				 
		   std::vector<::pb::ems::Order> m_init_orders;		 

	  private:
	  	
                 DISALLOW_COPY_AND_ASSIGN(CRemGlobexCommunicator);			 

			
    };











}
}
}









#endif