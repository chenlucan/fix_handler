#include "rem_efhmarket.h"
#include <unistd.h>
#include <time.h>
#include "core/assist/logger.h"

namespace fh
{
namespace rem
{
namespace efhmarket
{


CRemEfhMarket::CRemEfhMarket(fh::core::market::MarketListenerI *listener)
: fh::core::market::MarketI(listener)	
{
    m_pFileConfig = NULL;
}

CRemEfhMarket::~CRemEfhMarket()
{

}

bool CRemEfhMarket::Start()
{
    return true;
}


void CRemEfhMarket::Initialize(std::vector<std::string> insts)
{

}

void CRemEfhMarket::Stop()
{

}

void CRemEfhMarket::Subscribe(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::UnSubscribe(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::ReqDefinitions(std::vector<std::string> instruments)
{

}

void CRemEfhMarket::SetFileConfigData(std::string &FileConfig)
{

}





}
}
}