#ifndef CORE_EXCHANGE_EXCHANGEI_H
#define CORE_EXCHANGE_EXCHANGEI_H

#include <vector>

#include "pb/ems/ems.pb.h"

#include "core/exchange/fwd.h"

namespace fh {
namespace core {
namespace exchange {

class ExchangeI {
public:
  ExchangeI(core::exchange::ExchangeListenerI* listener) {};
  virtual ~ExchangeI() {};

public:
  // blocking call
  virtual void Initialize(fh::core::exchange::ContractVec contracts) = 0;
  // to verify these orders status with exchange
  virtual bool Start(PbOrderVec)  = 0;
  virtual void Stop()             = 0;

  virtual void Add(const ::pb::ems::Order& order)    = 0;
  virtual void Change(const ::pb::ems::Order& order) = 0;
  virtual void Delete(const ::pb::ems::Order& order) = 0;
};

} // exchange
} // core
} // fh
#endif // CORE_EXCHANGE_EXCHANGEI_H
