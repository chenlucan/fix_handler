#ifndef FH_CORE_MARKET_MARKETI_H
#define FH_CORE_MARKET_MARKETI_H

#include "core/market/fwd.h"

#include <string>
#include <vector>

namespace fh {
namespace core {
namespace market {

typedef std::vector<std::string> InstrumentVec;

class MarketI {
public:
  MarketI(core::market::MarketListenerI *listener) {};
  virtual ~MarketI() {};

  virtual void Initialize(InstrumentVec insts) = 0;
  virtual bool Start() = 0;
  virtual void Stop()  = 0;

  virtual void Subscribe(InstrumentVec instruments)       = 0;
  virtual void UnSubscribe(InstrumentVec instruments)     = 0;
  virtual void ReqDefinitions(InstrumentVec instruments)  = 0;
};

} // market
} // core
} // fh
#endif // FH_CORE_MARKET_MARKETI_H
