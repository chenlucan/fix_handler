#ifndef CORE_MARKET_MOCK_MARKETI_H_
#define CORE_MARKET_MOCK_MARKETI_H_

#include "gmock/gmock.h"

#include <string>
#include <vector>
#include "core/market/marketi.h"
#include "mock_fwd.h"

namespace fh {
namespace core {
namespace market {

typedef std::vector<std::string> InstrumentVec;

class MockMarketI:public MarketI {
  public:  
  MOCK_METHOD1(Initialize, void(InstrumentVec insts));
  MOCK_METHOD0(Start, bool());
  MOCK_METHOD0(Join, bool());
  MOCK_METHOD0(Stop, void());

  MOCK_METHOD1(Subscribe, void(InstrumentVec instruments));
  MOCK_METHOD1(UnSubscribe, void(InstrumentVec instruments));
  MOCK_METHOD1(ReqDefinitions, void(InstrumentVec instruments));
};

}  // namespace market
}  // namespace core
}  // namespace fh
#endif  // CORE_MARKET_MOCK_MARKETI_H_