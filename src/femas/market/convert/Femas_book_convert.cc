#include "Femas_book_convert.h"
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
    m_bbo.Clear();
    m_bbo = bbo;
}
// implement of MarketListenerI
void FemasConvertListenerI::OnBid(const pb::dms::Bid &bid)
{
    m_bid.Clear();
    m_bid = bid;
}
// implement of MarketListenerI
void FemasConvertListenerI::OnOffer(const pb::dms::Offer &offer)
{
    m_offer.Clear();
    m_offer = offer;
}
// implement of MarketListenerI
void FemasConvertListenerI::OnL2(const pb::dms::L2 &l2)
{
    m_l2.Clear();
    m_l2 = l2; 
}
// implement of MarketListenerI
void FemasConvertListenerI::OnL3()
{

}
// implement of MarketListenerI
void FemasConvertListenerI::OnTrade(const pb::dms::Trade &trade)
{
    m_trade.Clear();
    m_trade = trade;
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
    LOG_INFO("FemasConvertListenerI::OnOrginalMessage");
}

void FemasConvertListenerI::Reset()
{
    LOG_INFO("FemasConvertListenerI::Reset");
    m_bbo.Clear();
    m_bid.Clear();
    m_offer.Clear();
    m_l2.Clear();
    m_trade.Clear();	
}

//===================================================================================
FemasBookConvert::FemasBookConvert()
{
    LOG_INFO("FemasBookConvert::FemasBookConvert");
    m_listener = new FemasConvertListenerI();
    m_femas_book_manager = new fh::femas::market::CFemasBookManager((fh::core::market::MarketListenerI*)m_listener);	
    m_messagemap.clear();	
}

FemasBookConvert::~FemasBookConvert()
{
    LOG_INFO("FemasBookConvert::~FemasBookConvert");
    delete m_listener;
    delete m_femas_book_manager;	
}

//void FemasBookConvert::Add_listener(fh::core::market::MarketListenerI *listener)
//{
//    LOG_INFO("FemasBookConvert::Add_listener");
//    m_listener = listener;
//}

MessMap FemasBookConvert::Convert(const std::string &message)
{
     LOG_INFO("FemasBookConvert::Apply_message");
     LOG_INFO("FemasBookConvert::Apply_message str = ",message);	 
     m_messagemap.clear();
     m_listener->Reset();
	 
     auto doc = bsoncxx::from_json(message);	 
     auto json = doc.view();
     auto body = GET_SUB_FROM_JSON(json, "message");
     auto instrumentID = GET_STR_FROM_JSON(json, "InstrumentID");	
     auto volumeMultiple = GET_INT_FROM_JSON(json, "VolumeMultiple");
     auto is = GET_STR_FROM_JSON(json, "insertTime");
     auto se = GET_STR_FROM_JSON(json, "sendingTime");	
     auto sestr = GET_STR_FROM_JSON(json, "sendingTimeStr");	 
     FemasmarketData(body,volumeMultiple);
     bsoncxx::builder::basic::document tmp_l2;
     if(MakeL2Json(tmp_l2))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("offer")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime",T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_offer));
	  m_messagemap["offer"] =  bsoncxx::to_json(tmp_h.view());
     }	 
     bsoncxx::builder::basic::document tmp_bbo;		 
     if(MakeBboJson(tmp_bbo))
     {
         bsoncxx::builder::basic::document tmp_h;
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));	
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
	  tmp_h.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));	
	  tmp_h.append(bsoncxx::builder::basic::kvp("type", T("trade")));	 
         tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
         tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	  tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_trade));	
	  m_messagemap["trade"] =  bsoncxx::to_json(tmp_h.view());
     }	 	 
   

     return m_messagemap;	 
}

void FemasBookConvert::FemasmarketData(const JSON_ELEMENT &message,int volumeMultiple)
{
    LOG_INFO("FemasBookConvert::FemasmarketData");
    CUstpFtdcDepthMarketDataField tmpMarketData;
    memset(&tmpMarketData,0,sizeof(CUstpFtdcDepthMarketDataField));
    strcpy(tmpMarketData.InstrumentID,GET_STR_FROM_JSON(message, "InstrumentID").c_str());
    strcpy(tmpMarketData.TradingDay,GET_STR_FROM_JSON(message, "TradingDay").c_str());
    strcpy(tmpMarketData.SettlementGroupID,GET_STR_FROM_JSON(message, "SettlementGroupID").c_str());	
    tmpMarketData.SettlementID=GET_INT_FROM_JSON(message, "SettlementID");
    tmpMarketData.PreSettlementPrice=GET_DOUBLE_FROM_JSON(message, "PreSettlementPrice");	
    tmpMarketData.PreClosePrice=GET_DOUBLE_FROM_JSON(message, "PreClosePrice");
    tmpMarketData.PreOpenInterest=GET_DOUBLE_FROM_JSON(message, "PreOpenInterest");
    tmpMarketData.PreDelta=GET_DOUBLE_FROM_JSON(message, "PreDelta");
    tmpMarketData.OpenPrice=GET_DOUBLE_FROM_JSON(message, "OpenPrice");	
    tmpMarketData.HighestPrice=GET_DOUBLE_FROM_JSON(message, "HighestPrice");	
    tmpMarketData.LowestPrice=GET_DOUBLE_FROM_JSON(message, "LowestPrice");
    tmpMarketData.ClosePrice=GET_DOUBLE_FROM_JSON(message, "ClosePrice");
    tmpMarketData.UpperLimitPrice=GET_DOUBLE_FROM_JSON(message, "UpperLimitPrice");
    tmpMarketData.LowerLimitPrice=GET_DOUBLE_FROM_JSON(message, "LowerLimitPrice");
    tmpMarketData.SettlementPrice=GET_DOUBLE_FROM_JSON(message, "SettlementPrice");
    tmpMarketData.CurrDelta=GET_DOUBLE_FROM_JSON(message, "CurrDelta");
    tmpMarketData.LastPrice=GET_DOUBLE_FROM_JSON(message, "LastPrice");
    tmpMarketData.Volume=GET_INT_FROM_JSON(message, "Volume");
    tmpMarketData.Turnover=GET_DOUBLE_FROM_JSON(message, "Turnover");
    tmpMarketData.OpenInterest=GET_DOUBLE_FROM_JSON(message, "OpenInterest");
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
    strcpy(tmpMarketData.UpdateTime,GET_STR_FROM_JSON(message, "UpdateTime").c_str());	
    tmpMarketData.UpdateMillisec=GET_INT_FROM_JSON(message, "UpdateMillisec");	
    strcpy(tmpMarketData.ActionDay,GET_STR_FROM_JSON(message, "ActionDay").c_str());
    tmpMarketData.HisHighestPrice=GET_DOUBLE_FROM_JSON(message, "HisHighestPrice");
    tmpMarketData.HisLowestPrice=GET_DOUBLE_FROM_JSON(message, "HisLowestPrice");
    tmpMarketData.LatestVolume=GET_INT_FROM_JSON(message, "LatestVolume");
    tmpMarketData.InitVolume=GET_INT_FROM_JSON(message, "InitVolume");
    tmpMarketData.ChangeVolume=GET_INT_FROM_JSON(message, "ChangeVolume");
    tmpMarketData.BidImplyVolume=GET_INT_FROM_JSON(message, "BidImplyVolume");
    tmpMarketData.AskImplyVolume=GET_INT_FROM_JSON(message, "AskImplyVolume");
    tmpMarketData.AvgPrice=GET_DOUBLE_FROM_JSON(message, "AvgPrice");
    tmpMarketData.ArbiType=GET_CHAR_FROM_JSON(message,"ArbiType");
    tmpMarketData.TotalBidVolume=GET_INT_FROM_JSON(message, "TotalBidVolume");
    tmpMarketData.TotalAskVolume=GET_INT_FROM_JSON(message, "TotalAskVolume");
    strcpy(tmpMarketData.InstrumentID_1,GET_STR_FROM_JSON(message, "InstrumentID_1").c_str());
    strcpy(tmpMarketData.InstrumentID_2,GET_STR_FROM_JSON(message, "InstrumentID_2").c_str());
    strcpy(tmpMarketData.InstrumentName,GET_STR_FROM_JSON(message, "InstrumentName").c_str());	
 
    int BidVolume_x = 0;
    int AskVolume_y = 0;	
    if(volumeMultiple <= 0)
    {
        AskVolume_y = 0;
	 BidVolume_x = 0;
    }
    else
    {
        try
       {
            AskVolume_y = (tmpMarketData.Turnover-tmpMarketData.BidPrice1*tmpMarketData.Volume*volumeMultiple)/((tmpMarketData.AskPrice1-tmpMarketData.BidPrice1)*volumeMultiple);                  
       }
       catch(...)
       {
           AskVolume_y = 0;
       }
	BidVolume_x = tmpMarketData.Volume-AskVolume_y;     
    }		

    if(BidVolume_x < 0)
    {
        BidVolume_x = 0;
	 AskVolume_y = 0;	
    }	
    if(AskVolume_y < 0)
    {
        BidVolume_x = 0;
        AskVolume_y = 0;
    }
    m_listener->bid_turnover = BidVolume_x;
    m_listener->offer_turnover = AskVolume_y;	
	
    m_femas_book_manager->SendFemasmarketData(&tmpMarketData);
	
    return;	
}

bool FemasBookConvert::MakeL2Json(bsoncxx::builder::basic::document& json)
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
    json.append(bsoncxx::builder::basic::kvp("bid_turnover", T(std::to_string(m_listener->bid_turnover)))); 	
    json.append(bsoncxx::builder::basic::kvp("offer_turnover", T(std::to_string(m_listener->offer_turnover)))); 	
	
    return true;	
}
bool FemasBookConvert::MakeBidJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("FemasBookConvert::MakeBidJson");
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
bool FemasBookConvert::MakeOfferJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("FemasBookConvert::MakeOfferJson");
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
bool FemasBookConvert::MakeBboJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("FemasBookConvert::MakeBboJson");
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

bool FemasBookConvert::MakeTradeJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("FemasBookConvert::MakeTradeJson");
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
} // namespace market
} // namespace femas
} // namespace fh
