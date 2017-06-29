#include "MDWrapper.h"
#include "iostream"
namespace fh
{
namespace ctp
{
namespace market
{
// --
MDWrapper::MDWrapper(std::shared_ptr<fh::core::market::MarketListenerI> sender, std::shared_ptr<fh::ctp::market::MDAccountID> id){
	this->id = id;
	//初始化api和spi并尝试连接
	api = CThostFtdcMdApi::CreateFtdcMdApi();
	spi = new CustomMdSpi(sender, id);
	//注册事件
	api->RegisterSpi(spi);
	//注册前置机
	char *frontAddress = new char[100];
	strcpy(frontAddress, id->getMarketFrontAddress().c_str());
	std::cout << frontAddress << std::endl;
	api->RegisterFront(frontAddress);	
	//初始化
	api->Init();
}

MDWrapper::~MDWrapper(){
	//释放api空间
	api->RegisterSpi(nullptr);
	api->Release();
	api = nullptr;
	//释放spi空间
	delete spi;
	spi = nullptr;
}

//登陆
/*
0，代表成功。
-1，表示网络连接失败；
-2，表示未处理请求超过许可数；
-3，表示每秒发送请求数超过许可数。
-4, 连接未成功
*/
int MDWrapper::login(CThostFtdcReqUserLoginField *pReqAuthenticateField, int nRequestID){
     time_t tmtimeout = time(NULL);
	 while(true != spi->isConn())
     {
		if(time(NULL)-tmtimeout > std::atoi(id->getTimeout().c_str()))
		{
			std::cout << "CCtpMarket::mIConnet tiomeout " << std::endl;
			return -4;		  
		}
         sleep(0.1);    
     }	 
     std::cout <<"CCtpMarket::mIConnet is ok " << std::endl;	
	
	 return api->ReqUserLogin(pReqAuthenticateField, nRequestID);
}

//登出
int MDWrapper::logout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID)
{
	return api->ReqUserLogout(pUserLogout, nRequestID);	 
}

//订阅行情
int MDWrapper::ReqSubscribeMarketData(char *ppInstrumentID[], int nCount){
	return api->SubscribeMarketData(ppInstrumentID, nCount);
}
}
}
}
