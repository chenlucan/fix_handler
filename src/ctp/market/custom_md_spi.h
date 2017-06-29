#ifndef __FH_CUSTOM_MD_SPI_H__
#define __FH_CUSTOM_MD_SPI_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "ThostFtdcMdApi.h"
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#include "core/assist/utility.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/global.h"
#include <atomic>
#include "MDAccountID.h"

namespace fh
{
namespace ctp
{
namespace market
{
// ---- ������������ ---- //
class CustomMdSpi: public CThostFtdcMdSpi
{
public:
	CustomMdSpi(std::shared_ptr<fh::core::market::MarketListenerI> sender, std::shared_ptr<fh::ctp::market::MDAccountID> id):m_book_sender(sender),m_last_volume(0),conn(false),mdable(false)
    {
		this->id = id;
	}	
	
	virtual ~CustomMdSpi(){}
	// ---- �̳���CTP����Ļص��ӿڲ�ʵ�� ---- //
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	void OnHeartBeatWarning(int nTimeLapse);

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����ѯ��Ӧ��
	void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ������ѯ��Ӧ��
	void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///ѯ��֪ͨ
	void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

// ---- �Զ��庯�� ---- //
private:	
	void SendDepthMarketData(CThostFtdcDepthMarketDataField *pMarketData);
	void StructToJSON(CThostFtdcDepthMarketDataField *pMarketData);
	
public:	
	bool isMdable(){ return mdable; };
	bool isConn(){ return conn; };
	
private:
    TThostFtdcVolumeType m_last_volume;
	std::shared_ptr<fh::core::market::MarketListenerI> m_book_sender;
	//�˻���Ϣ
	std::shared_ptr<fh::ctp::market::MDAccountID> id;
	//�Ƿ����ӳɹ� ���Ե�¼
	bool conn;	
	//�Ƿ���Խ��ж�������
	bool mdable;	
};

}
}
}
#endif