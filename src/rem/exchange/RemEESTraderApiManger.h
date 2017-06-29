#ifndef __FH_REM_EXCHANGE_REMEESTRADERAPIMANAGER_H__
#define __FH_REM_EXCHANGE_REMEESTRADERAPIMANAGER_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include "EesTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/assist/utility.h"
#include "core/global.h"
#include <atomic>

namespace fh
{
namespace rem
{
namespace exchange
{
template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

    class CEESTraderApiManger : public EESTraderEvent
    {
        public:
		   CEESTraderApiManger(EESTraderApi *pUserApi) :
                                                                  m_pUserApi (pUserApi) 
		   {
                      mIConnet = -1;
			MaxOrderLocalID = 0;
			m_InitQueryNum = 0;
			m_orderTokenmap.clear();
			m_ordermap.clear();
			m_startfinish = false;
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
		   void SendQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish);
		   void SendQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish);
		   void SendQuerySymbol(EES_SymbolField* pSymbol);
		   
		   void StructToJSON(EES_SymbolField *pSymbol);
		   void RemDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,int VolumeMultiple);
				  
		   void SetStrategy(core::exchange::ExchangeListenerI *strategy)
		   {
                      m_strategy = strategy;
		   }
		   void SetFileConfigData(const std::string &FileConfig);	
		   void AddOrderId(std::string cl_orderid,int i_key);
		   void AddOrderToken(int MarketOrderToken,int i_key);
		   std::string GetOrderId(int i_key);
		   int GetOrderToken(int i_key);
		   std::atomic_int mIConnet;
		   int MaxOrderLocalID;
		   std::atomic_int m_InitQueryNum;
		   // ClientOrderToken , cl_order_id
		   std::map <int, std::string> m_ordermap;
		   // OrderToken , ClientOrderToken
		   std::map <int, int> m_orderTokenmap;
		   bool m_startfinish;
		   
        private:
		   core::exchange::ExchangeListenerI *m_strategy;	 
		   EESTraderApi *m_pUserApi;
		   fh::core::assist::Settings *m_pFileConfig;
		   
    };

}
}
}
#endif