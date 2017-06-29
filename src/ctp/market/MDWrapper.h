#include "ThostFtdcMdApi.h"
#include "custom_md_spi.h"
#include "MDAccountID.h"
#include <time.h>

namespace fh
{
namespace ctp
{
namespace market
{
// --
class MDWrapper{
public:
	MDWrapper(std::shared_ptr<fh::core::market::MarketListenerI> sender, std::shared_ptr<fh::ctp::market::MDAccountID> id);
	~MDWrapper();
	//��½
	int login(CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID);
	//�ǳ�
	int logout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
	//�������顣
	//@param ppInstrumentID ��ԼID  
	//@param nCount Ҫ����/�˶�����ĺ�Լ����
	//@remark 
	int ReqSubscribeMarketData(char *ppInstrumentID[], int nCount);
    bool isMdable(){return spi->isMdable();}
	
private:
	CThostFtdcMdApi *api;
	CustomMdSpi *spi;
	//�˻���Ϣ
	std::shared_ptr<fh::ctp::market::MDAccountID> id;	
};
}
}
}