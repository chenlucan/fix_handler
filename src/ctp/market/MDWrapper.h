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
	//登陆
	int login(CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID);
	//登出
	int logout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
	//订阅行情。
	//@param ppInstrumentID 合约ID  
	//@param nCount 要订阅/退订行情的合约个数
	//@remark 
	int ReqSubscribeMarketData(char *ppInstrumentID[], int nCount);
    bool isMdable(){return spi->isMdable();}
	void Initialize(std::vector<std::string> insts);
	
private:
	CThostFtdcMdApi *api;
	CustomMdSpi *spi;
	//账户信息
	std::shared_ptr<fh::ctp::market::MDAccountID> id;	
	std::vector<std::string> m_insts;
};
}
}
}