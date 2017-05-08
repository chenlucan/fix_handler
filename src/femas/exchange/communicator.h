#ifndef __FH_FEMA_EXCHANGE_GLOBX_H__
#define __FH_FEMA_EXCHANGE_GLOBX_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "USTPFtdcTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"

namespace fh
{
namespace femas
{
namespace exchange
{

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
		   void OnQryOrder(CUstpFtdcOrderField *pOrder);
		   void SetFileConfigData(const std::string &FileConfig);
		   void OnInsertOrder(CUstpFtdcInputOrderField  *pInputOrder,CUstpFtdcRspInfoField  *pRspInfo);
		   void OnOrder(CUstpFtdcOrderField  *pOrder);
		   void OnFill(CUstpFtdcTradeField *pTrade);
		   void OnActionOrder(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
		   void SetStrategy(core::exchange::ExchangeListenerI *strategy)
		   {
                      m_strategy = strategy;
		   }
		   int mIConnet;
		   int MaxOrderLocalID;
		   int m_InitQueryNum;
		   
        private:
		   core::exchange::ExchangeListenerI *m_strategy;	 
		   CUstpFtdcTraderApi *m_pUserApi;
		   fh::core::assist::Settings *m_pFileConfig;
		   
    };

    class CFemasGlobexCommunicator : public core::exchange::ExchangeI
    {
          public:
		   CFemasGlobexCommunicator(core::exchange::ExchangeListenerI *strategy,const std::string &config_file);
                 virtual ~CFemasGlobexCommunicator();
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
                 CUstpFtdcTraderManger* m_pUstpFtdcTraderManger;
                 fh::core::assist::Settings *m_pFileConfig;
		   CUstpFtdcTraderApi *m_pUserApi;
		   int m_itimeout;
  

	  private:
                 core::exchange::ExchangeListenerI *m_strategy;
				 
		   std::vector<::pb::ems::Order> m_init_orders;		 

	  private:
	  	
                 DISALLOW_COPY_AND_ASSIGN(CFemasGlobexCommunicator);			 

			
    };



}
}
}









#endif