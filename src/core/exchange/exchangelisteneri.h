#ifndef CORE_EXCHANGE_EXCHANGELISTENERI_H
#define CORE_EXCHANGE_EXCHANGELISTENERI_H

#include <boost/container/flat_map.hpp>
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"
#include "core/exchange/fwd.h"

namespace fh {
namespace core {
namespace exchange {

class ExchangeListenerI {
public:
  ExchangeListenerI() {};
  virtual ~ExchangeListenerI() {};

public:
  virtual void OnOrder(const ::pb::ems::Order &order)  = 0;
  virtual void OnFill(const ::pb::ems::Fill &fill)     = 0;
  virtual void OnPosition(const core::exchange::PositionVec&) = 0;

  virtual void OnContractDefinition(const pb::dms::Contract &contract) = 0;

  virtual void OnContractAuctioning(const std::string &contract)   = 0;
  virtual void OnContractNoTrading(const std::string &contract)    = 0;
  virtual void OnContractTrading(const std::string &contract)      = 0;

  typedef boost::container::flat_map<std::string, std::string> ValueMap;
  virtual void OnExchangeReady(ValueMap)         = 0;

  virtual void OnOrginalMessage(const std::string &message) = 0;
};

} // exchange
} // core
} // fh
#endif // CORE_EXCHANGE_EXCHANGELISTENERI_H
