#ifndef __FH_FEMA_EXCHANGE_FEMASUSTPFTDCTRADERMANAGER_H__
#define __FH_FEMA_EXCHANGE_FEMASUSTPFTDCTRADERMANAGER_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include "USTPFtdcTraderApi.h"
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
namespace femas
{
namespace exchange
{
template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

    class CUstpFtdcTraderManger : public CUstpFtdcTraderSpi
    {
        public:
		   CUstpFtdcTraderManger(CUstpFtdcTraderApi *pUserApi) :
                                                                  m_pUserApi (pUserApi) 
		   {
                      mIConnet = -1;
			MaxOrderLocalID = 0;
			m_strategy = NULL;
			m_InitQueryNum = 0;
			m_ordermap.clear();
			m_startfinish = false;
		   }
	          ~CUstpFtdcTraderManger ()
	          {
                      delete m_pFileConfig;
		   }
	 public:
	 	   virtual void OnFrontConnected();
		   virtual void OnFrontDisconnected(int nReason);
		   virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast);
		   virtual void OnRspOrderInsert(CUstpFtdcInputOrderField  *pInputOrder, CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast);
		   virtual void OnRtnOrder(CUstpFtdcOrderField  *pOrder);
		   virtual void OnRspError(CUstpFtdcRspInfoField  *pRspInfo, int nRequestID, bool bIsLast);
		   virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade) ;
		   virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);
		   virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast); 
		   virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
		   virtual void OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
                  ///成交单查询应答
	           virtual void OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		    ///投资者持仓查询应答
	           virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		    ///合约查询应答
	           virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast); 	
		   
		   void OnQryOrder(CUstpFtdcOrderField *pOrder);
		   void SetFileConfigData(const std::string &FileConfig);
		   void OnInsertOrder(CUstpFtdcInputOrderField  *pInputOrder,CUstpFtdcRspInfoField  *pRspInfo);
		   void OnOrder(CUstpFtdcOrderField  *pOrder);
		   void OnFill(CUstpFtdcTradeField *pTrade);
		   void OnActionOrder(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
                  void OnQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		    void OnQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		    void OnQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument);	

                  void StructToJSON(CUstpFtdcRspInstrumentField *pRspInstrument);
		    void FemasDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,int VolumeMultiple);
			
		   
		   void SetStrategy(core::exchange::ExchangeListenerI *strategy)
		   {
                      m_strategy = strategy;
		   }
		   void AddOrderId(std::string cl_orderid,int i_key=-1);
		   std::string GetOrderId(int i_key);
                 void SetOrderStatus(CUstpFtdcOrderField *pOrder,::pb::ems::Order &tmporder);
		   
		   std::atomic_int mIConnet;
		   int MaxOrderLocalID;
		   std::atomic_int m_InitQueryNum;
		   std::map <int, std::string> m_ordermap;
		   bool m_startfinish;
		   
        private:
		   core::exchange::ExchangeListenerI *m_strategy;	 
		   CUstpFtdcTraderApi *m_pUserApi;
		   fh::core::assist::Settings *m_pFileConfig;
		   
        private:
                 DISALLOW_COPY_AND_ASSIGN(CUstpFtdcTraderManger);				   
		   
    };



}
}
}

#endif