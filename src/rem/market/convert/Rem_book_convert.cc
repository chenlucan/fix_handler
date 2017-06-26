#include "Rem_book_convert.h"
#include "core/assist/logger.h"

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
namespace rem
{
namespace market
{
namespace convert
{

RemConvertListenerI::RemConvertListenerI()
{

}

RemConvertListenerI::~RemConvertListenerI()
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnMarketDisconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnMarketReconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnContractDefinition(const pb::dms::Contract &contract)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnBBO(const pb::dms::BBO &bbo)
{
    m_bbo.Clear();
    m_bbo = bbo;
}
// implement of MarketListenerI
void RemConvertListenerI::OnBid(const pb::dms::Bid &bid)
{
    m_bid.Clear();
    m_bid = bid;
}
// implement of MarketListenerI
void RemConvertListenerI::OnOffer(const pb::dms::Offer &offer)
{
    m_offer.Clear();
    m_offer = offer;
}
// implement of MarketListenerI
void RemConvertListenerI::OnL2(const pb::dms::L2 &l2)
{
    m_l2.Clear();
    m_l2 = l2; 
}
// implement of MarketListenerI
void RemConvertListenerI::OnL3()
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnTrade(const pb::dms::Trade &trade)
{
    m_trade.Clear();
    m_trade = trade;	
}
// implement of MarketListenerI
void RemConvertListenerI::OnContractAuctioning(const std::string &contract)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnContractNoTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnContractTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void RemConvertListenerI::OnOrginalMessage(const std::string &message)
{
    LOG_INFO("FemasConvertListenerI::OnOrginalMessage");
}

// implement of MarketListenerI
void RemConvertListenerI::OnTurnover(const pb::dms::Turnover &turnover)
{
    LOG_INFO("OnTurnover: ", fh::core::assist::utility::Format_pb_message(turnover));
    m_turnover.Clear();
    m_turnover = turnover;	
}

void RemConvertListenerI::Reset()
{
    LOG_INFO("FemasConvertListenerI::Reset");
    m_bbo.Clear();
    m_bid.Clear();
    m_offer.Clear();
    m_l2.Clear();
    m_trade.Clear();	
    m_turnover.Clear();	
}

//===================================================================================

RemBookConvert::RemBookConvert()
{
    LOG_INFO("FemasBookConvert::FemasBookConvert");
    m_listener = new RemConvertListenerI();
    m_rem_book_manager = new fh::rem::market::CRemBookManager((fh::core::market::MarketListenerI*)m_listener);	
    m_messagemap.clear();	
}

RemBookConvert::~RemBookConvert()
{
    LOG_INFO("FemasBookConvert::~FemasBookConvert");
    delete m_listener;
    delete m_rem_book_manager;	
}



MessMap RemBookConvert::Convert(const std::string &message)
{
     LOG_INFO("FemasBookConvert::Apply_message");
     LOG_INFO("FemasBookConvert::Apply_message str = ",message);	 
     m_messagemap.clear();
     m_listener->Reset();
	 
     auto doc = bsoncxx::from_json(message);	 
     auto json = doc.view();
     auto body = GET_SUB_FROM_JSON(json, "message");
     auto instrumentID = GET_STR_FROM_JSON(json, "InstrumentID");	
     auto is = GET_STR_FROM_JSON(json, "insertTime");
     auto se = GET_STR_FROM_JSON(json, "sendingTime");	
     auto sestr = GET_STR_FROM_JSON(json, "sendingTimeStr");	 
     RemmarketData(body);
     bsoncxx::builder::basic::document tmp_l2;
     if(MakeL2Json(tmp_l2))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("l2")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_l2));	
	  m_messagemap["l2"] =  bsoncxx::to_json(tmp_h.view());
     }
     bsoncxx::builder::basic::document tmp_bid;	 
     if(MakeBidJson(tmp_bid))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("bid")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_bid));	
	  m_messagemap["bid"] =  bsoncxx::to_json(tmp_h.view());
     }	 
     bsoncxx::builder::basic::document tmp_offer;		 
     if(MakeOfferJson(tmp_offer))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("offer")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_offer));
	  m_messagemap["offer"] =  bsoncxx::to_json(tmp_h.view());
     }	 
     bsoncxx::builder::basic::document tmp_bbo;		 
     if(MakeBboJson(tmp_bbo))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("bbo")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_bbo));	
	  m_messagemap["bbo"] =  bsoncxx::to_json(tmp_h.view());
     }	 
     bsoncxx::builder::basic::document tmp_trade;		 
     if(MakeTradeJson(tmp_trade))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("trade")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_trade));	
	  m_messagemap["trade"] =  bsoncxx::to_json(tmp_h.view());
     }	 
     bsoncxx::builder::basic::document tmp_turnover;		 
     if(MakeTurnoverJson(tmp_turnover))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REM")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("turnover")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_turnover));	
	  m_messagemap["turnover"] =  bsoncxx::to_json(tmp_h.view());
     }	 
   

     return m_messagemap;	 
}

void RemBookConvert::RemmarketData(const JSON_ELEMENT &message)
{
    LOG_INFO("FemasBookConvert::FemasmarketData");
    EESMarketDepthQuoteData tmpMarketData;	
    memset(&tmpMarketData,0,sizeof(EESMarketDepthQuoteData));
    strcpy(tmpMarketData.InstrumentID,GET_STR_FROM_JSON(message, "InstrumentID").c_str());
    strcpy(tmpMarketData.TradingDay,GET_STR_FROM_JSON(message, "TradingDay").c_str());
    strcpy(tmpMarketData.ExchangeID,GET_STR_FROM_JSON(message, "ExchangeID").c_str());
    strcpy(tmpMarketData.ExchangeInstID,GET_STR_FROM_JSON(message, "ExchangeInstID").c_str());
    tmpMarketData.PreSettlementPrice=GET_DOUBLE_FROM_JSON(message, "PreSettlementPrice");
    tmpMarketData.LastPrice=GET_DOUBLE_FROM_JSON(message, "LastPrice");	
    tmpMarketData.PreClosePrice=GET_DOUBLE_FROM_JSON(message, "PreClosePrice");
    tmpMarketData.PreOpenInterest=GET_INT_FROM_JSON(message, "PreOpenInterest");
    tmpMarketData.OpenPrice=GET_DOUBLE_FROM_JSON(message, "OpenPrice");
    tmpMarketData.HighestPrice=GET_DOUBLE_FROM_JSON(message, "HighestPrice");
    tmpMarketData.LowestPrice=GET_DOUBLE_FROM_JSON(message, "LowestPrice");	
    tmpMarketData.Volume=GET_INT_FROM_JSON(message, "PreOpenInterest");
    tmpMarketData.Turnover=GET_DOUBLE_FROM_JSON(message, "Turnover");
    tmpMarketData.OpenInterest=GET_INT_FROM_JSON(message, "OpenInterest");
    tmpMarketData.ClosePrice=GET_DOUBLE_FROM_JSON(message, "ClosePrice");
    tmpMarketData.SettlementPrice=GET_DOUBLE_FROM_JSON(message, "SettlementPrice");
    tmpMarketData.UpperLimitPrice=GET_DOUBLE_FROM_JSON(message, "UpperLimitPrice");
    tmpMarketData.LowerLimitPrice=GET_DOUBLE_FROM_JSON(message, "LowerLimitPrice");
    tmpMarketData.PreDelta=GET_DOUBLE_FROM_JSON(message, "PreDelta");
    tmpMarketData.CurrDelta=GET_DOUBLE_FROM_JSON(message, "CurrDelta");
    strcpy(tmpMarketData.UpdateTime,GET_STR_FROM_JSON(message, "UpdateTime").c_str());
    tmpMarketData.UpdateMillisec=GET_INT_FROM_JSON(message, "UpdateMillisec");	    
    tmpMarketData.BidPrice1=GET_DOUBLE_FROM_JSON(message, "BidPrice1");
    tmpMarketData.BidVolume1=GET_INT_FROM_JSON(message, "BidVolume1");
    tmpMarketData.AskPrice1=GET_DOUBLE_FROM_JSON(message, "AskPrice1");
    tmpMarketData.AskVolume1=GET_INT_FROM_JSON(message, "AskVolume1");	
    tmpMarketData.BidPrice2=GET_DOUBLE_FROM_JSON(message, "BidPrice2");
    tmpMarketData.BidVolume2=GET_INT_FROM_JSON(message, "BidVolume2");
    tmpMarketData.AskPrice2=GET_DOUBLE_FROM_JSON(message, "AskPrice2");
    tmpMarketData.AskVolume2=GET_INT_FROM_JSON(message, "AskVolume2");		
    tmpMarketData.BidPrice3=GET_DOUBLE_FROM_JSON(message, "BidPrice3");
    tmpMarketData.BidVolume3=GET_INT_FROM_JSON(message, "BidVolume3");
    tmpMarketData.AskPrice3=GET_DOUBLE_FROM_JSON(message, "AskPrice3");
    tmpMarketData.AskVolume3=GET_INT_FROM_JSON(message, "AskVolume3");		
    tmpMarketData.BidPrice4=GET_DOUBLE_FROM_JSON(message, "BidPrice4");
    tmpMarketData.BidVolume4=GET_INT_FROM_JSON(message, "BidVolume4");
    tmpMarketData.AskPrice4=GET_DOUBLE_FROM_JSON(message, "AskPrice4");
    tmpMarketData.AskVolume4=GET_INT_FROM_JSON(message, "AskVolume4");		
    tmpMarketData.BidPrice5=GET_DOUBLE_FROM_JSON(message, "BidPrice5");
    tmpMarketData.BidVolume5=GET_INT_FROM_JSON(message, "BidVolume5");
    tmpMarketData.AskPrice5=GET_DOUBLE_FROM_JSON(message, "AskPrice5");
    tmpMarketData.AskVolume5=GET_INT_FROM_JSON(message, "AskVolume5");	
 
    m_rem_book_manager->SendRemmarketData(&tmpMarketData);
	
    return;	
}

bool RemBookConvert::MakeL2Json(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("FemasBookConvert::MakeL2Json");
    if(!(m_listener->m_l2).has_contract())
    {
        return false;
    }
		
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_l2.contract()))); 
	
    bsoncxx::builder::basic::array tmarray_b;	    	
    for(int i=0;i<m_listener->m_l2.bid_size();i++)
    {
        bsoncxx::builder::basic::document tmjsona;
	 tmjsona.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_l2.bid(i).price())))); 	
	 tmjsona.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_l2.bid(i).size())))); 
	 tmarray_b.append(tmjsona);
    }
    json.append(bsoncxx::builder::basic::kvp("bid", tmarray_b));

    bsoncxx::builder::basic::array tmarray_a;	    	
    for(int i=0;i<m_listener->m_l2.offer_size();i++)
    {
        bsoncxx::builder::basic::document tmjsona;
	 tmjsona.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_l2.offer(i).price())))); 	
	 tmjsona.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_l2.offer(i).size())))); 
	 tmarray_a.append(tmjsona);
    }
	
    json.append(bsoncxx::builder::basic::kvp("offer", tmarray_a));	
	
    return true;	
}
bool RemBookConvert::MakeBidJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("RemBookConvert::MakeBidJson");
    if(!(m_listener->m_bid).has_contract())
    {
        return false;
    }		
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_bid.contract()))); 

    bsoncxx::builder::basic::document tmarray_b;
    tmarray_b.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bid.bid().price())))); 	
    tmarray_b.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bid.bid().size())))); 
    json.append(bsoncxx::builder::basic::kvp("bid", tmarray_b));
	
    return true;	
}
bool RemBookConvert::MakeOfferJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("RemBookConvert::MakeOfferJson");
    if(!(m_listener->m_offer).has_contract())
    {
        return false;
    }			
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_offer.contract()))); 

    bsoncxx::builder::basic::document tmarray_a;
    tmarray_a.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_offer.offer().price())))); 	
    tmarray_a.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_offer.offer().size())))); 
    json.append(bsoncxx::builder::basic::kvp("offer", tmarray_a));   

    return true;	
}
bool RemBookConvert::MakeBboJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("RemBookConvert::MakeBboJson");
    if(!(m_listener->m_bbo).has_contract())
    {
        return false;
    } 	
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_bbo.contract()))); 

    bsoncxx::builder::basic::document tmarray_b;
    tmarray_b.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bbo.bid().price())))); 	
    tmarray_b.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bbo.bid().size())))); 

    json.append(bsoncxx::builder::basic::kvp("bid", tmarray_b));

    bsoncxx::builder::basic::document tmarray_a;
    tmarray_a.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bbo.offer().price())))); 	
    tmarray_a.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bbo.offer().size())))); 

    json.append(bsoncxx::builder::basic::kvp("offer", tmarray_a));

    return true;
}

bool RemBookConvert::MakeTradeJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("RemBookConvert::MakeTradeJson");
    if(!(m_listener->m_trade).has_contract())
    {
        return false;
    } 	
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_trade.contract()))); 

    bsoncxx::builder::basic::document tmarray_b;
    tmarray_b.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_trade.last().price())))); 	
    tmarray_b.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_trade.last().size())))); 

    json.append(bsoncxx::builder::basic::kvp("last", tmarray_b));

    return true;
}

bool RemBookConvert::MakeTurnoverJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("RemBookConvert::MakeTurnoverJson");
    if(!(m_listener->m_turnover).has_contract())
    {
        return false;
    } 	
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_turnover.contract()))); 
    json.append(bsoncxx::builder::basic::kvp("total_volume", T(m_listener->m_turnover.total_volume()))); 
    json.append(bsoncxx::builder::basic::kvp("turnover", T(m_listener->m_turnover.turnover()))); 
	
    return true;
}


} // namespace convert
} // namespace rem
} // namespace femas
} // namespace fh
