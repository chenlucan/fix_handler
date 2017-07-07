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

typedef unsigned long long  ullong;

namespace fh
{
namespace ctp
{
namespace market
{
	
typedef struct strade
{
    int mvolume;
    std::string mtime;
    strade()
    {
        mvolume = 0;
		mtime = "";	
    }
} mstrade;

typedef std::map <std::string,mstrade*> TradeMap;	
	
// ---- 派生的行情类 ---- //
class CustomMdSpi: public CThostFtdcMdSpi
{
public:
	CustomMdSpi(std::shared_ptr<fh::core::market::MarketListenerI> sender, std::shared_ptr<fh::ctp::market::MDAccountID> id, CThostFtdcMdApi *api):m_last_volume(0),conn(false),mdable(false),m_api(api)
    {
		m_trademap.clear();	
		this->id = id;
		m_book_sender = sender;
	}	
	
	virtual ~CustomMdSpi(){}
	// ---- 继承自CTP父类的回调接口并实现 ---- //
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	void OnHeartBeatWarning(int nTimeLapse);

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅询价应答
	void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅询价应答
	void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///询价通知
	void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

// ---- 自定义函数 ---- //
private:	
	void SendDepthMarketData(CThostFtdcDepthMarketDataField *pMarketData);
	void StructToJSON(CThostFtdcDepthMarketDataField *pMarketData);
	std::string GetUpdateTimeStr(CThostFtdcDepthMarketDataField *pMarketData);
	ullong GetUpdateTimeInt(CThostFtdcDepthMarketDataField *pMarketData);
    ullong str2stmp(const char *strTime);	
	int MakePriceVolume(CThostFtdcDepthMarketDataField *pMarketData);
	void CheckTime(CThostFtdcDepthMarketDataField *pMarketData);
	
public:	
	bool isMdable(){ return mdable; };
	bool isConn(){ return conn; };
	
private:
    TThostFtdcVolumeType m_last_volume;
	std::shared_ptr<fh::core::market::MarketListenerI> m_book_sender;
	//账户信息
	std::shared_ptr<fh::ctp::market::MDAccountID> id;
	//是否连接成功 可以登录
	bool conn;	
	//是否可以进行订阅行情
	bool mdable;	
	CThostFtdcMdApi *m_api;
	TradeMap m_trademap;
};

}
}
}
#endif
