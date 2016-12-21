#ifndef CME_MARKET_CMEMARKET_H_
#define CME_MARKET_CMEMARKET_H_

#include "core/market/fwd.h"
#include "core/market/marketi.h"

namespace fh {
namespace cme {
namespace market {

class CmeMarket : public core::market::MarketI {
 public:
  explicit CmeMarket(core::market::MarketListenerI *listener);
  virtual ~CmeMarket();

  void Initialize(core::market::InstrumentVec insts) override;
  bool Start() override;
  void Stop()  override;

  void Subscribe(core::market::InstrumentVec instruments)       override;
  void UnSubscribe(core::market::InstrumentVec instruments)     override;
  void ReqDefinitions(core::market::InstrumentVec instruments)  override;
};

}  // namespace market
}  // namespace cme
}  // namespace fh
#endif  // CME_MARKET_CMEMARKET_H_
