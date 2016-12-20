#ifndef CORE_MARKET_MARKETI_H_
#define CORE_MARKET_MARKETI_H_

#include <string>
#include <vector>

#include "core/market/fwd.h"

namespace fh {
namespace core {
namespace market {

typedef std::vector<std::string> InstrumentVec;

class MarketI {
 public:
  explicit MarketI(core::market::MarketListenerI *listener) {}
  virtual ~MarketI() {}

  virtual void Initialize(InstrumentVec insts) = 0;
  virtual bool Start() = 0;
  virtual void Stop()  = 0;

  virtual void Subscribe(InstrumentVec instruments)       = 0;
  virtual void UnSubscribe(InstrumentVec instruments)     = 0;
  virtual void ReqDefinitions(InstrumentVec instruments)  = 0;
};

}  // namespace market
}  // namespace core
}  // namespace fh
#endif  // CORE_MARKET_MARKETI_H_
