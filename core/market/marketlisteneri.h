#ifndef FH_CORE_MARKET_MARKETLISTENERI_H
#define FH_CORE_MARKET_MARKETLISTENERI_H

#include "core/market/fwd.h"

namespace fh {
namespace core {
namespace market {

class MarketListenerI {
public:
  MarketListenerI() {};
  virtual ~MarketListenerI() {};

public:
  virtual void OnMarketDisconnect(core::market::MarketI* market) = 0;
  virtual void OnMarketReconnect(core::market::MarketI* market)  = 0;
  virtual void OnContractDefinition() = 0;  // (TODO) - add data definition
  virtual void OnBBO()   = 0;               // (TODO) - add data definition
  virtual void OnBid()   = 0;               // (TODO) - add data definition
  virtual void OnOffer() = 0;               // (TODO) - add data definition
  virtual void OnL2()    = 0;               // (TODO) - add data definition
  virtual void OnL3()    = 0;               // (TODO) - add data definition
  virtual void OnTrade() = 0;               // (TODO) - add data definition
};

} // market
} // core
} // fh
#endif // FH_CORE_MARKET_MARKETLISTENERI_H
