#include "book_convert.h"
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
namespace ctp
{
namespace market
{
namespace convert
{

CtpBookConvert::CtpConvertListenerI::CtpConvertListenerI()
{

}

CtpBookConvert::CtpConvertListenerI::~CtpConvertListenerI()
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnMarketDisconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnMarketReconnect(core::market::MarketI* market)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnContractDefinition(const pb::dms::Contract &contract)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnBBO(const pb::dms::BBO &bbo)
{
    m_bbo.Clear();
    m_bbo = bbo;
}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnBid(const pb::dms::Bid &bid)
{
    m_bid.Clear();
    m_bid = bid;
}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnOffer(const pb::dms::Offer &offer)
{
    m_offer.Clear();
    m_offer = offer;
}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnL2(const pb::dms::L2 &l2)
{
    m_l2.Clear();
    m_l2 = l2; 
}

// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnTurnover(const pb::dms::Turnover &turnover)
{
    LOG_INFO("OnTurnover: ", fh::core::assist::utility::Format_pb_message(turnover));
    m_turnover.Clear();
    m_turnover = turnover;	
}

// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnL3()
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnTrade(const pb::dms::Trade &trade)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnContractAuctioning(const std::string &contract)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnContractNoTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnContractTrading(const std::string &contract)
{

}
// implement of MarketListenerI
void CtpBookConvert::CtpConvertListenerI::OnOrginalMessage(const std::string &message)
{
    LOG_INFO("CtpConvertListenerI::OnOrginalMessage");
}

void CtpBookConvert::CtpConvertListenerI::Reset()
{
    LOG_INFO("CtpConvertListenerI::Reset");
    m_bbo.Clear();
    m_bid.Clear();
    m_offer.Clear();
    m_l2.Clear();
    m_trade.Clear();	
    m_turnover.Clear();	
}

//===================================================================================
CtpBookConvert::CtpBookConvert()
{
    LOG_INFO("CtpBookConvert::CtpBookConvert");
    m_listener = new  CtpBookConvert::CtpConvertListenerI();	
    m_messagemap.clear();
    m_trademap.clear();	
}

CtpBookConvert::~CtpBookConvert()
{
    LOG_INFO("CtpBookConvert::~CtpBookConvert");
    delete m_listener;
}

//void CtpBookConvert::Add_listener(fh::core::market::MarketListenerI *listener)
//{
//    LOG_INFO("CtpBookConvert::Add_listener");
//    m_listener = listener;
//}

MessMap CtpBookConvert::Convert(const std::string &message)
{
     LOG_INFO("CtpBookConvert::Apply_message");
     LOG_INFO("CtpBookConvert::Apply_message str = ",message);	 
     m_messagemap.clear();
     m_listener->Reset();
	 
     auto doc = bsoncxx::from_json(message);	 
     auto json = doc.view();
     auto body = GET_SUB_FROM_JSON(json, "message");
     auto instrumentID = GET_STR_FROM_JSON(json, "InstrumentID");	
     auto is = GET_STR_FROM_JSON(json, "insertTime");
     auto se = GET_STR_FROM_JSON(json, "sendingTime");	
     auto sestr = GET_STR_FROM_JSON(json, "sendingTimeStr");	 
     CtpMarketData(body);
	 
     bsoncxx::builder::basic::document tmp_bid;	 
     if(MakeBidJson(tmp_bid))
     {
        bsoncxx::builder::basic::document tmp_h;
        tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
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
	    tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
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
	    tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
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
	    tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
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
	    tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
	    tmp_h.append(bsoncxx::builder::basic::kvp("type", T("turnover")));	 
        tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
        tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
        tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	    tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_turnover));	
	    m_messagemap["turnover"] =  bsoncxx::to_json(tmp_h.view());
     }	
     bsoncxx::builder::basic::document tmp_l2;
     if(MakeL2Json(tmp_l2))
     {
        bsoncxx::builder::basic::document tmp_h;
	    tmp_h.append(bsoncxx::builder::basic::kvp("market", T("CTP")));	
	    tmp_h.append(bsoncxx::builder::basic::kvp("type", T("l2")));	 
        tmp_h.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
        tmp_h.append(bsoncxx::builder::basic::kvp("sendingTime", T(se)));	
        tmp_h.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(sestr)));
	    tmp_h.append(bsoncxx::builder::basic::kvp("message", tmp_l2));	
	    m_messagemap["l2"] =  bsoncxx::to_json(tmp_h.view());
     }	 	 
   

     return m_messagemap;	 
}

void CtpBookConvert::CtpMarketData(const JSON_ELEMENT &message)
{
    LOG_INFO("CtpBookConvert::CtpMarketData");
    CThostFtdcDepthMarketDataField tmpMarketData;
    memset(&tmpMarketData,0,sizeof(CThostFtdcDepthMarketDataField));
    strcpy(tmpMarketData.InstrumentID,GET_STR_FROM_JSON(message, "InstrumentID").c_str());
    strcpy(tmpMarketData.TradingDay,GET_STR_FROM_JSON(message, "TradingDay").c_str());
 //   strcpy(tmpMarketData.SettlementGroupID,GET_STR_FROM_JSON(message, "SettlementGroupID").c_str());	
 //   tmpMarketData.SettlementID=GET_INT_FROM_JSON(message, "SettlementID");
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
//    tmpMarketData.HisHighestPrice=GET_DOUBLE_FROM_JSON(message, "HisHighestPrice");
//    tmpMarketData.HisLowestPrice=GET_DOUBLE_FROM_JSON(message, "HisLowestPrice");
//    tmpMarketData.LatestVolume=GET_INT_FROM_JSON(message, "LatestVolume");
//    tmpMarketData.InitVolume=GET_INT_FROM_JSON(message, "InitVolume");
//    tmpMarketData.ChangeVolume=GET_INT_FROM_JSON(message, "ChangeVolume");
//    tmpMarketData.BidImplyVolume=GET_INT_FROM_JSON(message, "BidImplyVolume");
//    tmpMarketData.AskImplyVolume=GET_INT_FROM_JSON(message, "AskImplyVolume");
//    tmpMarketData.AvgPrice=GET_DOUBLE_FROM_JSON(message, "AvgPrice");
//    tmpMarketData.ArbiType=GET_CHAR_FROM_JSON(message,"ArbiType");
//    tmpMarketData.TotalBidVolume=GET_INT_FROM_JSON(message, "TotalBidVolume");
//    tmpMarketData.TotalAskVolume=GET_INT_FROM_JSON(message, "TotalAskVolume");
//    strcpy(tmpMarketData.InstrumentID_1,GET_STR_FROM_JSON(message, "InstrumentID_1").c_str());
//    strcpy(tmpMarketData.InstrumentID_2,GET_STR_FROM_JSON(message, "InstrumentID_2").c_str());
//    strcpy(tmpMarketData.InstrumentName,GET_STR_FROM_JSON(message, "InstrumentName").c_str());	
    SendDepthMarketData(&tmpMarketData);
	
    return;	
}

bool CtpBookConvert::MakeL2Json(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("CtpBookConvert::MakeL2Json");
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
bool CtpBookConvert::MakeBidJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("CtpBookConvert::MakeBidJson");
    if(!(m_listener->m_bid).has_contract())
    {
        return false;
    }		
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_bid.contract()))); 

    bsoncxx::builder::basic::array tmarray_b;
    bsoncxx::builder::basic::document tmjsona1;
    tmjsona1.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bid.bid().price())))); 	
    tmjsona1.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bid.bid().size())))); 
    tmarray_b.append(tmjsona1);
    json.append(bsoncxx::builder::basic::kvp("bid", tmarray_b));
	
    return true;	
}
bool CtpBookConvert::MakeOfferJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("CtpBookConvert::MakeOfferJson");
    if(!(m_listener->m_offer).has_contract())
    {
        return false;
    }			
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_offer.contract()))); 

    bsoncxx::builder::basic::array tmarray_a;
    bsoncxx::builder::basic::document tmjsona2;
    tmjsona2.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_offer.offer().price())))); 	
    tmjsona2.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_offer.offer().size())))); 
    tmarray_a.append(tmjsona2);
    json.append(bsoncxx::builder::basic::kvp("offer", tmarray_a));   

    return true;	
}
bool CtpBookConvert::MakeBboJson(bsoncxx::builder::basic::document& json) 
{
    LOG_INFO("CtpBookConvert::MakeBboJson");
    if(!(m_listener->m_bbo).has_contract())
    {
        return false;
    } 	
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_bbo.contract()))); 

    bsoncxx::builder::basic::array tmarray_b;
    bsoncxx::builder::basic::document tmjsona1;
    tmjsona1.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bbo.bid().price())))); 	
    tmjsona1.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bbo.bid().size())))); 
    tmarray_b.append(tmjsona1);
    json.append(bsoncxx::builder::basic::kvp("bid", tmarray_b));

    bsoncxx::builder::basic::array tmarray_a;
    bsoncxx::builder::basic::document tmjsona2;
    tmjsona2.append(bsoncxx::builder::basic::kvp("price", T(std::to_string(m_listener->m_bbo.offer().price())))); 	
    tmjsona2.append(bsoncxx::builder::basic::kvp("size", T(std::to_string(m_listener->m_bbo.offer().size())))); 
    tmarray_a.append(tmjsona2);
    json.append(bsoncxx::builder::basic::kvp("offer", tmarray_a));

    return true;
}

bool CtpBookConvert::MakeTradeJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("CtpBookConvert::MakeTradeJson");
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

bool CtpBookConvert::MakeTurnoverJson(bsoncxx::builder::basic::document& json)
{
    LOG_INFO("CtpBookConvert::MakeTurnoverJson");
    if(!(m_listener->m_turnover).has_contract())
    {
        return false;
    } 	
    json.append(bsoncxx::builder::basic::kvp("contract", T(m_listener->m_turnover.contract()))); 
    json.append(bsoncxx::builder::basic::kvp("total_volume", T(m_listener->m_turnover.total_volume()))); 
    json.append(bsoncxx::builder::basic::kvp("turnover", T(m_listener->m_turnover.turnover()))); 
	
    return true;
}

void CtpBookConvert::SendDepthMarketData(CThostFtdcDepthMarketDataField *pMarketData)
{
	LOG_INFO("CCtpBookManager::SendCtpmarketData ");     
	if(NULL == pMarketData)
	{
       	        LOG_INFO("Error pMarketData is NULL ");
		return;
	}
	pb::dms::L2 l2_info;

       l2_info.set_contract(pMarketData->InstrumentID);

	   
	pb::dms::DataPoint *bid;// = l2_info.add_bid();
	pb::dms::DataPoint *ask;// = l2_info.add_offer();
	
	if (pMarketData->BidPrice1==DBL_MAX || pMarketData->BidVolume1 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice1);
	    bid->set_size(pMarketData->BidVolume1);	   
	}	
		
	if (pMarketData->AskPrice1==DBL_MAX || pMarketData->AskVolume1 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice1);
	    ask->set_size(pMarketData->AskVolume1);	   
	}
	

	if (pMarketData->BidPrice2==DBL_MAX || pMarketData->BidVolume2 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice2);
	    bid->set_size(pMarketData->BidVolume2);	   
	}	
	

	if (pMarketData->AskPrice2==DBL_MAX || pMarketData->AskVolume2 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice2);
	    ask->set_size(pMarketData->AskVolume2);	   
	}
	
	if (pMarketData->BidPrice3==DBL_MAX || pMarketData->BidVolume3 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice3);
	    bid->set_size(pMarketData->BidVolume3);	   
	}	
	

	if (pMarketData->AskPrice3==DBL_MAX || pMarketData->AskVolume3 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice3);
	    ask->set_size(pMarketData->AskVolume3);	   
	}
	
	if (pMarketData->BidPrice4==DBL_MAX || pMarketData->BidVolume4 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice4);
	    bid->set_size(pMarketData->BidVolume4);	   
	}	
	

	if (pMarketData->AskPrice4==DBL_MAX || pMarketData->AskVolume4 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice4);
	    ask->set_size(pMarketData->AskVolume4);	   
	}
	
	if (pMarketData->BidPrice5==DBL_MAX || pMarketData->BidVolume5 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice5);
	    bid->set_size(pMarketData->BidVolume5);	   
	}	
	

	if (pMarketData->AskPrice5==DBL_MAX || pMarketData->AskVolume5 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice5);
	    ask->set_size(pMarketData->AskVolume5);	   
	}
	
	//m_listener->OnL2(l2_info);

	//以上发送L2 行情

	//发送最优价
	if((pMarketData->BidPrice1 == DBL_MAX || pMarketData->BidVolume1 <= 0) && (pMarketData->AskVolume1 <= 0 || pMarketData->AskPrice1 == DBL_MAX))
	{
           LOG_INFO("Bid and Offer NULL ");
	}
	else
	if(pMarketData->BidPrice1 == DBL_MAX || pMarketData->BidVolume1 <= 0)	
	{
           pb::dms::Offer offer_info;
	    offer_info.set_contract(pMarketData->InstrumentID);	   
	    pb::dms::DataPoint *offer = offer_info.mutable_offer();
	    offer->set_price(pMarketData->AskPrice1);
	    offer->set_size(pMarketData->AskVolume1);	
	    m_listener->OnOffer(offer_info);
	}
	else
	if(pMarketData->AskPrice1 == DBL_MAX || pMarketData->AskVolume1 <= 0)	
	{
           pb::dms::Bid bid_info;
	    bid_info.set_contract(pMarketData->InstrumentID);	   
	    pb::dms::DataPoint *bid = bid_info.mutable_bid();
	    bid->set_price(pMarketData->BidPrice1);
	    bid->set_size(pMarketData->BidVolume1);	
	    m_listener->OnBid(bid_info);
	}	
	else
	{
           pb::dms::BBO bbo_info;
	    bbo_info.set_contract(pMarketData->InstrumentID);		
           pb::dms::DataPoint *bid = bbo_info.mutable_bid();
	    bid->set_price(pMarketData->BidPrice1);
	    bid->set_size(pMarketData->BidVolume1);	
           pb::dms::DataPoint *ask = bbo_info.mutable_offer();
           ask->set_price(pMarketData->AskPrice1);
           ask->set_size(pMarketData->AskVolume1);	   
           m_listener->OnBBO(bbo_info);
		
	}

	//发送teade行情
	int tmpvolume = MakePriceVolume(pMarketData);
	LOG_INFO("CtpBookConvert::MakePriceVolume = ",tmpvolume); 
	if(tmpvolume > 0)
	{
            pb::dms::Trade trade_info;
	     trade_info.set_contract(pMarketData->InstrumentID);	
	     pb::dms::DataPoint *trade_id = trade_info.mutable_last();	
	     trade_id->set_price(pMarketData->LastPrice);
	     trade_id->set_size(tmpvolume);	
	     m_listener->OnTrade(trade_info);	 
	}

       pb::dms::Turnover Turnoverinfo;
       Turnoverinfo.set_contract(pMarketData->InstrumentID);
	Turnoverinfo.set_total_volume(pMarketData->Volume);
	Turnoverinfo.set_turnover(pMarketData->Turnover);
	m_listener->OnTurnover(Turnoverinfo);
	   
	m_listener->OnL2(l2_info);
}

void CtpBookConvert::CheckTime(CThostFtdcDepthMarketDataField *pMarketData)
{
    LOG_INFO("CtpBookConvert::CheckTime "); 
    char ctmpf[3]={0};
    char ctmps[3]={0};	
    strncpy(ctmpf,pMarketData->UpdateTime,2);	
    strncpy(ctmps,(m_trademap[pMarketData->InstrumentID]->mtime).c_str(),2);		
    if(std::atoi(ctmpf) > 18 && std::atoi(ctmps) < 18)
    {
        LOG_INFO("CtpBookConvert::clear  Instrument map");
        m_trademap[pMarketData->InstrumentID]->mvolume=pMarketData->Volume;
	 m_trademap[pMarketData->InstrumentID]->mtime=pMarketData->UpdateTime;	
    }
}

//void CtpBookConvert::ClearMap()
//{
//    m_trademap.clear();
//}

int CtpBookConvert::MakePriceVolume(CThostFtdcDepthMarketDataField *pMarketData)
{
    LOG_INFO("CtpBookConvert::MakePriceVolume "); 
    if(m_trademap.count(pMarketData->InstrumentID) == 0)
    {
        LOG_INFO("CtpBookConvert::insert map InstrumentID = ",pMarketData->InstrumentID); 
        m_trademap[pMarketData->InstrumentID] = new mstrade();
	 m_trademap[pMarketData->InstrumentID]->mvolume=pMarketData->Volume;
	 m_trademap[pMarketData->InstrumentID]->mtime=pMarketData->UpdateTime;
        return 0;
    }
    else
    {
        CheckTime(pMarketData);
	 LOG_INFO("pMarketData->Volume =  ",pMarketData->Volume); 	
	 LOG_INFO("m_trademap->Volume =  ",m_trademap[pMarketData->InstrumentID]->mvolume); 
     int tmpVolume =  pMarketData->Volume - m_trademap[pMarketData->InstrumentID]->mvolume;
	 m_trademap[pMarketData->InstrumentID]->mvolume=pMarketData->Volume;
	 m_trademap[pMarketData->InstrumentID]->mtime=pMarketData->UpdateTime;	
        return (tmpVolume > 0 ? tmpVolume : 0);     
    }		
}

} // namespace convert
} // namespace market
} // namespace ctp
} // namespace fh
