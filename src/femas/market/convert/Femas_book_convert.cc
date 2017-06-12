#include "Femas_book_convert.h"
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include "core/assist/logger.h"
#include "core/market/marketlisteneri.h"

#define  GET_SUB_FROM_JSON(view, key) view[key]
#define  GET_STR_FROM_JSON(view, key) view[key].get_utf8().value.to_string()
#define  GET_ARR_FROM_JSON(view, key) view[key].get_array().value
#define  GET_CHAR_FROM_JSON(view, key) GET_STR_FROM_JSON(view, key).at(0)
#define  GET_INT_FROM_JSON(view, key) std::stol(GET_STR_FROM_JSON(view, key))
#define  GET_DOUBLE_FROM_JSON(view, key) std::stod(GET_STR_FROM_JSON(view, key))
#define  GET_INT_OR_DEFAULT_FROM_JSON(view, key, def) GET_SUB_FROM_JSON(view, key) ? GET_INT_FROM_JSON(view, key) : def
#define  GET_OPTIONAL_PRICE_FROM_JSON(target, view, key) { auto v = GET_SUB_FROM_JSON(view, #key); \
                                                                                                                    if(v) target.key = {GET_INT_FROM_JSON (v, "mantissa"), GET_INT_FROM_JSON (v, "exponent")}; }

namespace fh
{
namespace femas
{
namespace market
{
namespace convert
{

FemasConvertListenerI::FemasConvertListenerI()
{

}

FemasConvertListenerI::~FemasConvertListenerI()
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnMarketDisconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnMarketReconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnContractDefinition(const pb::dms::Contract &contract)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnBBO(const pb::dms::BBO &bbo)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnBid(const pb::dms::Bid &bid)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnOffer(const pb::dms::Offer &offer)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnL2(const pb::dms::L2 &l2)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnL3()
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnTrade(const pb::dms::Trade &trade)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnContractAuctioning(const std::string &contract)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnContractNoTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnContractTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnOrginalMessage(const std::string &message)
{

}



//===================================================================================
FemasBookConvert::FemasBookConvert()
{

}

FemasBookConvert::~FemasBookConvert()
{

}

void FemasBookConvert::Add_listener(fh::core::market::MarketListenerI *listener)
{
    LOG_INFO("FemasBookConvert::Add_listener");
    m_listener = listener;
}

void FemasBookConvert::Apply_message(const std::string &message)
{

}

void FemasBookConvert::FemasmarketData(const JSON_ELEMENT &message,int volumeMultiple=0)
{

}

} // namespace convert
} // namespace market
} // namespace femas
} // namespace fh