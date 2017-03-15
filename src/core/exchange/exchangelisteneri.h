#ifndef CORE_EXCHANGE_EXCHANGELISTENERI_H
#define CORE_EXCHANGE_EXCHANGELISTENERI_H
#include "boost/container/flat_map.hpp"
#include "build/pb/ems/ems.pb.h"

#include "core/exchange/fwd.h"

namespace fh {
namespace core {
namespace exchange {

class ExchangeListenerI {
public:
  ExchangeListenerI() {};
  virtual ~ExchangeListenerI() {};

public:
  virtual void OnOrder(::pb::ems::Order order)  = 0;
  virtual void OnFill(::pb::ems::Fill fill)     = 0;
  virtual void OnPosition(core::pb::PositionVec) = 0;

  typedef boost::container::flat_map<std::string, std::string> ValueMap;
  virtual void OnExchangeReady(ValueMap)         = 0;
};

} // exchange
} // core
} // fh
#endif // CORE_EXCHANGE_EXCHANGELISTENERI_H
