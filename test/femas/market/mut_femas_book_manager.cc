#include "core/assist/logger.h"

#include "mut_femas_book_manager.h"

namespace fh
{
namespace femas
{
namespace market
{


CMutFemasBookManager::CMutFemasBookManager(fh::core::market::MarketListenerI *sender)
{
        // noop
        
}      


CMutFemasBookManager::~CMutFemasBookManager()
{
        // noop
}   




void CMutFemasBookManager::SendMutFemasmarketData(CUstpFtdcDepthMarketDataField *pMarketData)
{
	
	
}

void CMutFemasBookManager::SendMutFemasToDB(const std::string &message)
{
	
}

	
} // namespace market
} // namespace femas
} // namespace fh

