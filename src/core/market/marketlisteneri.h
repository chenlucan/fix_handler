#ifndef CORE_MARKET_MARKETLISTENERI_H_
#define CORE_MARKET_MARKETLISTENERI_H_

#include "core/market/fwd.h"

namespace fh {
namespace core {
namespace market {

class MarketListenerI {
 public:
  MarketListenerI() {}
  virtual ~MarketListenerI() {}

 public:
  virtual void OnMarketDisconnect(core::market::MarketI* market) = 0;
  virtual void OnMarketReconnect(core::market::MarketI* market)  = 0;
  virtual void OnContractDefinition() = 0;
  virtual void OnBBO()   = 0;
  virtual void OnBid()   = 0;
  virtual void OnOffer() = 0;
  virtual void OnL2()    = 0;
  virtual void OnL3()    = 0;
  virtual void OnTrade() = 0;
};

}  // namespace market
}  // namespace core
}  // namespace fh
#endif  // CORE_MARKET_MARKETLISTENERI_H_
