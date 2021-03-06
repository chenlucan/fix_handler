#ifndef __FH_CTP_EXCHANGE_CTPTHOSTFTDCTRADERMANAGER_H__
#define __FH_CTP_EXCHANGE_CTPTHOSTFTDCTRADERMANAGER_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <fstream>
#include <map>
#include "ThostFtdcTraderApi.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"
#include <atomic>
#include <string.h>
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"
#include "core/assist/settings.h"
#include "core/exchange/exchangelisteneri.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/assist/utility.h"
#include "core/global.h"
#include "ApiCommand.h"
#include "ComfirmSettlementCommand.h"
#include "AccountID.h"
#include "InsertOrderCommand.h"
#include "LoginCommand.h"
#include "LoginOutCommand.h"
#include "ReqQryTradeCommand.h"
#include "ReqQryInstrumentCommand.h"
#include "QueryOrderCommand.h"
#include "QueryPositionCommand.h"
#include "WithdrawOrderCommand.h"
#include "CommandQueue.h"
#include <boost/make_shared.hpp>

namespace fh
{
namespace ctp
{
namespace exchange
{
	template <typename IntType>
	inline std::string T(IntType v){return std::to_string(v);}
	inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
	inline std::string T(const char *v){return std::string(v);}
	inline std::string T(char *v){return std::string(v);}

    class CCtpTraderSpi : public CThostFtdcTraderSpi
    {
        public:
		    CCtpTraderSpi(core::exchange::ExchangeListenerI *strategy, std::shared_ptr<AccountID> id) :
                                                                  m_strategy(strategy),requestID(0),tradable(false)
		    {
				m_InitQueryNum = 0;
                this->id = id;
				// 创建文件来存储所有合约代码,编译行情模块获取
				char filePath[100] = "InstrumentID.csv";
				std::ofstream outFile;
				outFile.open(filePath, std::ios::out); // 新开文件
				outFile << "合约代码"<< std::endl;
				outFile.close();
				readyToTrade();
		    }

		// ---- ctp_api部分回调接口 ---- //	
	    public:
			// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
            virtual void OnFrontConnected();

			// 登录请求响应
			virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// 投资者结算结果确认响应
			virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// 请求查询投资者持仓响应
			virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// 报单录入请求响应
			virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// 报单操作请求响应
			virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// 请求查询合约响应
	        virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			
			// 错误应答
			virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			
			// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
			virtual void OnFrontDisconnected(int nReason);

			// 报单通知
			virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

			// 成交通知
			virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
			// 请求查询成交响应
	        virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	        // 报单录入错误回报
			virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);			
			
			// 报单操作错误回报
	        virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
			// 请求查询报单响应
			virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
        // ---- 自定义函数 ---- //
		public:
		    char* itoa(int val, char *buf, unsigned radix);
		    void login();
		    void queryPosition(const std::vector<::pb::ems::Order> &init_orders);
		    bool isTradable(){ return tradable; };
            void readyToTrade();		
            void comfirmSettlement();			
			void reqQueryInstrument();
			void Initialize(std::vector<::pb::dms::Contract> contracts);
			void OnOrder(CThostFtdcOrderField  *pOrder);
			void OnActionOrder(CThostFtdcInputOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
		    void OnFill(CThostFtdcTradeField *pTrade);
		    void OnInsertOrder(CThostFtdcInputOrderField  *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
			void OnQryOrder(CThostFtdcOrderField *pOrder);
		    void OnQryInvestorPosition(CThostFtdcInvestorPositionField *pRspInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			void SetOrderStatus(CThostFtdcOrderField *pOrder, ::pb::ems::Order &tmporder);
			void OnQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);	    
		    void reqQryOrder(const ::pb::ems::Order& order); //请求查询报单			
			void reqOrderInsert(const ::pb::ems::Order& order); //报单录入请求			
			void reqQryTrade(const std::vector<::pb::ems::Order> &orders);
			void reqUserLogin(); //用户登录请求
			void reqUserLogout(); //用户退出请求
			void reqOrderAction(const ::pb::ems::Order& order, TThostFtdcActionFlagType ActionFlag); //报单操作请求		
 		    void OnQryInstrument(CThostFtdcInstrumentField *pRspInstrument);
			void CtpDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,int VolumeMultiple);
        private:
		    //报单引用自增
	        void increaseRef(){ maxOrderRef++;}
		 
        public:
            std::atomic_int m_InitQueryNum;
			
        private:
		   //交易api
	       CThostFtdcTraderApi *api;
		   core::exchange::ExchangeListenerI *m_strategy;
		   //请求编号
		   int requestID;
		   //是否可以进行交易
	       bool tradable;	
		   std::vector<::pb::dms::Contract> m_contracts;		   	  
		   //账户信息
	       std::shared_ptr<AccountID> id;
		   //本地的最大报单引用
	       int maxOrderRef;
	       std::map<int, std::string> client_orders_index;
		public:
		   //api请求队列
		   fh::ctp::exchange::CommandQueue commandQueue;

        private:
           DISALLOW_COPY_AND_ASSIGN(CCtpTraderSpi);				   
		   
    };



}
}
}

#endif