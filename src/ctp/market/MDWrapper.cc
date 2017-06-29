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
	//��ʼ��api��spi����������
	api = CThostFtdcMdApi::CreateFtdcMdApi();
	spi = new CustomMdSpi(sender, id);
	//ע���¼�
	api->RegisterSpi(spi);
	//ע��ǰ�û�
	char *frontAddress = new char[100];
	strcpy(frontAddress, id->getMarketFrontAddress().c_str());
	std::cout << frontAddress << std::endl;
	api->RegisterFront(frontAddress);	
	//��ʼ��
	api->Init();
}

MDWrapper::~MDWrapper(){
	//�ͷ�api�ռ�
	api->RegisterSpi(nullptr);
	api->Release();
	api = nullptr;
	//�ͷ�spi�ռ�
	delete spi;
	spi = nullptr;
}

//��½
/*
0������ɹ���
-1����ʾ��������ʧ�ܣ�
-2����ʾδ�������󳬹��������
-3����ʾÿ�뷢�������������������
-4, ����δ�ɹ�
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

//�ǳ�
int MDWrapper::logout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID)
{
	return api->ReqUserLogout(pUserLogout, nRequestID);	 
}

//��������
int MDWrapper::ReqSubscribeMarketData(char *ppInstrumentID[], int nCount){
	return api->SubscribeMarketData(ppInstrumentID, nCount);
}
}
}
}
