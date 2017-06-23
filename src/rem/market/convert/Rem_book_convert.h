#ifndef __FH_REM_MARKET_CONVERT_REM_BOOK_CONVERT_H__
#define __FH_REM_MARKET_CONVERT_REM_BOOK_CONVERT_H__
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <map>
#include <vector>
#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include "EESQuoteApi.h"
#include "rem/market/rem_market_manager.h"
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

typedef std::map <std::string,std::string> MessMap;

namespace fh
{
namespace rem
{
namespace market
{
namespace convert
{

template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

    class RemConvertListenerI : public fh::core::market::MarketListenerI 
    {
        public:
            RemConvertListenerI();
            virtual ~RemConvertListenerI();

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
            void OnOrginalMessage(const std::string &message) override;
            // implement of MarketListenerI
            void OnTurnover(const pb::dms::Turnover &turnover) override;

	     void Reset();

        public:
            //fh::core::zmq::ZmqSender m_org_sender;
            //fh::core::zmq::ZmqSender m_book_sender;
            pb::dms::BBO m_bbo;
	     pb::dms::Bid m_bid;
	     pb::dms::Offer m_offer;
	     pb::dms::L2 m_l2;	
	     pb::dms::Trade m_trade;	 
		 
            int bid_turnover;
	     int offer_turnover;		
	 

        private:
            DISALLOW_COPY_AND_ASSIGN(RemConvertListenerI);
			
    };

    class RemBookConvert : public fh::core::persist::Converter
    {
        public:
            RemBookConvert();
            virtual ~RemBookConvert();
        public:	
	     //void Add_listener(fh::core::market::MarketListenerI *listener);
	     MessMap Convert(const std::string &message) override;
	     //MessMap Apply_message(const std::string &message);
	     void RemmarketData(const JSON_ELEMENT &message);
	     bool MakeL2Json(bsoncxx::builder::basic::document& json);
	     bool MakeBidJson(bsoncxx::builder::basic::document& json);
	     bool MakeOfferJson(bsoncxx::builder::basic::document& json); 	 
	     bool MakeBboJson(bsoncxx::builder::basic::document& json); 
	     bool MakeTradeJson(bsoncxx::builder::basic::document& json); 	 
	 private:
	     RemConvertListenerI* m_listener;	 
	     fh::rem::market::CRemBookManager *m_rem_book_manager;	
	     MessMap m_messagemap;
	 private:
            DISALLOW_COPY_AND_ASSIGN(RemBookConvert);	 
    };	

} // namespace convert
} // namespace rem
} // namespace femas
} // namespace fh

#endif // __FH_REM_MARKET_CONVERT_REM_BOOK_CONVERT_H__


