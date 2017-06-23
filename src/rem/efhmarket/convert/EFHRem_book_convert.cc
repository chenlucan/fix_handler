#include "EFHRem_book_convert.h"
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
namespace efhmarket
{
namespace convert
{

EfhRemConvertListenerI::EfhRemConvertListenerI()
{

}

EfhRemConvertListenerI::~EfhRemConvertListenerI()
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnMarketDisconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnMarketReconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnContractDefinition(const pb::dms::Contract &contract)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnBBO(const pb::dms::BBO &bbo)
{
    m_bbo.Clear();
    m_bbo = bbo;
}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnBid(const pb::dms::Bid &bid)
{
    m_bid.Clear();
    m_bid = bid;
}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnOffer(const pb::dms::Offer &offer)
{
    m_offer.Clear();
    m_offer = offer;
}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnL2(const pb::dms::L2 &l2)
{
    m_l2.Clear();
    m_l2 = l2; 
}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnL3()
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnTrade(const pb::dms::Trade &trade)
{
    m_trade.Clear();
    m_trade = trade;
}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnContractAuctioning(const std::string &contract)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnContractNoTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnContractTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void EfhRemConvertListenerI::OnOrginalMessage(const std::string &message)
{
    LOG_INFO("FemasConvertListenerI::OnOrginalMessage");
}

// implement of MarketListenerI
void EfhRemConvertListenerI::OnTurnover(const pb::dms::Turnover &turnover)
{
    LOG_INFO("OnTurnover: ", fh::core::assist::utility::Format_pb_message(turnover));
    // TODO save it
}

void EfhRemConvertListenerI::Reset()
{
    LOG_INFO("FemasConvertListenerI::Reset");
    m_bbo.Clear();
    m_bid.Clear();
    m_offer.Clear();
    m_l2.Clear();
    m_trade.Clear();		
}

//===================================================================================
EfhRemBookConvert::EfhRemBookConvert()
{
    LOG_INFO("FemasBookConvert::FemasBookConvert");
    m_listener = new EfhRemConvertListenerI();
    m_efhrem_book_manager = new fh::rem::efhmarket::CRemEfhMarkrtManager((fh::core::market::MarketListenerI*)m_listener);	
    m_messagemap.clear();	
}

EfhRemBookConvert::~EfhRemBookConvert()
{
    LOG_INFO("FemasBookConvert::~FemasBookConvert");
    delete m_listener;
    delete m_efhrem_book_manager;	
}

MessMap EfhRemBookConvert::Convert(const std::string &message)
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
     EfhRemmarketData(body);
     bsoncxx::builder::basic::document tmp_l2;
     if(MakeL2Json(tmp_l2))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("trade")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_trade));	
	  m_messagemap["trade"] =  bsoncxx::to_json(tmp_h.view());
     }	 	 
   

     return m_messagemap;	 
}

void EfhRemBookConvert::EfhRemmarketData(const JSON_ELEMENT &message)
{
    LOG_INFO("FemasBookConvert::FemasmarketData");
    guava_udp_normal tmpMarketData;	
    memset(&tmpMarketData,0,sizeof(guava_udp_normal));
    tmpMarketData.m_sequence=GET_INT_FROM_JSON(message, "sequence");
    tmpMarketData.m_exchange_id=GET_CHAR_FROM_JSON(message, "exchange_id");	
    tmpMarketData.m_channel_id=GET_CHAR_FROM_JSON(message, "channel_id");	
    tmpMarketData.m_quote_flag=GET_CHAR_FROM_JSON(message, "quote_flag");		
    strcpy(tmpMarketData.m_symbol,GET_STR_FROM_JSON(message, "symbol").c_str());	
    strcpy(tmpMarketData.m_update_time,GET_STR_FROM_JSON(message, "update_time").c_str());
    tmpMarketData.m_millisecond=GET_INT_FROM_JSON(message, "millisecond");
    tmpMarketData.m_last_px=GET_DOUBLE_FROM_JSON(message, "last_px");	
    tmpMarketData.m_last_share=GET_INT_FROM_JSON(message, "last_share");
    tmpMarketData.m_total_value=GET_DOUBLE_FROM_JSON(message, "total_value");
    tmpMarketData.m_total_pos=GET_DOUBLE_FROM_JSON(message, "total_pos");
    tmpMarketData.m_bid_px=GET_DOUBLE_FROM_JSON(message, "bid_px");
    tmpMarketData.m_bid_share=GET_INT_FROM_JSON(message, "bid_share");
    tmpMarketData.m_ask_px=GET_DOUBLE_FROM_JSON(message, "ask_px");
    tmpMarketData.m_ask_share=GET_DOUBLE_FROM_JSON(message, "ask_share");		

    m_efhrem_book_manager->SendRemmarketData(&tmpMarketData);

    return;	
}

bool EfhRemBookConvert::MakeL2Json(bsoncxx::builder::basic::document& json)
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
bool EfhRemBookConvert::MakeBidJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("EfhRemBookConvert::MakeBidJson");
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
bool EfhRemBookConvert::MakeOfferJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("EfhRemBookConvert::MakeOfferJson");
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
bool EfhRemBookConvert::MakeBboJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("EfhRemBookConvert::MakeBboJson");
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

bool EfhRemBookConvert::MakeTradeJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("EfhRemBookConvert::MakeTradeJson");
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

} // namespace convert
} // namespace efhmarket
} // namespace rem
} // namespace fh
