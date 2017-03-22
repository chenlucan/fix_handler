#ifndef CORE_MARKET_MARKETLISTENERI_H_
#define CORE_MARKET_MARKETLISTENERI_H_

#include "core/market/fwd.h"
#include "pb/dms/dms.pb.h"


namespace fh {
namespace core {
namespace market {

class MarketListenerI {
 public:
  MarketListenerI() {}
  virtual ~MarketListenerI() {}

 public:
  virtual void OnMarketDisconnect(core::market::MarketI* market) = 0;
  virtual void OnMarketReconnect(core::market::MarketI* market)  = 0;
  virtual void OnContractDefinition(const pb::dms::Contract &contract) = 0;
  virtual void OnBBO(const pb::dms::BBO &bbo)   = 0;
  virtual void OnBid(const pb::dms::Bid &bid)   = 0;
  virtual void OnOffer(const pb::dms::Offer &offer) = 0;
  virtual void OnL2(const pb::dms::L2 &l2)    = 0;
  virtual void OnL3()    = 0;
  virtual void OnTrade(const pb::dms::Trade &trade) = 0;
};

}  // namespace market
}  // namespace core
}  // namespace fh
#endif  // CORE_MARKET_MARKETLISTENERI_H_
