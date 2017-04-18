#ifndef CORE_MARKET_MOCK_MARKETLISTENERI_H_
#define CORE_MARKET_MOCK_MARKETLISTENERI_H_

#include "gmock/gmock.h"

#include "mock_fwd.h"
#include "pb/dms/dms.pb.h"


namespace fh {
namespace core {
namespace market {

class MockMarketListenerI :public MarketListenerI{
 public:
  MockMarketListenerI() {}
  virtual ~MockMarketListenerI() {}

 public:
  MOCK_METHOD1(OnMarketDisconnect, void(core::market::MarketI* market));
  MOCK_METHOD1(OnMarketReconnect, void(core::market::MarketI* market));
  MOCK_METHOD1(OnContractDefinition, void(const pb::dms::Contract &contract));
  MOCK_METHOD1(OnBBO, void(const pb::dms::BBO &bbo));
  MOCK_METHOD1(OnBid, void(const pb::dms::Bid &bid));
  MOCK_METHOD1(OnOffer, void(const pb::dms::Offer &offer));
  MOCK_METHOD1(OnL2, void(const pb::dms::L2 &l2));
  MOCK_METHOD0(OnL3, void());
  MOCK_METHOD1(OnTrade, void(const pb::dms::Trade &trade));
  
  MOCK_METHOD1(OnContractAuctioning, void(const std::string &contract));
  MOCK_METHOD1(OnContractNoTrading, void(const std::string &contract));
  MOCK_METHOD1(OnContractTrading, void(const std::string &contract));
  
  MOCK_METHOD1(OnOrginalMessage, void(const std::string &message));

};

}  // namespace market
}  // namespace core
}  // namespace fh
#endif  // CORE_MARKET_MOCK_MARKETLISTENERI_H_
