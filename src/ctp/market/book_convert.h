#ifndef __FH_CTP_MARKET_REPLAYER_CTP_BOOK_CONVERT_H__
#define __FH_CTP_MARKET_REPLAYER_CTP_BOOK_CONVERT_H__
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include <vector>
#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include "ThostFtdcMdApi.h"
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include "core/market/marketlisteneri.h"
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/assist/utility.h"
#include "core/global.h"
#include "core/persist/converter.h"


#define JSON_ELEMENT bsoncxx::document::element

namespace fh
{
namespace ctp
{
namespace market
{
namespace convert
{
	
typedef struct strade
{
    int mvolume;
    std::string mtime;
    strade()
    {
        mvolume = 0;
	 mtime = "";	
    }
} mstrade;

typedef std::map <std::string,mstrade*> TradeMap;

typedef std::map <std::string,std::string> MessMap;	
    
template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}
	


class CtpBookConvert : public fh::core::persist::Converter
{
	public:
		class CtpConvertListenerI : public fh::core::market::MarketListenerI 
		{
			public:
				CtpConvertListenerI();
				virtual ~CtpConvertListenerI();

			public:
				// implement of MarketListenerI
				void OnMarketDisconnect(core::market::MarketI* market) override;
				// implement of MarketListenerI
				void OnMarketReconnect(core::market::MarketI* market) override;
				// implement of MarketListenerI
				void OnContractDefinition(const pb::dms::Contract &contract) override;
				// implement of MarketListenerI
				void OnBBO(const pb::dms::BBO &bbo) override;
				// implement of MarketListenerI
				void OnBid(const pb::dms::Bid &bid) override;
				// implement of MarketListenerI
				void OnOffer(const pb::dms::Offer &offer) override;
				// implement of MarketListenerI
				void OnL2(const pb::dms::L2 &l2) override;
				// implement of MarketListenerI
				void OnL3() override;
				// implement of MarketListenerI
				void OnTrade(const pb::dms::Trade &trade) override;
				// implement of MarketListenerI
				void OnContractAuctioning(const std::string &contract) override;
				// implement of MarketListenerI
				void OnContractNoTrading(const std::string &contract)  override;
				// implement of MarketListenerI
				void OnContractTrading(const std::string &contract)    override;
				// implement of MarketListenerI
				virtual void OnOrginalMessage(const std::string &message);
				// implement of MarketListenerI
                void OnTurnover(const pb::dms::Turnover &turnover) override;
				void Reset();

			public:
				pb::dms::BBO m_bbo;
				pb::dms::Bid m_bid;
				pb::dms::Offer m_offer;
				pb::dms::L2 m_l2;	 
				pb::dms::Trade m_trade;	 
				pb::dms::Turnover m_turnover;	
				int bid_turnover;
				int offer_turnover;		
 
			private:
			DISALLOW_COPY_AND_ASSIGN(CtpConvertListenerI);

		};

	public:
		CtpBookConvert();
		virtual ~CtpBookConvert();
	public:	
		MessMap Convert(const std::string &message) override;
		void CtpMarketData(const JSON_ELEMENT &message,int volumeMultiple=0);
		bool MakeL2Json(bsoncxx::builder::basic::document& json);
		bool MakeBidJson(bsoncxx::builder::basic::document& json);
		bool MakeOfferJson(bsoncxx::builder::basic::document& json); 	 
		bool MakeBboJson(bsoncxx::builder::basic::document& json);
        bool MakeTradeJson(bsoncxx::builder::basic::document& json);	
        bool MakeTurnoverJson(bsoncxx::builder::basic::document& json); 		
        void SendDepthMarketData(CThostFtdcDepthMarketDataField *pMarketData);
		int  MakePriceVolume(CThostFtdcDepthMarketDataField *pMarketData);
		void CheckTime(CThostFtdcDepthMarketDataField *pMarketData);
		
	private:
		CtpConvertListenerI* m_listener;	 	
		MessMap m_messagemap;	
        TradeMap m_trademap;			
	private:
		DISALLOW_COPY_AND_ASSIGN(CtpBookConvert);
};


} // namespace convert
} // namespace market
} // namespace Ctp
} // namespace fh

#endif // __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_CONVERT_H__
