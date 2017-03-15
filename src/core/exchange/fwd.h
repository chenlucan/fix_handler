#ifndef CORE_EXCHANGE_FWD_H
#define CORE_EXCHANGE_FWD_H

#include <memory>

#include "build/pb/ems/ems.pb.h"

namespace fh {
namespace core {
namespace exchange {

class ExchangeI;
typedef std::unique_ptr<ExchangeI>  ExchangePtr;

class ExchangeListenerI;

typedef std::vector<::pb::dms::Contract> ContractVec;
typedef std::vector<::pb::ems::Order> PbOrderVec;
typedef std::vector<std::string> InstrumentVec;
typedef std::function<void()> ExchangeDisconnected;
} // exchange
} // core
} // fh
#endif  // CORE_EXCHANGE_FWD_H
