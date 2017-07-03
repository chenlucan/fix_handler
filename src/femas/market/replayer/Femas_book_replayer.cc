
#include "Femas_book_replayer.h"
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
namespace replayer
{

FemasBookReplayer::FemasBookReplayer()
: m_listener(nullptr)	
{

}

FemasBookReplayer::~FemasBookReplayer()
{

}

void FemasBookReplayer::Add_listener(fh::core::market::MarketListenerI *listener)
{
    LOG_INFO("FemasBookReplayer::Add_listener");
    m_listener = listener;
    m_femas_book_manager = new fh::femas::market::CFemasBookManager(m_listener);	
    return;
}

void FemasBookReplayer::Apply_message(const std::string &message)
{
    LOG_INFO("FemasBookReplayer::Apply_message");
    auto doc = bsoncxx::from_json(message);
    auto json = doc.view();
    auto body = GET_SUB_FROM_JSON(json, "message");
    auto instrumentID = GET_STR_FROM_JSON(json, "InstrumentID");	
    auto volumeMultiple = GET_INT_FROM_JSON(json, "VolumeMultiple");
    FemasmarketData(body,volumeMultiple);	
    return;
}

void FemasBookReplayer::FemasmarketData(const JSON_ELEMENT &message,int volumeMultiple)
{
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

    std::string tmpmessage = std::to_string(BidVolume_x)+",";
    tmpmessage += std::to_string(AskVolume_y);	

    m_femas_book_manager->SendFemasToDB(tmpmessage);
	
    m_femas_book_manager->SendFemasmarketData(&tmpMarketData);
    return;
}



} // namespace replayer
} // namespace market
} // namespace femas
} // namespace fh

