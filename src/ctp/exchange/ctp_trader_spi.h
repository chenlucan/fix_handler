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
				std::cout << "Initializing account " << id->getInvestorID() << "information;" << std::endl;
				// �����ļ����洢���к�Լ����,��������ģ���ȡ
				char filePath[100] = "InstrumentID.csv";
				std::ofstream outFile;
				outFile.open(filePath, std::ios::out); // �¿��ļ�
				outFile << "��Լ����"<< std::endl;
				outFile.close();
				readyToTrade();
		    }

		// ---- ctp_api���ֻص��ӿ� ---- //	
	    public:
			// ���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
            virtual void OnFrontConnected();

			// ��¼������Ӧ
			virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// Ͷ���߽�����ȷ����Ӧ
			virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// �����ѯͶ���ֲ߳���Ӧ
			virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// ����¼��������Ӧ
			virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// ��������������Ӧ
			virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

			// �����ѯ��Լ��Ӧ
	        virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			
			// ����Ӧ��
			virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			
			// ���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
			virtual void OnFrontDisconnected(int nReason);

			// ����֪ͨ
			virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

			// �ɽ�֪ͨ
			virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
			// �����ѯ�ɽ���Ӧ
	        virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	        // ����¼�����ر�
			virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);			
			
			// ������������ر�
	        virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
			// �����ѯ������Ӧ
			virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
        // ---- �Զ��庯�� ---- //
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
		    void reqQryOrder(const ::pb::ems::Order& order); //�����ѯ����			
			void reqOrderInsert(const ::pb::ems::Order& order); //����¼������			
			void reqQryTrade(const std::vector<::pb::ems::Order> &orders);
			void reqUserLogin(); //�û���¼����
			void reqUserLogout(); //�û��˳�����
			void reqOrderAction(const ::pb::ems::Order& order, TThostFtdcActionFlagType ActionFlag); //������������		
 		    void OnQryInstrument(CThostFtdcInstrumentField *pRspInstrument);
			void CtpDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,int VolumeMultiple);
        private:
		    //������������
	        void increaseRef(){ maxOrderRef++;}
		 
        public:
            std::atomic_int m_InitQueryNum;
			
        private:
		   //����api
	       CThostFtdcTraderApi *api;
		   core::exchange::ExchangeListenerI *m_strategy;
		   //������
		   int requestID;
		   //�Ƿ���Խ��н���
	       bool tradable;	
		   std::vector<::pb::dms::Contract> m_contracts;		   	  
		   //�˻���Ϣ
	       std::shared_ptr<AccountID> id;
		   //���ص���󱨵�����
	       int maxOrderRef;
	       std::map<int, std::string> client_orders_index;
		public:
		   //api�������
		   fh::ctp::exchange::CommandQueue commandQueue;

        private:
           DISALLOW_COPY_AND_ASSIGN(CCtpTraderSpi);				   
		   
    };



}
}
}

#endif