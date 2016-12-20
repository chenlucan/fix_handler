#ifndef FH_CME_MARKET_CMEMARKET_H
#define FH_CME_MARKET_CMEMARKET_H

#include "core/market/fwd.h"
#include "core/market/marketi.h"

namespace fh {
namespace cme {
namespace market {

class CmeMarket : public core::market::MarketI {
public:
  CmeMarket(core::market::MarketListenerI *listener);
  virtual ~CmeMarket();

  virtual void Initialize(core::market::InstrumentVec insts) override;
  virtual bool Start() override;
  virtual void Stop()  override;

  virtual void Subscribe(core::market::InstrumentVec instruments)       override;
  virtual void UnSubscribe(core::market::InstrumentVec instruments)     override;
  virtual void ReqDefinitions(core::market::InstrumentVec instruments)  override;
};

} // market
} // cme
} // fh
#endif // FH_CME_MARKET_CMEMARKET_H
