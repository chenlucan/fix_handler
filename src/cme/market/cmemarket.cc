#include "cme/market/cmemarket.h"

namespace fh {
namespace cme {
namespace market {

CmeMarket::CmeMarket(core::market::MarketListenerI *listener) : core::market::MarketI(listener) {}
CmeMarket::~CmeMarket() {}

void CmeMarket::Initialize(core::market::InstrumentVec insts) {}
bool CmeMarket::Start() { return false; }
void CmeMarket::Stop()  {}

void CmeMarket::Subscribe(core::market::InstrumentVec instruments) {}
void CmeMarket::UnSubscribe(core::market::InstrumentVec instruments) {}
void CmeMarket::ReqDefinitions(core::market::InstrumentVec instruments) {}

}  // namespace market
}  // namespace cme
}  // namespace fh
